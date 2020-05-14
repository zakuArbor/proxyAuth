#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>


#define GNOME_SESSION_DBUS_NAME                 "org.gnome.SessionManager"
#define GNOME_SESSION_DBUS_OBJ_PATH_PRESENCE    "/org/gnome/SessionManager/Presence"
#define GNOME_SESSION_DBUS_INTERFACE_PRESENCE   "org.gnome.SessionManager.Presence"

struct server_data_t {
    int server;
    int *client;
    sdp_session_t *session;
};

struct dbus_obj {
    GMainContext *context;
    GDBusProxy *proxy;
    gulong handler_id;
    int server;
    int *client;
    sdp_session_t *session;
};

//Referenced https://gitlab.gnome.org/GNOME/glib/-/blob/master/gio/tests/gdbus-example-watch-proxy.c

extern void terminate_server(int server, int client, sdp_session_t *session);

void terminate(struct dbus_obj *data_obj);

/*
* Terminate program if user is locked by actively "listening"/monitoring the changes in presence status
*
* Signal Handler whenever the property of status changes
*
* @param proxy:
* @param sender_name:
* @param signal_name:
* @param parameters:
* @param user_data: a pointer to data that needs to be freed before terminating the program
*/
void on_signal (
    GDBusProxy *proxy, 
    gchar *sender_name, 
    gchar *signal_name, 
    GVariant *parameters, //in the form of (u)
    gpointer user_data
);

/*
* Calls and sets all the necessary data to listen for the status of the user's session (i.e. is the session locked)
*
* @param server: a struct that contains the server and client's fd and the Bluetooth's SDP server object
*
* NOTE: returns a reference to a struct dbus_obj that needs to be freed
* @return an object that contains all the necessary data to listen for the lock status and the data that needs to be freed when program terminate
*/
struct dbus_obj *set_lock_listener(struct server_data_t *server);

void check_lock_status(GMainContext *context);