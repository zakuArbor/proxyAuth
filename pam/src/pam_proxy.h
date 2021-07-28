/*
* @author: zakuarbor (Ju Hong Kim)
* @brief: PAM module for the ProxyAuth project. Authenticates users via bluetooth proximity
*/

#ifndef PAM_PROXY_H
#define PAM_PROXY_H

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

/**
 * Not Used
 * */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv );

/**
 * Not Used
 */
PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv); 

/**
 * Handles User Authentication whether or not to approve the connected device 
 * as the hardware authenticator
 *
 * @param pamh the PAM handle representing the current user authentication session
 * @param flags 
 * @param argc
 * @param argv
 * @return PAM_SUCCESS iff the connected device is trusted
 */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv );

#endif
