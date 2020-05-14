#ifndef PAM_BT_MISC_H
#define PAM_BT_MISC_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BT_MAC_LEN 17
#define BT_MAX_CONN 7 //Bluetooth Adapters can only connect up to 7 devices
#define LOG 1

const char *trusted_dir_path = "/etc/proxy_auth/";

/*
* Return 1 iff the address is a valid Bluetooth Address
*
* NOTE: Bluetooth Address is a 48 bit address. The function will treat address as a string expecting:
*       * 17 bit string
*       * Each bit can only be a hexadecimal digit other than ":" in pos 2 5 8 11 14
*       * i.e. 00:00:00:00:00:00
*
* @param address: the bluetooth address
* @param log_fp: the file handle for the log file
* @return: 1 iff the address is a valid Bluetooth Address. Else return 0 (False)
*/
int verify_bt_addr(char *address, FILE *log_fp);

/*
* Return 1 iff the given address is one of the trusted devices the user trusts
*
* @param log_fp: the handle of the log file
* @param trusted_devices: the array of trusted bluetooth MAC addresses of the user
* @param num_of_devices: the number of devices the user trusts
* @return: 1 iff the given address is one of the trusted devices the user trusts. Else return 0
*/
int is_dev_trusted(FILE *log_fp, char *dev, char **trusted_devices, int num_of_devices);

/*
* Free the list of bluetooth MAC addresses from memory
*
* @param device_list: the list of bluetooth MAC addresses
* @param num_of_devices: the cardinality of device array - represents the number of bluetooth devices stored in the array
*/
void free_device_list(char **device_list, int num_of_devices);
#endif