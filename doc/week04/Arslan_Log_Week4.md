# WEEK 4 LOG
#### Author: Arslan Ahmed Qamar
---

## OBJECTIVES
* Setup the bluetooth adaptor that can allow us to connect to bluetooth in virtual environment
* Peform testing 

## SOURCES USED 
* https://www.tecmint.com/configure-pam-in-centos-ubuntu-linux/
* https://people.csail.mit.edu/albert/bluez-intro/
* https://raspberrypi.stackexchange.com/questions/41776/failed-to-connect-to-sdp-server-on-ffffff000000-no-such-file-or-directory

## TODO
* We have to used the bluetooth adaptor since Virtual machine doesn't scan the bluetooth hardware from the machine. 
---

## BLUETOOTH
### SETUP
* We will be using the bluez for bluetooth in vitual box. Install the bluetooth library
`moon@moon-ThinkPad-T61:~/proxyAuth$ sudo apt install bluez`
```moon@moon-ThinkPad-T61:~/proxyAuth$ sudo apt-get install libbluetooth-dev
[sudo] password for moon: 
Reading package lists... Done
Building dependency tree       
Reading state information... Done
The following NEW packages will be installed:
  libbluetooth-dev
0 upgraded, 1 newly installed, 0 to remove and 0 not upgraded.
Need to get 152 kB of archives.
After this operation, 591 kB of additional disk space will be used.
Get:1 http://ca.archive.ubuntu.com/ubuntu bionic-updates/main amd64 libbluetooth-dev amd64 5.48-0ubuntu3.3 [152 kB]
Fetched 152 kB in 1s (144 kB/s)          
Selecting previously unselected package libbluetooth-dev.
(Reading database ... 124779 files and directories currently installed.)
Preparing to unpack .../libbluetooth-dev_5.48-0ubuntu3.3_amd64.deb ...
Unpacking libbluetooth-dev (5.48-0ubuntu3.3) ...
Setting up libbluetooth-dev (5.48-0ubuntu3.3) ...
```
* Verify if bluez detects the bluetooth adaptor 
```
moon@moon-ThinkPad-T61:~/proxyAuth$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 00:1E:4C:FD:65:18  ACL MTU: 1017:8  SCO MTU: 64:8
	UP RUNNING 
	RX bytes:1018 acl:0 sco:0 events:47 errors:0
	TX bytes:939 acl:0 sco:0 commands:47 errors:0
```
* Ccompile the simple.c
`gcc -o simplescan simple.c -lbluetooth`
* **As Before,** even after running the simplescan for many times, it won't show any devices. I also run the hcitool scan but that didn't work either 
* **But Now** It does show the devices. 
```
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ ./simplescan 
AC:1F:74:45:C6:AA  Arslan Ahmed iPhone
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ ./simplescan 
AC:1F:74:45:C6:AA  Arslan Ahmed iPhone
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ ./simplescan 
AC:1F:74:45:C6:AA  [unknown]

moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ hcitool scan
Scanning ...
	AC:1F:74:45:C6:AA	Arslan Ahmed iPhone
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ hcitool scan
Scanning ...
	AC:1F:74:45:C6:AA	n/a
```
* **The solution,** I have to do the dualboot. 
* Now, run the rfcomm-server.c program, make sure your bluetooth is turned on and paired, and connected to device with the android app. 
```
$ gcc rfcomm-server.c -l bluetooth
$ ./a.out 
Registering UUID 00001101-0000-1000-8000-00805f9b34fb
Segmentation fault (core dumped)
```
* The seg fault occured, when callinng the sdp_record_register(). It register the bluetooth service in SDP using BlueZ. Apparently, the seg fault is coming from broken stptool in BLuez. 
* Run this command and enter your sudo password 
```
$ sudo vim /etc/systemd/system/dbus-org.bluez.service
Welcome arslan
This is simple PAM module
[sudo] password for arslan: 
Acct mgmt
```
* Then change this line `ExecStart=/usr/lib/bluetooth/bluetoothd` to `ExecStart=/usr/lib/bluetooth/bluetoothd --compat`
* Restart the blietooth like this and change the permission
```
$ sudo systemctl daemon-reload
$ sudo systemctl restart bluetooth
$ sudo chmod 777 /var/run/sdp
```
*  Now connected the device again. Connection successful, able to recieve the hellow world message from the andorid app. 
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








