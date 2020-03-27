#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <errno.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "pam_misc.h"
#include "pam_bt_misc.h"
#include "pam_bt_pair.h"
#include "pam_bt_trust.h"


#define SERVICE_NAME "Proxy Auth"
#define SERVICE_DESC "Continuous Authentication via Bluetooth"
#define SERVICE_PROV "ProxyAuth"
#define minThroughput 0  //this value needs to be calibrated according to specific devices. 

struct server_data_t {
    int server;
    int *client;
    sdp_session_t *session;
};

/*
* Special Thanks to: Ryan Scott for providing how to register service and Albert Huang
*/

sdp_session_t *sdp_connect( const bdaddr_t *src, const bdaddr_t *dst, uint32_t flags );

int sdp_close( sdp_session_t *session );

int sdp_record_register(sdp_session_t *sess, sdp_record_t *rec, uint8_t flags);

void terminate_server(int server, int client, sdp_session_t *session) {
    if (client) {
        close(client);
    }
    if (server) {
        close(server);
    }
    if (session) {
        sdp_close(session);
    }
}

/*
* Set the general service ID and service class
*/
void set_service(uuid_t *svc_uuid, uuid_t *svc_class_uuid, sdp_list_t **svc_class_list, sdp_record_t *record, uint32_t *svc_uuid_int) {
    /* set the general service ID */

    //set svc_uuid (service uuid) by creating an unreserved 128-bit UUID from svc_uuid_int (a list of 4 32-bit uuid).
    //sdp_uuid128_create is similar to str2ba function which converts the uint32_t uuid (rather than a string) to uuid_t type
    sdp_uuid128_create(svc_uuid, svc_uuid_int);
    sdp_set_service_id(record, *svc_uuid);

    char str[256] = "";
    sdp_uuid2strn(svc_uuid, str, 256);
    printf("Registering UUID %s\n", str);

    // set the service class
    sdp_uuid16_create(svc_class_uuid, SERIAL_PORT_SVCLASS_ID); //create a Service Class ID
    *svc_class_list = sdp_list_append(0, svc_class_uuid);
    sdp_set_service_classes(record, *svc_class_list);
}

/*
* Set Bluetooth profile metadata for the service and the version of the profile
*/
void set_bluetooth_service_info(sdp_profile_desc_t *profile, sdp_list_t **profile_list, sdp_record_t *record) {
    // set the Bluetooth profile information
    sdp_uuid16_create(&(profile->uuid), SERIAL_PORT_PROFILE_ID); //create a Profile ID
    profile->version = 0x0100;
    *profile_list = sdp_list_append(0, profile);
    sdp_set_profile_descs(record, *profile_list);
}

/*
* Make the service record publicly browsable. Allows remote bluetooth devices to see the service record.
*/
void set_browsable(sdp_list_t **root_list, sdp_record_t *record, uuid_t *root_uuid) {
    sdp_uuid16_create(root_uuid, PUBLIC_BROWSE_GROUP);
    *root_list = sdp_list_append(0, root_uuid);
    sdp_set_browse_groups(record, *root_list);
}

/*
* set l2cap information
*/
void set_l2cap_info(sdp_list_t **l2cap_list, sdp_list_t **proto_list, uuid_t *l2cap_uuid) {
    sdp_uuid16_create(l2cap_uuid, L2CAP_UUID); 
    *l2cap_list = sdp_list_append(0, l2cap_uuid);
    *proto_list = sdp_list_append(0, *l2cap_list);
}

/*
* register the RFCOMM channel for RFCOMM sockets
*/
void register_rfcomm_sock(
    sdp_data_t **channel, 
    sdp_record_t *record, 
    sdp_list_t **rfcomm_list, 
    sdp_list_t **proto_list, 
    sdp_list_t **access_proto_list,
    uuid_t *rfcomm_uuid, 
    uint8_t *rfcomm_channel
    ) {
    // register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(rfcomm_uuid, RFCOMM_UUID);
    *channel = sdp_data_alloc(SDP_UINT8, rfcomm_channel);
    *rfcomm_list = sdp_list_append(0, rfcomm_uuid);
    sdp_list_append(*rfcomm_list, *channel);
    sdp_list_append(*proto_list, *rfcomm_list);

    *access_proto_list = sdp_list_append(0, *proto_list);    
    sdp_set_access_protos(record, *access_proto_list);
}

