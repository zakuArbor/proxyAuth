#include "pam_bt_trust.h"

int find_trusted_paired_device(FILE *log_fp, char **trusted_devices, int num_of_devices, char **paired_devices, int num_of_paired, char **detected_dev) {
    if (!trusted_devices || !paired_devices) {
        return 0;
    }
    int trusted_dev_found = 0;

    for (int i = 0; i < num_of_paired; i++) {
        if (is_dev_trusted(log_fp, paired_devices[i], trusted_devices, num_of_devices)) {
            if (log_fp) {
                fprintf(log_fp, "Trusted Device: %s\n", paired_devices[i]);
            }
            if ((*detected_dev = malloc(sizeof(char) * (BT_MAC_LEN + 1)))) {
                strncpy(*detected_dev, paired_devices[i], BT_MAC_LEN);
                (*detected_dev)[BT_MAC_LEN] = '\0';
            }
            trusted_dev_found = 1;
            break;
        }
    }

    return trusted_dev_found;
}

FILE *get_trusted_dev_file(const char *trusted_dir_path, const char *username, FILE *log_fp) {
    FILE *trusted_dev_fp = NULL;

    char file_name[NAME_MAX];
    int len = 0;
    strcpy(file_name, "");
    
    if (strlen(trusted_dir_path) > 0) {
        strncat(file_name, trusted_dir_path, strlen(trusted_dir_path));
        len += strlen(trusted_dir_path);
    }
    if (strlen(username) > 0) {
        strncat(file_name, username, strlen(username));
        len += strlen(username);
    }
    //I am paranoid
    if (len > NAME_MAX) {
        file_name[NAME_MAX] = '\0';
    }
    else {
        file_name[len] = '\0';
    }

    if (!(trusted_dev_fp = fopen(file_name, "r"))) {
        perror("There are no trusted device");
        if (log_fp) {
            fprintf(log_fp, "File: %s does not exist. There are no trusted device for the user: %s\n", file_name, username);

        }
        fprintf(stderr, "File: %s does not exist. There are no trusted device for the user: %s\n", file_name, username);
        return NULL;
    }
    return trusted_dev_fp;
}

void set_trusted_devices(FILE *trusted_dev_fp, char **trusted_devices, int num_trusted_devices) {
    int i = 0;

    char *line = NULL;
    char **dev = NULL;
    size_t len = 0;
    ssize_t read;

    if (trusted_dev_fp && trusted_devices && num_trusted_devices > 0) {
        fseek(trusted_dev_fp, 0, SEEK_SET);
        while ((read = getline(&line, &len, trusted_dev_fp)) != -1 && i < num_trusted_devices) {
            if (strcmp(line, "\n") != 0) {
                dev = trusted_devices + i;
                line[BT_MAC_LEN] = '\0';
                if ((*dev = malloc(sizeof(char) * (BT_MAC_LEN + 1)))) {
                    strncpy(*dev, line, BT_MAC_LEN);
                    (*dev)[BT_MAC_LEN] = '\0';
                    i++;
                }
            }
        }
    }

    if (line) {
        free(line);
    }
}

char **find_trusted_devices(FILE *log_fp, const char *trusted_dir_path, const char *username, int *num_of_devices) {
    char **trusted_devices = NULL;
    FILE *trusted_dev_fp = NULL;
    int num_of_devices_lc = 0; //localalizing for possible memory performance improvement

    /*** Check if the trusted device directory exist ***/
    if (check_or_creat_dir(trusted_dir_path, log_fp) <= 0) {
        //NOTE: ret status of 0 means directory just got created, so there are no trusted device
        goto find_trusted_devices_terminate;
    }
    /***************************************************/

    if (!(trusted_dev_fp = get_trusted_dev_file(trusted_dir_path, username, log_fp))) {
        goto find_trusted_devices_terminate;
    }

    if (!(num_of_devices_lc = get_num_lines(trusted_dev_fp))) {
        goto find_trusted_devices_terminate;
    }
   
    if (!(trusted_devices = malloc(sizeof(char *) * num_of_devices_lc))) {
        perror("malloc");
        goto find_trusted_devices_terminate;
    }

    set_trusted_devices(trusted_dev_fp, trusted_devices, num_of_devices_lc);

find_trusted_devices_terminate:
    if (trusted_dev_fp) {
        fclose(trusted_dev_fp);
    }

    *num_of_devices = num_of_devices_lc;
    return trusted_devices;
}

int bluetooth_login(FILE *log_fp, const char *trusted_dir_path, const char *username, char **detected_dev) {
    int bluetooth_status = 0;
    char curr_time[50];

    int num_of_devices = 0;
    int num_of_paired = 0;

    char **trusted_devices = NULL;
    char **paired_devices = get_paired_devices(&num_of_paired);
    *detected_dev = NULL;

    if (!(trusted_devices = find_trusted_devices(log_fp, trusted_dir_path, username, &num_of_devices))) {
        goto bluetooth_login_terminate;
    }

    /*** LOGIN TIME ***/
    get_login_time(curr_time);
    /*******************/

    if (log_fp) {
        fprintf(log_fp, "%s: Call find device\n", curr_time);
    }
    
    if (paired_devices && (bluetooth_status = find_trusted_paired_device(log_fp, trusted_devices, num_of_devices, paired_devices, num_of_paired, detected_dev))) {
        if (log_fp && *detected_dev) {
            fprintf(log_fp, "%s: Device %s found\n", curr_time, *detected_dev);
        }
    }
    else {
        if (log_fp) {
            fprintf(log_fp, "%s: No trusted devices was found %d\n", curr_time, bluetooth_status);
        }
    }

bluetooth_login_terminate:
    if (trusted_devices) {
        free_device_list(trusted_devices, num_of_devices);
    }

    if (paired_devices) {
        free_device_list(paired_devices, num_of_paired);
    }
    
    return bluetooth_status;
}
