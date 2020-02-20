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
#include <security/pam_appl.h>
#include <security/pam_modules.h>

//#define DEVICE_ADDR "C0:1A:DA:7A:30:B7"
#define LOG 1
#define DEVICE_ADDR "F0:81:73:92:2E:C2"

const char *trusted_log_dir = "/etc/proxy_auth/";

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

int find_device(char *dev_addr) {
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };

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
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        if (strcmp(addr, dev_addr) == 0) {
            printf("Found Device: %s\n", addr);
            return(1);
        }
        printf("%s\n", addr);
    }

    free( ii );
    close( sock );

    return(0);
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
        num_lines++;
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
* Populate the array with trusted devices
*
* @param log_fp: the file handle for the log file
* @return the list of trusted devices
*/
char **find_trusted_devices(FILE *log_fp, const char *trusted_dir_path, const char *username) {
    char **trusted_devices = NULL;
    FILE *trusted_dev_fp = NULL;

    /*** Check if the trusted device directory exist ***/
    if (check_or_creat_dir(trusted_dir_path, log_fp) <= 0) {
        //NOTE: ret status of 0 means directory just got created, so there are no trusted device
        goto terminate;
    }
    /***************************************************/

    if (!(trusted_dev_fp = get_trusted_dev_file(trusted_dir_path, username, log_fp))) {
        goto terminate;
    }

    int get_num_of_devices = get_num_lines(trusted_dev_fp);
    printf("the number of trusted_devices are: %d\n", get_num_of_devices);
    //find_trusted_devces = malloc();
terminate:
    if (trusted_dev_fp) {
        fclose(trusted_dev_fp);
    }
    if (trusted_devices) {
        free(trusted_devices);
    }

    return trusted_devices;
}

/*
* Deals whether or not the device is able to login via bluetooth
*/
int bluetooth_login(FILE *log_file) {
    int bluetooth_login = 0;
    char curr_time[50];
    
    printf("Try to find device: %s\n", DEVICE_ADDR);

    bluetooth_login = find_device(DEVICE_ADDR);

    /*** LOGIN TIME ***/
    get_login_time(curr_time);
    /*******************/

    if (log_file){
        if ((bluetooth_login = find_device(DEVICE_ADDR))) {
            printf("Device found\n");
            if (log_file) {
                fprintf(log_file, "%s: Device %s found\n", curr_time, DEVICE_ADDR);
            }
        }
        else {
            if (log_file) {
                fprintf(log_file, "%s: Device %s NOT found\n", curr_time, DEVICE_ADDR);
            }
        }
    }

    return bluetooth_login;
}

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
    return PAM_SUCCESS;

	int pam_status = 0;
    int bluetooth_status = 0;

    
	const char* username;

    FILE *fp = NULL;

    /*** OPEN LOG FILE ***/
    if (!(fp = fopen("/var/log/pam-proxy-auth.log", "a"))) {
        perror("Failed to open file");
    }
    /*********************/

    fprintf(fp, "Pika test\n");
    
    /*** Get Username ***/
    if (pam_get_user(pamh, &username, "Username: ") != PAM_SUCCESS) {
        perror("Could not find username");
        if (fp) {
            fprintf(fp, "Could not find username\n");
        }
        return 0;
    }
    /*******************/

	printf("Welcome %s. Login via Auth Proxy.\n", username);

    if (fp) {
        fclose(fp);
    }

	if (pam_status != PAM_SUCCESS && !bluetooth_status) {
		return pam_status;
	}

	return PAM_SUCCESS;
}
