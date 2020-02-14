#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>



int main(int argc, char **argv)

{

    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    
    char buf[1024] = { 0 };
    char bindBuf[30] = { 0 };

    char bindAddr[30] = "A4:50:46:19:C1:E6";

    int s, client, bytes_read;

    socklen_t opt = sizeof(rem_addr);


    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter

    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_channel = (uint8_t) 1;
    str2ba(bindAddr, &loc_addr.rc_bdaddr);

    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    ba2str(&loc_addr.rc_bdaddr, bindBuf);
    printf("Binded to add: %s\n", bindBuf);

    // put socket into listening mode
    listen(s, 1);


    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str(&rem_addr.rc_bdaddr, buf);
    printf("Accepted connection from %s\n", buf);

    memset(buf, 0, sizeof(buf));

    // read data from the client
    bytes_read =  read(client, buf, sizeof(buf));

    if( bytes_read > 0 ) {

        printf("received [%s]\n", buf);

    }

    // close connection
    close(client);
    close(s);

    return 0;

}
