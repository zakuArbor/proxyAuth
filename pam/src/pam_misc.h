#ifndef PAM_MISC
#define PAM_MISC

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


/**
* Set the login time to the given pointer           <br>
* Format: %a %b %d %H:%M:%S %Y                      <br>
* Example: Fri Feb 11 02:59:18 2020                 <br>
*                                                   <br>
* %a: locale's abbreviated weekday name (e.g., Sun) <br>
* %b: locale's abbreviated month name (e.g., Jan)   <br>
* %d: day of month (e.g., 01)                       <br>
* %H: hour (00..23)                                 <br>
* %M: minute (00..59)                               <br>
* %S: second (00..60)                               <br>
* %Y: year                                          <br>
* Credits: `man date` for the format specifier
*
* @param curr_time a pointer to store the current time
* @return nothing
*/
void get_login_time(char *curr_time);

/**
* Return the number of lines there are in the file
*
* Use Case: Each user will have a list of trusted bluetooth MAC stored in a file. E
*   Each line will contain a trusted bluetooth MAC address.
*   The number of lines in the file will represent the number of trusted devices
*
* @param FILE *fp: the file handle that contains the user's device
* @return: the number of lines there are in the file
*/
int get_num_lines(FILE *fp);

/**
* Check if the directory exists. If not try to create the directory
*
* @param dir_path: The path to the directory
* @param log_fp: the handle of the log file
* @return
*   -1: cannot access dir or file is not a directory or directory could not be created <br>
*   0: created the directory <br>
*   1: file exists and it is a directory <br>
*/
int check_or_creat_dir(const char *dir_path, FILE *log_fp);
#endif
