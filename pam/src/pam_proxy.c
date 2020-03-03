/*
* AUTHOR: zakuarbor (Ju Hong Kim)
* DESC: PAM module for the ProxyAuth project. Authenticates users via bluetooth proximity
*/


#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h> 
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <glib.h>
#include <gio/gio.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define BT_MAC_LEN 17
#define BT_MAX_CONN 7 //Bluetooth Adapters can only connect up to 7 devices

#define LOG 1

const char *trusted_dir_path = "/etc/proxy_auth/";

/*
* Set the login time to the given pointer
* Format: %a %b %d %H:%M:%S %Y
* Example: Fri Feb 11 02:59:18 2020
*
* %a: locale's abbreviated weekday name (e.g., Sun)
* %b: locale's abbreviated month name (e.g., Jan)
* %d: day of month (e.g., 01)
* %H: hour (00..23)
* %M: minute (00..59)
* %S: second (00..60)
* %Y: year
* Credits: `man date` for the format specifier
*/
void get_login_time(char *curr_time) {
    time_t raw_time;
    struct tm *info_time;
    time(&raw_time);
    info_time = localtime(&raw_time);
    int time_len = strlen(asctime(info_time));
    strncpy(curr_time, asctime(info_time), time_len);
    curr_time[(time_len - 1)] = '\0'; //asctime appends a \n to the string
}

