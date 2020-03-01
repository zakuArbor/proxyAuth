#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>

#define DEVICE 20:DA:22:DE:0F:68

static void bluez_property_value(const gchar *key, GVariant *value)
{
	const gchar *type = g_variant_get_type_string(value);
	if (!g_ascii_strcasecmp("Address", key)) {
		g_print("\t\t%s: %s\n", key, g_variant_get_string(value, NULL));
	}
	if (!g_ascii_strcasecmp("Connected", key) && g_variant_get_boolean(value) == 1) {
		g_print("\t\t%s (%c) : %d\n", key, *type, g_variant_get_boolean(value));
	}

}

int main() {
	GDBusConnection *conn;

	GVariant *result = NULL;
	GVariantIter i;
	const gchar *object_path;
	GVariant *ifaces_and_properties;
	//GMainLoop *loop;

	if(!(conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL))) {
		g_print("Not able to get connection to system bus\n");
		return 1;
	}

	result = g_dbus_connection_call_sync(
		conn,									//connection
		"org.bluez",							//bus_name
		"/",									//object_path
		"org.freedesktop.DBus.ObjectManager",	//interface_name
		"GetManagedObjects",					//method_name
		NULL,									//parameters
		G_VARIANT_TYPE("(a{oa{sa{sv}}})"),		//return type
		G_DBUS_CALL_FLAGS_NONE,					//flag
		1000,									//timeout_msec
		NULL,									//cancel error
		NULL									//error if parameter is not compatible with D-Bus protocol
	);

	if(result) {
		result = g_variant_get_child_value(result, 0);
		g_variant_iter_init(&i, result);
		while(g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
			const gchar *interface_name;
			GVariant *properties;
			GVariantIter ii;
			g_variant_iter_init(&ii, ifaces_and_properties);
			while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
				if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
					g_print("[ %s ]\n", object_path);
 					const gchar *property_name;
					GVariantIter iii;
					GVariant *prop_val;
					g_variant_iter_init(&iii, properties);
					while(g_variant_iter_next(&iii, "{&sv}", &property_name, &prop_val))
						bluez_property_value(property_name, prop_val);
					g_variant_unref(prop_val);
				}
				g_variant_unref(properties);
			}
			g_variant_unref(ifaces_and_properties);
		}
		g_variant_unref(result);
	}

	g_object_unref(conn);

	return 0;
}