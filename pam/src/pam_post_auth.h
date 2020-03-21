#ifndef PAM_POST_AUTH
#define PAM_POST_AUTH

#define DEAUTH "deauth"

/*
* Once Authenticated, run the deauth background service
*
* NOTE: exec replaces the child process. Memory mappings are not preservered on an exec() call so memory is reclaimed apparently.
* NOTE: will set the euid to be the user instead of root
* 
* @param bt_addr: the trusted bluetooth address we want to log in
* @param username: the user we want to run the program as (i.e. should be the username of the user who wishes to login)
*/
int exec_deauth(char *bt_addr, const char *username, FILE *log_fp) {
    int pid = fork();

    if (pid == 0) { //child process
        char *path = NULL;

        char curr_time[50];

        if (log_fp) {
            get_login_time(curr_time);
        }

        int len = strlen(trusted_dir_path) + strlen(DEAUTH);

        if (!(path = malloc(sizeof(char) + len + 1))) {
            perror("malloc");
            fprintf(log_fp, "%s: (exec_deauth) malloc Error\n", curr_time);
            return PAM_BUF_ERR;
        }

        if (!(strncpy(path, trusted_dir_path, strlen(trusted_dir_path)))) {
            perror("strncpy");
            fprintf(log_fp, "%s: (exec_deauth) strncpy Error\n", curr_time);
            return PAM_BUF_ERR;
        }

        path[strlen(trusted_dir_path)] = '\0';

        if (!(strncat(path, DEAUTH, strlen(DEAUTH) + 1))) {
            perror("strncat");
            fprintf(log_fp, "%s: (exec_deauth) strncat Error\n", curr_time);
            return PAM_BUF_ERR;
        }
        path[len] = '\0';

        struct passwd *pass = NULL;
        if (!(pass = getpwnam(username))) {
            fprintf(log_fp, "%s: (exec_deauth) getpwnam Error. Trued to get uid of user %s\n", curr_time, username);
            perror("getpwnam");
            return PAM_SYSTEM_ERR;
        }
        
        setuid(pass->pw_uid);

        if ((execl(path, path, bt_addr, NULL)) == -1) {
            perror("exec");
            fprintf(log_fp, "%s: (exec_deauth) execl Error\n", curr_time);
            return PAM_SYSTEM_ERR;
        }
    }
    return PAM_SUCCESS;	
}
#endif