#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

//#define DEVICE_ADDR "C0:1A:DA:7A:30:B7"
#define PASSWORD_LOGIN 0
#define DEVICE_ADDR "F0:81:73:92:2E:C2"


/*
*
*/
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

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int retval = 0;

	const char* pUsername;
    int device_found = 0;

    time_t raw_time;
    struct tm *info_time;
    time(&raw_time);
    info_time = localtime(&raw_time);
    char curr_time[50];
    int time_len = strlen(asctime(info_time));
    strncpy(curr_time, asctime(info_time), time_len);
    curr_time[(time_len - 1)] = '\0'; //asctime appends a \n to the string


    FILE *fp = NULL;
    if (!(fp = fopen("/var/log/simple-pam.log", "a"))) {
        perror("Failed to open file\n");
    }

    printf("Try to find device: %s\n", DEVICE_ADDR);
    if ((device_found = find_device(DEVICE_ADDR))) {
        printf("Device found\n");
        if (fp) {
            fprintf(fp, "%s: Device %s found\n", curr_time, DEVICE_ADDR);
        }
    }
    else {
        if (fp) {
            fprintf(fp, "%s: Device %s NOT found\n", curr_time, DEVICE_ADDR);
        }
    } 

    #ifdef PASSWORD_LOGIN
	if ((retval = pam_get_user(pamh, &pUsername, "Username: ") == PAM_SUCCESS)) {
        if (fp) {
            fprintf(fp, "%s: %s logged on\n", curr_time, pUsername);
        }
    }
    #endif
    
	printf("Welcome %s\n", pUsername);
    printf("This is a simple PAM says Pikachu\n");
   
    if (fp) {
        fclose(fp);
    }

	if (retval != PAM_SUCCESS && !device_found) {
		return retval;
	}

	return PAM_SUCCESS;
}
