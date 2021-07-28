#include "proxy_dbus.h"

void terminate(struct dbus_obj *data_obj) {
    if (data_obj->proxy) {
        if (data_obj->handler_id > 0) {
            g_signal_handler_disconnect(data_obj->proxy, data_obj->handler_id);
        }
        g_object_unref(data_obj->proxy);
    }

    if (data_obj->context) {
        g_main_context_unref(data_obj->context);
    }

    terminate_server(data_obj->server, *(data_obj->client), data_obj->session);

    exit(0);
}

void on_signal (
        GDBusProxy *proxy, 
        gchar *sender_name, 
        gchar *signal_name, 
        GVariant *parameters, //in the form of (u)
        gpointer user_data
) {    
    GVariant *value;
    guint32 num = -1;
    if ((value = g_variant_get_child_value(parameters, 0))) { //extract the value from the tuple
        num = g_variant_get_uint32(value);
        if (num == 3){
            g_print("signal: %d\n", num);
            terminate(user_data);
        }
    }
}

struct dbus_obj *set_lock_listener(struct server_data_t *server) {
    struct dbus_obj *data_obj = NULL;

    if (!(data_obj = malloc(sizeof(struct dbus_obj)))) {
        return NULL;
    }

    if (!server) {
        return NULL;
    }

    data_obj->server = server->server;
    data_obj->client = server->client;
    data_obj->session = server->session;
    
    GError *error = NULL;

    data_obj->proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SESSION,                     //GBus Type
        G_DBUS_PROXY_FLAGS_NONE,                //Flag to use for constructing proxy
        NULL,                                   //GDBusInterfaceInfo
        GNOME_SESSION_DBUS_NAME,                //Bus Name
        GNOME_SESSION_DBUS_OBJ_PATH_PRESENCE,   //Object Path
        GNOME_SESSION_DBUS_INTERFACE_PRESENCE,  //DBus Interface
        NULL,                                   //GCancellable
        &error                                  //Error struct
    );

    if (data_obj->proxy == NULL && error) {
        g_printerr ("Error creating proxy: %s\n", error->message);
        g_error_free (error);
        terminate(data_obj);
    }

    data_obj->handler_id = g_signal_connect(data_obj->proxy, "g-signal", G_CALLBACK(on_signal), data_obj);
    data_obj->context = g_main_context_default();

    return data_obj;
}

void check_lock_status(GMainContext *context) {
    for (int i = 0; i < 15; i++) {
        g_main_context_iteration(context, FALSE);
    }
}
