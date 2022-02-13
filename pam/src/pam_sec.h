/**
* @author: zakuarbor (Ju Hong Kim)
* @brief: Where security checks and security tools are placed
*/

#ifndef PAM_SEC_H
#define PAM_SEC_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
* Return 1 iff the file exists (and not a link) and has the correct permission
* 
* @param log_fp: the handle of the log file
* @param file: the config file (a directory or regular file)
* @param is_dir: 1 iff the file is a directory
* @return: return 1 iff the file exists, is of the type specified by is_dir and has the correct permission. Else returns a 0.
*/
int check_config(FILE *log_fp, const char * const file, const int is_dir);

/**
* Return 1 iff the file exists and is not a symlink. Else returns a 0.
*
* @param log_fp: the handle of the log file
* @param file: the file to inspect
* @param st: the pointer to a file stat structure. This field is modified in the function
* @return: return 1 iff the file exists and is not a symlink. Else returns a 0.
*/
int is_nlnk(FILE *log_fp, const char * const file, struct stat * const st);

/**
* Return 1 iff the file is owned by root and has rw permission and the other fields does not have write permission
*
* @param log_fp: the handle of the log file
* @param file: the file to inspect
* @param st: the pointer to a file stat structure. This field is not modified in this function
* @param is_dir: 1 iff the desired file is supposed to be a directory
* @return: return 1 iff the file is owned by root and has rw permission and the other fields does not have write permission
*/
int check_perm(FILE *log_fp, const char * const file, const struct stat * const st, const int is_dir);

#endif
