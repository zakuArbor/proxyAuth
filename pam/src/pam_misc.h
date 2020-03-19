#ifndef PAM_MISC
#define PAM_MISC
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

/*
* Return the number of lines there are in the file
*
* Use Case: Each user will have a list of trusted bluetooth MAC stored in a file. E
*   Each line will contain a trusted bluetooth MAC address.
*   The number of lines in the file will represent the number of trusted devices
*
* @param FILE *fp: the file handle that contains the user's device
* @return: the number of lines there are in the file
*/
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

/*
* Check if the directory exists. If not try to create the directory
*
* @param dir_path: The path to the directory
* @param log_fp: the handle of the log file
* @return:
*   -1: cannot access dir or file is not a directory or directory could not be created
*   0: created the directory
*   1: file exists and it is a directory
*/
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
#endif