/*
* Return 1 iff the given address is one of the trusted devices the user trusts
*
* @param log_fp: the handle of the log file
* @param trusted_devices: the array of trusted bluetooth MAC addresses of the user
* @param num_of_devices: the number of devices the user trusts
* @return: 1 iff the given address is one of the trusted devices the user trusts. Else return 0
*/
int is_dev_trusted(FILE *log_fp, char *dev, char **trusted_devices, int num_of_devices) {
    for (int i = 0; i < num_of_devices; i++) {
        if (strcmp(dev, trusted_devices[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
* Return 1 iff a trusted device for the user is detected
*
* @param log_fp: the handle of the log file
* @param trusted_devices: the array of trusted bluetooth MAC addresses of the user
* @param num_of_devices: the number of devices the user trusts
* @param detected_dev: the address of the detected device
*   NOTE: detected_dev will be set in this function if a device was detected
* @return: 1 iff a trusted device for the user is detected
*/
int find_device(FILE *log_fp, char **trusted_devices, int num_of_devices, char **detected_dev) {
    if (!trusted_devices) {
        return 0;
    }

    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    int trusted_dev_found = 0;

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        return(0);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if(num_rsp < 0) {
        if (log_fp) {
            fprintf(log_fp, "Error: hci_inquiry\n");
        }
        perror("hci_inquiry");
    }
    
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);

        if (is_dev_trusted(log_fp, addr, trusted_devices, num_of_devices)) {
            if (log_fp) {
                fprintf(log_fp, "Trusted Device: %s\n", addr);
            }
            if ((*detected_dev = malloc(sizeof(char) * (BT_MAC_LEN + 1)))) {
                strncpy(*detected_dev, addr, BT_MAC_LEN);
                (*detected_dev)[BT_MAC_LEN] = '\0';
            }
            trusted_dev_found = 1;
            break;
        }
    }

    if (ii) {
        free(ii);
    }
    if (sock) {
        close(sock);
    }

    return trusted_dev_found;
}

/*
* Return Bluetooth address if the device is paired. Else return NULL
*
* @param properties: An 'object' that holds a specific bluetooth device's metadata
* @return: Return Bluetooth address if the device is paired. Else return NULL
*/
char *check_is_paired(GVariant *properties) {
    char *addr = NULL;

    GVariantDict prop_dict;
    GVariant *is_connected_gvariant;
    GVariant *is_trusted_gvariant;
    GVariant *addr_gvariant;

    g_variant_dict_init(&prop_dict, properties);
    
    if (!(is_connected_gvariant = g_variant_dict_lookup_value(&prop_dict, "Connected", G_VARIANT_TYPE("b")))) {
        perror("No such key or Unexpected type");
        return NULL;
    }
    
    if (!(is_trusted_gvariant = g_variant_dict_lookup_value(&prop_dict, "Trusted", G_VARIANT_TYPE("b")))) {
        perror("No such key or Unexpected type");
        return NULL;
    }
    
    if (g_variant_get_boolean(is_connected_gvariant) && g_variant_get_boolean(is_trusted_gvariant)) {
        if (!(addr_gvariant = g_variant_dict_lookup_value(&prop_dict, "Address", G_VARIANT_TYPE("s")))) {
            perror("No such key or Unexpected type");
            return NULL;
        }

        g_print("pika: %s\n", g_variant_get_string(addr_gvariant, NULL));

        if (!(addr = malloc(sizeof(char) * BT_MAC_LEN + 1))) {
            perror("malloc");
            return NULL;
        }

        strncpy(addr, (char *)g_variant_get_string(addr_gvariant, NULL), BT_MAC_LEN);
        addr[BT_MAC_LEN] = '\0';
    }

    return addr;
}

/*
*Return a list of paired devices
* 
* @param num_of_paired: the number of devices that are paired
*   NOTE: The number is set in this function
* @param result: The object returned by request to DBUS for all managed bluetooth object
* @return: returns a list of paired devices. Need to free
*/
char **process_dbus_bt_list(GVariant *result, int *num_of_paired) {
    if(!result) {
        return NULL;
    }

    char **paired_devices = NULL;
    int num_of_paired_lc = 0;

    if (!(paired_devices = malloc(sizeof(char *) *BT_MAX_CONN))) {
        perror("malloc");
        return NULL;
    }

    GVariantIter i;
    const gchar *object_path;
    GVariant *ifaces_and_properties;

    result = g_variant_get_child_value(result, 0);
    g_variant_iter_init(&i, result);
    while(g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
            gchar *interface_name_lower = g_ascii_strdown(interface_name, -1);
            if(g_strstr_len(interface_name_lower, -1, "device")) {
                char *addr;
                if ((addr = check_is_paired(properties))) {
                    paired_devices[num_of_paired_lc] = addr;
                    num_of_paired_lc++;
                }
            }
            g_free(interface_name_lower);
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }

    *num_of_paired = num_of_paired_lc;
    return paired_devices;
}

/*
* Return a list of Bluetooth addresses that is currently paired (connected) to the host
*
* Approach: Use dbus to list all the bluetooth devices and see the connected property
*           The paired property only indicates if the device has been paired with the host before and not if it is currently paired
*
* @param log_fp: the handle of the log file
* @param num_of_paired: the number of devices paired to the host
*   NOTE: the value is set within the helper function that the function will call
* @return: return a list of bluetooth addresses connected to the host 
*/
char **get_paired_devices(FILE *log_fp, int *num_of_paired) {
    GDBusConnection *conn;

    GVariant *result = NULL;
    char **paired_devices = NULL;

    if(!(conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL))) {
        g_print("Not able to get connection to system bus\n");
        if (log_fp) {
            fprintf(log_fp, "Error: UNable to connect to system bus\n");   
        }

        return NULL;
    }
    if (log_fp) {
        fprintf(log_fp, "Connect to system bus\n");   
    }

    result = g_dbus_connection_call_sync(
        conn,                                   //connection
        "org.bluez",                            //bus_name
        "/",                                    //object_path
        "org.freedesktop.DBus.ObjectManager",   //interface_name
        "GetManagedObjects",                    //method_name
        NULL,                                   //parameters
        G_VARIANT_TYPE("(a{oa{sa{sv}}})"),      //return type
        G_DBUS_CALL_FLAGS_NONE,                 //flag
        1000,                                   //timeout_msec
        NULL,                                   //cancel error
        NULL                                    //error if parameter is not compatible with D-Bus protocol
    );
    if (!result) {
        exit(1);
    }

    paired_devices = process_dbus_bt_list(result, num_of_paired);

    g_variant_unref(result);

    g_object_unref(conn);

    return paired_devices;
}

/*
* Return 1 iff a trusted device for the user is connected
*
* @param log_fp: the handle of the log file
* @param trusted_devices: the array of trusted bluetooth MAC addresses of the user
* @param num_of_devices: the number of devices the user trusts
* @param detected_dev: the address of the detected device
*   NOTE: detected_dev will be set in this function if a device was detected
* @return: 1 iff a trusted device for the user is detected
*/
int find_trusted_paired_device(FILE *log_fp, char **trusted_devices, int num_of_devices, char **paired_devices, int num_of_paired, char **detected_dev) {
    if (!trusted_devices || !paired_devices) {
        return 0;
    }
    int trusted_dev_found = 0;

    for (int i = 0; i < num_of_paired; i++) {
        if (is_dev_trusted(log_fp, paired_devices[i], trusted_devices, num_of_devices)) {
            if (log_fp) {
                fprintf(log_fp, "Trusted Device: %s\n", paired_devices[i]);
            }
            if ((*detected_dev = malloc(sizeof(char) * (BT_MAC_LEN + 1)))) {
                strncpy(*detected_dev, paired_devices[i], BT_MAC_LEN);
                (*detected_dev)[BT_MAC_LEN] = '\0';
            }
            trusted_dev_found = 1;
            break;
        }
    }
    
    return trusted_dev_found;
}

/*
* Return the number of lines there are in the file
*
* Use Case: Each user will have a list of trusted bluetooth MAC stored in a file. E
*   Each line will contain a trusted bluetooth MAC address.
*   The number of lines in the file will represent the number of trusted devices
*
* @param FILE *fp: the file handle that contains the user's device
* @return: the number of lines there are in the file
*/
int get_num_lines(FILE *fp) {
    int num_lines = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (!fp) {
        return num_lines;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strcmp(line, "\n") != 0) {
            num_lines++;
        }
    }

    if (line) {
        free(line);
    }

    return num_lines;
}

/*
* Check if the directory exists. If not try to create the directory
*
* @param dir_path: The path to the directory
* @param log_fp: the handle of the log file
* @return:
*   -1: cannot access dir or file is not a directory or directory could not be created
*   0: created the directory
*   1: file exists and it is a directory
*/
int check_or_creat_dir(const char *dir_path, FILE *log_fp) {
    struct stat s;
    int status = -1;

    int exist = stat(dir_path, &s);
    if (exist == -1) {
        if(errno == ENOENT) {
            /* does not exist */
            if (mkdir(dir_path, 0755) == 0) {
                status = 0;
            }

            if (log_fp){
                fprintf(log_fp, "Created dir: %s\n", dir_path);    
            }
        } 
        else {
            fprintf(stderr, "Cannot access %s\n", dir_path);
            
            if (log_fp){
                fprintf(log_fp, "Cannot access %s\n", dir_path);    
            }
        }
    } 
    else if (!S_ISDIR(s.st_mode)) {
        fprintf(stderr, "Error: %s is not a directory\n", dir_path);

        if (log_fp) {
            fprintf(log_fp, "Error: %s is not a directory\n", dir_path);
        }
    }
    else {
        status = 1;
    }
    return status;
}

/*
* Return file pointer for the user's trusted bluetooth device
*
* @param trusted_dir_path: the path to where all the user's trusted device file is located
* @param username: the username
* @param log_fp: the handle for the log file
* @return: the file handle of the user's trusted device file
*/
FILE *get_trusted_dev_file(const char *trusted_dir_path, const char *username, FILE *log_fp) {
    FILE *trusted_dev_fp = NULL;

    char file_name[NAME_MAX];
    int len = 0;
    strcpy(file_name, "");
    
    if (strlen(trusted_dir_path) > 0) {
        strncat(file_name, trusted_dir_path, strlen(trusted_dir_path));
        len += strlen(trusted_dir_path);
    }
    if (strlen(username) > 0) {
        strncat(file_name, username, strlen(username));
        len += strlen(username);
    }
    //I am paranoid
    if (len > NAME_MAX) {
        file_name[NAME_MAX] = '\0';
    }
    else {
        file_name[len] = '\0';
    }

    if (!(trusted_dev_fp = fopen(file_name, "r"))) {
        perror("There are no trusted device");
        if (log_fp) {
            fprintf(log_fp, "File: %s does not exist. There are no trusted device for the user: %s\n", file_name, username);

        }
        fprintf(stderr, "File: %s does not exist. There are no trusted device for the user: %s\n", file_name, username);
        return NULL;
    }
    return trusted_dev_fp;
}

/*
* Free the list of bluetooth MAC addresses from memory
*
* @param device_list: the list of bluetooth MAC addresses
* @param num_of_devices: the cardinality of device array - represents the number of bluetooth devices stored in the array
*/
void free_device_list(char **device_list, int num_of_devices) {
    if (device_list) {
        for (int i = 0; i < num_of_devices; i++) {
            free(device_list[i]);
        }
        free(device_list);
    }
}

/*
* Write to the array all the trusted bluetooth MAC addresses.
*
* @param trusted_dev_fp: the file handle of the user's trusted device file
* @param trusted_devices: an array that will contain all the user's trusted bluetooth MAC addresses to authenticate from
*/
void set_trusted_devices(FILE *trusted_dev_fp, char **trusted_devices, int num_trusted_devices) {
    int i = 0;

    char *line = NULL;
    char **dev = NULL;
    size_t len = 0;
    ssize_t read;

    if (trusted_dev_fp && trusted_devices && num_trusted_devices > 0) {
        fseek(trusted_dev_fp, 0, SEEK_SET);
        while ((read = getline(&line, &len, trusted_dev_fp)) != -1 && i < num_trusted_devices) {
            if (strcmp(line, "\n") != 0) {
                dev = trusted_devices + i;
                if ((*dev = malloc(sizeof(char) * (BT_MAC_LEN + 1)))) {
                    strncpy(*dev, line, BT_MAC_LEN);
                    (*dev)[BT_MAC_LEN] = '\0';
                    i++;
                }
            }
        }
    }

    if (line) {
        free(line);
    }
}

/*
* Populate the array with trusted devices
*
* @param log_fp: the file handle for the log file
* @param trusted_dir_path: the path where the trusted bluetooth device files for all users are located
* @param username: the username of the user that wants to log in
* @param num_of_devices: the number of devices the users trust to authenticate their system
*   Note: the number is set in this function
* @return the list of trusted devices
*/
char **find_trusted_devices(FILE *log_fp, const char *trusted_dir_path, const char *username, int *num_of_devices) {
    char **trusted_devices = NULL;
    FILE *trusted_dev_fp = NULL;
    int num_of_devices_lc = 0; //localalizing for possible memory performance improvement

    /*** Check if the trusted device directory exist ***/
    if (check_or_creat_dir(trusted_dir_path, log_fp) <= 0) {
        //NOTE: ret status of 0 means directory just got created, so there are no trusted device
        goto find_trusted_devices_terminate;
    }
    /***************************************************/

    if (!(trusted_dev_fp = get_trusted_dev_file(trusted_dir_path, username, log_fp))) {
        goto find_trusted_devices_terminate;
    }

    if (!(num_of_devices_lc = get_num_lines(trusted_dev_fp))) {
        goto find_trusted_devices_terminate;
    }
   
    if (!(trusted_devices = malloc(sizeof(char *) * num_of_devices_lc))) {
        perror("malloc");
        goto find_trusted_devices_terminate;
    }

    set_trusted_devices(trusted_dev_fp, trusted_devices, num_of_devices_lc);

find_trusted_devices_terminate:
    if (trusted_dev_fp) {
        fclose(trusted_dev_fp);
    }

    *num_of_devices = num_of_devices_lc;
    return trusted_devices;
}

/*
* Deals whether or not the device is able to login via bluetooth
*
* @param log_fp: the file handle for the log file
* @param trusted_dir_path: the path of where all the user's trusted devices are located
* @param username: the username of the user who wants to login
* @return: return 1 if the trusted bluetooth device is detected
*/
int bluetooth_login(FILE *log_fp, const char *trusted_dir_path, const char *username) {
    int bluetooth_status = 0;
    char curr_time[50];

    int num_of_devices = 0;
    int num_of_paired = 0;

    char **trusted_devices = NULL;
    char **paired_devices = get_paired_devices(log_fp, &num_of_paired);

    char *detected_dev = NULL;

    if (!(trusted_devices = find_trusted_devices(log_fp, trusted_dir_path, username, &num_of_devices))) {
        goto bluetooth_login_terminate;
    }

    /*** LOGIN TIME ***/
    get_login_time(curr_time);
    /*******************/


    if (log_fp) {
        fprintf(log_fp, "%s: %d devices are paired to host\n", curr_time, num_of_paired);
    }

    if (log_fp) {
        fprintf(log_fp, "%s: Call find device\n", curr_time);
    }

    if (paired_devices && (bluetooth_status = find_trusted_paired_device(log_fp, trusted_devices, num_of_devices, paired_devices, num_of_paired, &detected_dev))) {
        if (log_fp && detected_dev) {
            fprintf(log_fp, "%s: Device %s paired\n", curr_time, detected_dev);
        }
    }
    else if ((bluetooth_status = find_device(log_fp, trusted_devices, num_of_devices, &detected_dev))) {
        if (log_fp && detected_dev) {
            fprintf(log_fp, "%s: Device %s found\n", curr_time, detected_dev);
        }
    }
    else {
        if (log_fp) {
            fprintf(log_fp, "%s: No trusted devices was found %d\n", curr_time, bluetooth_status);
        }
    }

bluetooth_login_terminate:
    if (trusted_devices) {
        free_device_list(trusted_devices, num_of_devices);
    }

    if (paired_devices) {
        free_device_list(paired_devices, num_of_paired);
    }

    if (detected_dev) {
        free(detected_dev);
    }
    
    return bluetooth_status;
}

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int bluetooth_status = PAM_AUTH_ERR;

	const char* username;

    FILE *log_fp = NULL;

    /*** OPEN LOG FILE ***/
    if (!(log_fp = fopen("/var/log/pam-proxy-auth.log", "a"))) {
        perror("Failed to open file");
    }
    /*********************/
    
    /*** Get Username ***/
    if (pam_get_user(pamh, &username, "Username: ") != PAM_SUCCESS) {
        perror("Could not find username");
        if (log_fp) {
            fprintf(log_fp, "Could not find username\n");
        }
        goto pam_sm_authenticate;
    }
    /*******************/

    if (bluetooth_login(log_fp, trusted_dir_path, username)) {
	   if (log_fp) {
            fprintf(log_fp, "Login via Auth Proxy\n");
        }
       bluetooth_status = PAM_SUCCESS;
    }

pam_sm_authenticate:

    if (log_fp) {
        fclose(log_fp);
    }

	return bluetooth_status;
}
