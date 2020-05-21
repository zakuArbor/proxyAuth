#ifndef PAM_BT_PAIR
#define PAM_BT_PAIR

#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include "pam_bt_misc.h"

#define BLUEZ_DBUS_NAME                 "org.bluez"
#define BLUEZ_DBUS_OBJ_PATH             "/"
#define DBUS_INTERFACE_NAME             "org.freedesktop.DBus.ObjectManager"
#define DBUS_METHOD_NAME                "GetManagedObjects"
#define DBUS_METHOD_RETURN_TYPE         "(a{oa{sa{sv}}})"

/*
* Return Bluetooth address if the device is paired. Else return NULL
*
* @param properties: An 'object' that holds a specific bluetooth device's metadata
* @return: Return Bluetooth address if the device is paired. Else return NULL
*/
char *check_is_paired(GVariant *properties);

/*
*Return a list of paired devices
* 
* @param num_of_paired: the number of devices that are paired
*   NOTE: The number is set in this function
* @param result: The object returned by request to DBUS for all managed bluetooth object
* @return: returns a list of paired devices. Need to free
*/
char **process_dbus_bt_list(GVariant *result, int *num_of_paired);

/*
* Return a list of Bluetooth addresses that is currently paired (connected) to the host
*
* Approach: Use dbus to list all the bluetooth devices and see the connected property
*           The paired property only indicates if the device has been paired with the host before and not if it is currently paired
*
* @param num_of_paired: the number of devices paired to the host
*   NOTE: the value is set within the helper function that the function will call
* @return: return a list of bluetooth addresses connected to the host 
*/
char **get_paired_devices(int *num_of_paired);
#endif