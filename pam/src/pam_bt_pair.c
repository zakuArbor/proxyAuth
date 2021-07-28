#include "pam_bt_pair.h"

char *check_is_paired(GVariant *properties) {
    char *addr = NULL;

    GVariantDict prop_dict;
    GVariant *is_connected_gvariant;
    GVariant *is_trusted_gvariant;
    GVariant *addr_gvariant;

    g_variant_dict_init(&prop_dict, properties);
    
    if (!(is_connected_gvariant = g_variant_dict_lookup_value(&prop_dict, "Connected", G_VARIANT_TYPE("b")))) {
        perror("No such key or Unexpected type");
        return NULL;
    }
    
    if (!(is_trusted_gvariant = g_variant_dict_lookup_value(&prop_dict, "Trusted", G_VARIANT_TYPE("b")))) {
        perror("No such key or Unexpected type");
        return NULL;
    }
    
    if (g_variant_get_boolean(is_connected_gvariant) && g_variant_get_boolean(is_trusted_gvariant)) {
        if (!(addr_gvariant = g_variant_dict_lookup_value(&prop_dict, "Address", G_VARIANT_TYPE("s")))) {
            perror("No such key or Unexpected type");
            return NULL;
        }

        g_print("pika: %s\n", g_variant_get_string(addr_gvariant, NULL));

        if (!(addr = malloc(sizeof(char) * BT_MAC_LEN + 1))) {
            perror("malloc");
            return NULL;
        }

        strncpy(addr, (char *)g_variant_get_string(addr_gvariant, NULL), BT_MAC_LEN);
        addr[BT_MAC_LEN] = '\0';
    }

    return addr;
}

char **process_dbus_bt_list(GVariant *result, int *num_of_paired) {
    if(!result) {
        return NULL;
    }

    char **paired_devices = NULL;
    int num_of_paired_lc = 0;

    if (!(paired_devices = malloc(sizeof(char *) *BT_MAX_CONN))) {
        perror("malloc");
        return NULL;
    }

    GVariantIter i;
    const gchar *object_path;
    GVariant *ifaces_and_properties;

    result = g_variant_get_child_value(result, 0);
    g_variant_iter_init(&i, result);
    while(g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
            gchar *interface_name_lower = g_ascii_strdown(interface_name, -1);
            if(g_strstr_len(interface_name_lower, -1, "device")) {
                char *addr;
                if ((addr = check_is_paired(properties))) {
                    paired_devices[num_of_paired_lc] = addr;
                    num_of_paired_lc++;
                }
            }
            g_free(interface_name_lower);
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }

    *num_of_paired = num_of_paired_lc;
    return paired_devices;
}

char **get_paired_devices(int *num_of_paired) {
    GDBusConnection *conn;

    GVariant *result = NULL;
    char **paired_devices = NULL;

    if(!(conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL))) {
        g_print("Not able to get connection to system bus\n");
        return NULL;
    }

    result = g_dbus_connection_call_sync(
        conn,                                       //connection
        BLUEZ_DBUS_NAME,                            //bus_name
        BLUEZ_DBUS_OBJ_PATH,                        //object_path
        DBUS_INTERFACE_NAME,                        //interface_name
        DBUS_METHOD_NAME,                           //method_name
        NULL,                                       //parameters
        G_VARIANT_TYPE(DBUS_METHOD_RETURN_TYPE),    //return type
        G_DBUS_CALL_FLAGS_NONE,                     //flag
        1000,                                       //timeout_msec
        NULL,                                       //cancel error
        NULL                                        //error if parameter is not compatible with D-Bus protocol
    );

    paired_devices = process_dbus_bt_list(result, num_of_paired);

    g_variant_unref(result);

    g_object_unref(conn);

    return paired_devices;
}
