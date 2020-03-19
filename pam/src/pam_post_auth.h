#ifndef PAM_POST_AUTH
#define PAM_POST_AUTH

#define DEAUTH "deauth"

/*
* Once Authenticated, run the deauth background service
*
* NOTE: exec replaces the child process. Memory mappings are not preservered on an exec() call so memory is reclaimed apparently.
*/
int exec_deauth() {
    int pid = fork();

    if (pid == 0) { //child process
        char *path = NULL;
    
        char **argv = {NULL};

        int len = strlen(trusted_dir_path) + strlen(DEAUTH);

        if (!(path = malloc(sizeof(char) + len + 1))) {
            perror("malloc");
            return PAM_BUF_ERR;
        }

        if (!(strncpy(path, trusted_dir_path, strlen(trusted_dir_path)))) {
            perror("strncpy");
            return PAM_BUF_ERR;
        }

        path[strlen(trusted_dir_path)] = '\0';

        if (!(strncat(path, DEAUTH, strlen(DEAUTH) + 1))) {
            perror("strncat");
            return PAM_BUF_ERR;
        }
        
        path[len] = '\0';

        execv(path, argv);

    }
    return PAM_SUCCESS;	
}
#endif