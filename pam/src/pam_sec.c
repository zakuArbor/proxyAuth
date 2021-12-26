#include "pam_sec.h"

int check_config(FILE *log_fp, const char * const file, const int is_dir) {
    struct stat st;

    printf("checking file: %s\n", file);

    if (!is_nlnk(log_fp, file, &st)) {
        return 0;
    }

    if (is_dir) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Error: %s is not a directory\n", file);

            if (log_fp) {
                fprintf(log_fp, "Error: %s is not a directory\n", file);
            }
            return 0;
        }
    }
    else {
        if (!S_ISREG(st.st_mode)) {
            fprintf(stderr, "Error: %s is not a regular file\n", file);

            if (log_fp) {
                fprintf(log_fp, "Error: %s is not a regular file\n", file);
            }
            return 0;
        }        
    }


    if (!check_perm(log_fp, file, &st, is_dir)) {
        return 0;
    }

    return 1;
}

int is_nlnk(FILE *log_fp, const char * const file, struct stat * const st) {
    int exist = lstat(file, st);
    
    if (exist == -1 && errno == ENOENT) {
        fprintf(stderr, "%s does not exist\n", file);

        if (log_fp) {
            fprintf(log_fp, "Error: %s does not exist\n", file);
        }

        return 0;
    }
    if (S_ISLNK(st->st_mode)) {
        fprintf(stderr, "Error: %s is a link\n", file);

        if (log_fp) {
            fprintf(log_fp, "Error: %s is a link\n", file);
        }
        return 0;
    }
    return 1;
}

int check_perm(FILE *log_fp, const char * const file, const struct stat * const st, const int is_dir) {
    //check if permission is set correctly (i.e only owner should have rw permission while others and group should only have at most execute and read permission)
    /* Permission Check Logic
    * Shift the bits to the right by 6 bits to compare with the owner group (i.e. 3 bits from others and 3 bits from the group)
    * And it with 0x7 to mast out all other bits except for the most right 3 bits
    Credits: https://icarus.cs.weber.edu/~dab/cs1410/textbook/2.Core/file_access.html
    */

    int perm = 6;

    if (is_dir) {
        perm = 7; //execute permission is needed for the owner to access the directory
    }

    if ( ((st->st_mode >> 6) & 0x7) != perm) { //owner
        fprintf(stderr, "Error: Owner type does not have %s permission set to %d\n", file, perm);
        if (log_fp) {
            fprintf(log_fp, "Error: Owner type does not have %s permission set to %d\n", file, perm);
        }
        return 0;
    }
    
    if (st->st_mode & S_IWGRP) {
        fprintf(stderr, "Error: Group has write permission to %s\n", file);
        if (log_fp) {
            fprintf(log_fp, "Error: Group has write permission to %s\n", file);
        }
        return 0;
    } 
    if (st->st_mode & S_IWOTH) {
        fprintf(stderr, "Error: Other has write permission to %s\n", file);
        if (log_fp) {
            fprintf(log_fp, "Error: Other has write permission to %s\n", file);
        }
        return 0;
    } 

    //check if the owner is root
    if (st->st_uid != 0) {
        fprintf(stderr, "Error: %s is not owned by root\n", file);
        if (log_fp) {
            fprintf(log_fp, "Error: %s is not owned by root\n", file);
        }
        return 0;
    }

    return 1;  
}