sdp_session_t *register_service(uint8_t rfcomm_channel) {

    /* A 128-bit number used to identify this service. The words are ordered from most to least
    * significant, but within each word, the octets are ordered from least to most significant.
    * For example, the UUID represneted by this array is 00001101-0000-1000-8000-00805F9B34FB. (The
    * hyphenation is a convention specified by the Service Discovery Protocol of the Bluetooth Core
    * Specification, but is not particularly important for this program.)
    *
    * This UUID is the Bluetooth Base UUID and is commonly used for simple Bluetooth applications.
    * Regardless of the UUID used, it must match the one that the Armatus Android app is searching
    * for.
    *
    * NOTE: RFCOMM uses L2CAP as a transport
    * 
    */
    //maintain a list of four 32-bit uuid which will be used to form a 128-bit uuid for setting a uuid to our service
    uint32_t svc_uuid_int[] = { 0x01110000, 0x00100000, 0x80000080, 0xFB349B5F };
    
    uuid_t root_uuid; 
    uuid_t l2cap_uuid; 
    uuid_t rfcomm_uuid;
    uuid_t svc_uuid;
    uuid_t svc_class_uuid;

    //NOTE: sdp_list_t is a linkedlist which needs to be free using sdp_list_free after use
    sdp_list_t *l2cap_list = 0,
                *rfcomm_list = 0,
                *root_list = 0,
                *proto_list = 0,
                *access_proto_list = 0,
                *svc_class_list = 0,
                 *profile_list = 0;
    sdp_data_t *channel = 0; 
    sdp_profile_desc_t profile;
    sdp_record_t *record = sdp_record_alloc();//{ 0 };
    sdp_session_t *session = 0;

    const char *service_name = SERVICE_NAME;
    const char *svc_dsc = SERVICE_DESC;
    const char *service_prov = SERVICE_PROV;

    set_service(&svc_uuid, &svc_class_uuid, &svc_class_list, record, svc_uuid_int);
    
    set_bluetooth_service_info(&profile, &profile_list, record);

    set_browsable(&root_list, record, &root_uuid);

    set_l2cap_info(&l2cap_list, &proto_list, &l2cap_uuid);

    register_rfcomm_sock(&channel, record, &rfcomm_list, &proto_list, &access_proto_list, &rfcomm_uuid, &rfcomm_channel);

    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, svc_dsc);

    // connect to the local SDP server, register the service record, and disconnect
    /*
    * NOTE: Server needs to advertise the Bluetooth Server, so we will use IPC method to tell sdpd
    *   (Bluez implementation of SDP server which is a daemon) what to advertise. This is done through
    *   the pipe `/var/run/sdp`
    */

    session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, record, 0);

    // cleanup
    sdp_data_free(channel);
    sdp_list_free(l2cap_list, 0);
    sdp_list_free(rfcomm_list, 0);
    sdp_list_free(root_list, 0);
    sdp_list_free(proto_list, 0);
    sdp_list_free(access_proto_list, 0);
    sdp_list_free(svc_class_list, 0);
    sdp_list_free(profile_list, 0);
    sdp_record_free(record);
    return session;
}

/*
* Setup the bluetooth server
* 
* @return return the server's socket file descriptor
*/ 
int init_server(struct sockaddr_rc *loc_addr, sdp_session_t **session) {
    // allocate socket
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    int port = 1;

    // local bluetooth adapter
    loc_addr->rc_family = AF_BLUETOOTH;
    loc_addr->rc_bdaddr = *BDADDR_ANY;
    loc_addr->rc_channel = (uint8_t) 1;

    // bind socket to port 1 of the first available 
    bind(s, (struct sockaddr *)loc_addr, sizeof(*loc_addr));

    //register service added
    *session = register_service(port);

    // put socket into listening mode
    listen(s, port);

    return s;   
}

/*
* Lock the computer, cleanup memory and open fd, and terminate program
*
* @param server_data: a struct that contains fd that needs to be closed
*/
void lock(struct server_data_t *server_data) {
    system("dbus-send --type=method_call --dest=org.gnome.ScreenSaver /org/gnome/ScreenSaver org.gnome.ScreenSaver.Lock");
    if (server_data) {
        terminate_server(server_data->server, *(server_data->client), server_data->session);
        exit(0);
    }
}

/*
* Return 1 iff the given bluetooth address in the argument is valid
*
* @param argc: number of arguments (always >= 1 due to program name stored in argv[0])
* @param argv: array that contains cmdline arguments
* @return: True iff the cmd argument is a valid bluetooth address
*/
int check_arg(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "usage: %s bt_addr\n", argv[0]);
        return 0;
    }

    if (!verify_bt_addr(argv[1], NULL)) {
        fprintf(stderr, "%s: %s is not a valid bluetooth address\n", argv[0], argv[1]);
        return 0;
    }
    return 1;
}

