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
* @param FILE *fp: the file handle that contains the user's device
*/
int get_num_lines() {
    return 0;
}

/*
* Check if the directory exists. If not try to create the directory
*
* @param dir_path: The path to the directory
* @return:
*   -1: cannot access dir or file is not a directory or directory could not be created
*   0: created the directory
*   1: file exists and it is a directory
*/
int check_or_creat_dir(char *dir_path) {
    struct stat s;
    int status = -1;

    int exist = stat(dir_path, &s);
    if (exist == -1) {
        if(exist == ENOENT) {
            /* does not exist */
            if (mkdir(dir_path, 0755) == 0) {
                status = 0;
            }
        } 
        else {
            fprintf(stderr, "Cannot access %s\n", dir_path);
        }
    } 
    else if (!S_ISDIR(s.st_mode)) {
        fprintf(stderr, "Error: %s is not a directory\n", dir_path);
    }
    return status;
}

/*
* Populate the array with trusted devices
*
* @return 
* @return the list of trusted devices
*/
char **find_trusted_devces(FILE *log_fp) {
    char **trusted_devices = NULL;
    FILE *trusted_dev_fp = NULL;

    char *trusted_log_dir = "/etc/proxy_auth/";

    

    if (check_or_creat_dir(trusted_log_dir) <= 0) {
        //NOTE: ret status of 0 means directory just got created, so there are no trusted device
        goto terminate;
    }



    if (!(trusted_dev_fp = fopen("/etc/proxy_auth_devices", "r"))) {
        perror("There are no trusted device");
        return NULL;
    }

    //find_trusted_devces = malloc();
terminate:
    if (trusted_dev_fp) {
        free(trusted_dev_fp);
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

    #ifdef LOG
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
    #endif 

    return bluetooth_login;
}

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int retval = 0;

	const char* username;
    int device_found = 0;

    /*** LOGIN TIME ***/
    char curr_time[50];
    get_login_time(curr_time);
    /*******************/

    FILE *fp = NULL;
    #ifdef LOG
    if (!(fp = fopen("/var/log/simple-pam.log", "a"))) {
        perror("Failed to open file\n");
    }
    #endif
    
    if (pam_get_user(pamh, &username, "Username: ") != PAM_SUCCESS) {
        perror("Could not find username\n");
        if (fp) {
            fprintf(fp, "Could not find username\n");
        }
        return 0;
    }

    

	printf("Welcome %s. Login via Auth Proxy.\n", username);
    fprintf(fp, "Time is: %s\n", curr_time);
    return PAM_SUCCESS;
    if (fp) {
        fclose(fp);
    }

	if (retval != PAM_SUCCESS && !device_found) {
		return retval;
	}

	return PAM_SUCCESS;
}
