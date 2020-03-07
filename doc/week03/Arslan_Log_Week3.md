# WEEK 3 LOG
#### Author: Arslan Ahmed Qamar

---

## OBJECTIVES
* Setup the Bluetooth adaptor that can allow us to connect to Bluetooth within Virtual Environment
* Peform testing 

## SOURCES USED 
* https://www.tecmint.com/configure-pam-in-centos-ubuntu-linux/
* https://people.csail.mit.edu/albert/bluez-intro/
* https://raspberrypi.stackexchange.com/questions/41776/failed-to-connect-to-sdp-server-on-ffffff000000-no-such-file-or-directory

## TODO
* We have to used the Bluetooth adaptor since Virtual machine doesn't scan the Bluetooth hardware from the machine. 

---

## BLUETOOTH
### SETUP
* We will be using the Bluez for Bluetooth in Virtual Box. Install the Bluetooth library
`sudo apt install bluez`
`sudo apt-get install libbluetooth-dev`
* Verify if Bluez detects the Bluetooth adaptor 
```
$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 5C:F3:70:99:36:59  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING PSCAN ISCAN INQUIRY 
	RX bytes:2528 acl:0 sco:0 events:114 errors:0
	TX bytes:4944 acl:0 sco:0 commands:70 errors:0
```
* Compile the simple.c
`gcc -o simplescan simple.c -lbluetooth`
* Even after running the simple for many times, it won't show any devices. I also run the hcitool scan but that didn't work either 
```
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ gcc -o simple simple.c -lbluetooth
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ls
README.md  rfcomm-server.c  simple  simple.c  simplescan
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ gcc -o simple simple.c -lbluetooth
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C  
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ ./simple
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ hcitool scan
Scanning ...
^C
```
* It did pick up one device in 20 and so attempts. I still have to find the **solution** for this. 
* Now, run the rfcomm-server.c program, make sure your Bluetooth is turned on and paired, and connected to device with the Android App. 
```
$ gcc rfcomm-server.c -l bluetooth
$ ./a.out 
Registering UUID 00001101-0000-1000-8000-00805f9b34fb
Segmentation fault (core dumped)
```
* The seg fault occured, when callinng the sdp_record_register(). It register the bluetooth service in SDP using BlueZ. Apparently, the seg fault is coming from broken stptool in BLuez. 
* Run this command and enter your sudo password (if asked)
```
$ sudo vim /etc/systemd/system/dbus-org.bluez.service
Welcome arslan
This is simple PAM module
[sudo] password for arslan: 
Acct mgmt
```
* Then change this line from `ExecStart=/usr/lib/bluetooth/bluetoothd` to `ExecStart=/usr/lib/bluetooth/bluetoothd --compat`
* Restart the Bluetooth and change the permission
```
$ sudo systemctl daemon-reload
$ sudo systemctl restart bluetooth
$ sudo chmod 777 /var/run/sdp
```
*  Now connect the device again. Connection successful, able to recieve the [Hello World!] message from the Andorid App. 
```$ ./a.out 
Registering UUID 00001101-0000-1000-8000-00805f9b34fb
accepted connection from 50:46:5D:1F:74:E6
hireceived [Hello World!]
Send message to device:
hello
received [Hello World!]
Send message to device:
hello 
received [Hello World!Hello World!Hello World!]
Send message to device:
^C
arslan@arslan-VirtualBox:~/proxyAuth/bluetoothBasics$ 
```






