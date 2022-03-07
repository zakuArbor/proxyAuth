#include "pam_proxy.h"

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}


PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int bluetooth_status = PAM_AUTH_ERR;

	const char* username;
    char* detected_dev;

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

    if (bluetooth_login(log_fp, trusted_dir_path, username, &detected_dev)) {
	   if (log_fp) {
            fprintf(log_fp, "Login via Auth Proxy\n");
        }
        exec_deauth(detected_dev, username, log_fp, trusted_dir_path);
        free(detected_dev);
        bluetooth_status = PAM_SUCCESS;
    }

pam_sm_authenticate:

    if (log_fp) {
        fclose(log_fp);
    }

	return bluetooth_status;
}
