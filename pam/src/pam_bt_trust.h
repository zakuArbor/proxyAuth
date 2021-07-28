#ifndef PAM_BT_TRUST
#define PAM_BT_TRUST

#include <stdio.h>
#include <string.h>
#include "pam_misc.h"
#include "pam_bt_misc.h"
#include "pam_bt_pair.h"

extern const char *trusted_dir_path;

/**
* Return 1 iff a trusted device for the user is connected
*
* @param log_fp: the handle of the log file
* @param trusted_devices: the array of trusted bluetooth MAC addresses of the user
* @param num_of_devices: the number of devices the user trusts
* @param detected_dev: the address of the detected device
*   NOTE: detected_dev will be set in this function if a device was detected
* @return: 1 iff a trusted device for the user is detected
*/
int find_trusted_paired_device(FILE *log_fp, char **trusted_devices, int num_of_devices, char **paired_devices, int num_of_paired, char **detected_dev);

/**
* Return file pointer for the user's trusted bluetooth device
*
* @param trusted_dir_path: the path to where all the user's trusted device file is located
* @param username: the username
* @param log_fp: the handle for the log file
* @return: the file handle of the user's trusted device file
*/
FILE *get_trusted_dev_file(const char *trusted_dir_path, const char *username, FILE *log_fp);

/**
* Write to the array all the trusted bluetooth MAC addresses.
*
* @param trusted_dev_fp: the file handle of the user's trusted device file
* @param trusted_devices: an array that will contain all the user's trusted bluetooth MAC addresses to authenticate from
*/
void set_trusted_devices(FILE *trusted_dev_fp, char **trusted_devices, int num_trusted_devices);

/**
* Populate the array with trusted devices
*
* @param log_fp: the file handle for the log file
* @param trusted_dir_path: the path where the trusted bluetooth device files for all users are located
* @param username: the username of the user that wants to log in
* @param num_of_devices: the number of devices the users trust to authenticate their system
*   Note: the number is set in this function
* @return the list of trusted devices
*/
char **find_trusted_devices(FILE *log_fp, const char *trusted_dir_path, const char *username, int *num_of_devices);

/**
* Deals whether or not the device is able to login via bluetooth
*
* @param log_fp: the file handle for the log file
* @param trusted_dir_path: the path of where all the user's trusted devices are located
* @param username: the username of the user who wants to login
* @param detected_dev: The bluetooth address of the device that authenticates PAM
*   NOTE: find_trusted_paired_device(...) will allocate and set the bluetooth address. Developer must free the memory themselves
* @return: return 1 if the trusted bluetooth device is detected
*/
int bluetooth_login(FILE *log_fp, const char *trusted_dir_path, const char *username, char **detected_dev);

#endif
