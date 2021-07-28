#ifndef PAM_POST_AUTH
#define PAM_POST_AUTH

#define DEAUTH "deauth"

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include "pam_misc.h"
#include "pam_bt_misc.h"

/**
* Once Authenticated, run the deauth background service
*
* NOTE: exec replaces the child process. Memory mappings are not preservered on an exec() call so memory is reclaimed apparently.
* NOTE: will set the euid to be the user instead of root
* 
* @param bt_addr: the trusted bluetooth address we want to log in
* @param username: the user we want to run the program as (i.e. should be the username of the user who wishes to login)
*/
int exec_deauth(char *bt_addr, const char *username, FILE *log_fp, const char *trusted_dir_path);
#endif
