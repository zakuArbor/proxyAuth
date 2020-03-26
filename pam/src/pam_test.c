/*
* AUTHOR: zakuarbor (Ju Hong Kim)
* DESC: For testing new features of the PAM module before deployment testing (i.e. Test if feature works as intended without any obvious bugs). 
*   This does not replace testing the PAM module. It is suppose to be used for quick, dirty and simple runtime testing with memory leak checker.
*/

#include <errno.h>
#include <limits.h>
#include <pwd.h>
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

#include "pam_misc.h"
#include "pam_bt_misc.h"
#include "pam_bt_pair.h"
#include "pam_bt_trust.h"
#include "pam_post_auth.h"

int main(int argc, char **argv)
{
    const char *trusted_dir_path = "/etc/proxy_auth/";
    const char *username = "zaku";
    char *detected_dev;

    FILE *log_fp = NULL;
    
    /*** OPEN LOG FILE ***/
    if (!(log_fp = fopen("/var/log/pam-proxy-auth.log", "a"))) {
        perror("Failed to open file");
    }
    /*********************/
    printf("start bluetooth_login\n");
    if (bluetooth_login(log_fp, trusted_dir_path, username, &detected_dev)) {
       printf("Welcome %s. Login via Auth Proxy.\n", username);
       exec_deauth(detected_dev, username, log_fp);
    }
    else {
        printf("Failed");
    }

    if (log_fp) {
        fclose(log_fp);
    }
}
