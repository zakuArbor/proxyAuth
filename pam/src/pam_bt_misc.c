#include "pam_bt_misc.h"

const char *trusted_dir_path = "/etc/proxy_auth/";

int verify_bt_addr(char *address, FILE *log_fp) {
    char *curr = address;
    int len = 0;

    int is_valid = 1;

    if (!address) {
        return 0;
    }

    if (strlen(address) <= BT_MAC_LEN) {   
        while (curr && len < strlen(address)) {
            int is_div3 = (len + 1) % 3 == 0;
            
            if (    len > BT_MAC_LEN ||                                 //exceeds the BT String Len
                    (isxdigit(*curr) == 0 && !is_div3) ||               //not Alphanumeric when it should be
                    (isxdigit(*curr) == 0 && is_div3 && *curr != ':')   //not a colon when it should be
                ) {
                is_valid = 0;
                break;
            }

            len++;
            curr++;
        }
    }
    else {
        is_valid = 0;
    }
       
    if (is_valid && len == BT_MAC_LEN) {
        return 1;
    }
    
    if (log_fp) {
        fprintf(log_fp, "%s is an invalid Bluetooth Address\n", address);
    }

    return 0;
}

int is_dev_trusted(FILE *log_fp, char *dev, char **trusted_devices, int num_of_devices) {
    for (int i = 0; i < num_of_devices; i++) {
        if (strcmp(dev, trusted_devices[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void free_device_list(char **device_list, int num_of_devices) {
    if (device_list) {
        for (int i = 0; i < num_of_devices; i++) {
            free(device_list[i]);
            device_list[i] = NULL;
        }
        free(device_list); 
        device_list = NULL;
    }
}