/*
* Return 1 iff the given bluetooth address is trusted and paired
*
* @param bt_addr: the address of the bluetooth to check
* @return: True iff given bluetooth address is a trusted and pired device
*/
int is_trusted_client(char *bt_addr, const char *trusted_dir_path) {
    int status = 0;

    int num_of_paired, num_of_devices;
    char *username = getlogin();

    char **paired_devices = get_paired_devices(&num_of_paired);
    char **trusted_devices;

    if (!(trusted_devices = find_trusted_devices(NULL, trusted_dir_path, username, &num_of_devices))) {
        goto is_trusted_terminate;
    }

    //check if device is paired
    if (!(is_dev_trusted(NULL, bt_addr, paired_devices, num_of_paired))) {
        goto is_trusted_terminate;
    }

    //check if device is in trusted list
    if (!is_dev_trusted(NULL, bt_addr, trusted_devices, num_of_devices)) {
        goto is_trusted_terminate;
    }

    status = 1;

is_trusted_terminate:
    if (trusted_devices) {
        free_device_list(trusted_devices, num_of_devices);
    }

    if (paired_devices) {
        free_device_list(paired_devices, num_of_paired);
    }

    return status;
}

/*
* Connect a new client. If client is not from a trusted device nor authorized then lock the system.
*
* @param s: server's socket
* @param rem_addr: a pointer to sockaddr structure that will store the address of the client socket
* @param opt: the size of rem_addr
* @param authorized_dev: the address of the trusted device
* @param server_data: a struct that contains fd that needs to be closed before termination
* @return: The client's socket
*/
int connect_client(int s, struct sockaddr_rc *rem_addr, socklen_t *opt, char *authorized_dev, struct server_data_t *server_data) {
    // accept one connection
    char buf[1024] = { 0 };
    int client = accept(s, (struct sockaddr *)rem_addr, opt);
    fcntl(client, F_SETFL, O_NONBLOCK); //set FD to nonblocking 

    //bdaddr_t stores information about the bluetooth device address.
    ba2str(&(rem_addr->rc_bdaddr), buf); //converts the bluetooth data structure to string

    fprintf(stderr, "accepted connection from %s\n", buf);

    if (!is_trusted_client(buf, trusted_dir_path) || strcmp(buf, authorized_dev) != 0) {
        printf("%s is not trusted or not authorized to deauthenticate the system\n", buf);
        lock(server_data);
    }

    return client;
}

int main (int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    int server = -1, client = -1, bytes_read;
    socklen_t opt = sizeof(rem_addr);
    sdp_session_t *session = NULL; //SDP socket

    if (!check_arg(argc, argv)) {
        lock(NULL);
    }
    //check if the device passed is trusted
    if (!is_trusted_client(argv[1], trusted_dir_path)) {
        lock(NULL);
    }


    server = init_server(&loc_addr, &session);

    struct server_data_t server_data = {server, &client, session};

    time_t start, stop;
    int is_locked = 0; 

    //listen_lock_status(server, &client, session);
    client = -1; 
    msgHead = 0; 
    while(1) {
        if (client < 0) {
            client = connect_client(server, &rem_addr, &opt, argv[1], &server_data);
            start = time(NULL);
            is_locked = 0; 
        }

    	char buf[1024];
    	memset(buf, 0, sizeof(buf));

    	// read data from the client
    	bytes_read = read(client, buf, sizeof(buf));
    	if(bytes_read > 0) {
            printf("received [%s]\n", buf);
            start = time(NULL);
            is_locked = 0; 
    	}
        
        if(strlen(buf) > 0 && msgHead < 1024){ //read something, lets append some data to the msg array
            msgHead++; 
        }
        
        //when 10 units of time have passsed check bandwidth in units of writes///need to convert this to a more specific/accurate unit of measurement
        if ((stop-start) > 10){
            double throughput = msgHead/(stop-start); 
            msgHead = 0; 
            if (throughput < minThroughput){
                is_locked = 1;
                lock(&server_data);
                break;
            }
        }
        
        stop = time(NULL);  
        if ((stop - start) > 10 && !is_locked){ //check that 
            //exec no response being read lock user out
            is_locked = 1;
            lock(&server_data);
            break;
        }
    	
    	if (bytes_read > 0 && write(client, buf, strlen(buf) < 0)) {
    	    perror("Error writing to client");	
    	}
    }

    return 0;
}
