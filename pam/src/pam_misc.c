#include "pam_misc.h"

void get_login_time(char *curr_time) { 
    time_t raw_time;
    struct tm *info_time;
    time(&raw_time);
    info_time = localtime(&raw_time);
    int time_len = strlen(asctime(info_time));
    strncpy(curr_time, asctime(info_time), time_len);
    curr_time[(time_len - 1)] = '\0'; //asctime appends a \n to the string
}

int get_num_lines(FILE *fp) {
    int num_lines = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (!fp) {
        return num_lines;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strcmp(line, "\n") != 0) {
            num_lines++;
        }
    }

    if (line) {
        free(line);
    }

    return num_lines;
}

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
