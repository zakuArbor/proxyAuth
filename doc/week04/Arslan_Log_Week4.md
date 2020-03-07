# WEEK 4 LOG
#### Author: Arslan Ahmed Qamar
---

## OBJECTIVES
* **Previously,** setup the Bluetooth adaptor that can allow us to connect to Bluetooth within Virtual Environment but it was really slow because of two reasons: running the Virtual Machine and using the external Bluetooth Adaptor 
* Perform the Dual-boot, Implement the SSH Server 

## SOURCES USED 
* https://www.tecmint.com/configure-pam-in-centos-ubuntu-linux/
* https://people.csail.mit.edu/albert/bluez-intro/
* https://raspberrypi.stackexchange.com/questions/41776/failed-to-connect-to-sdp-server-on-ffffff000000-no-such-file-or-directory
* https://www.lifewire.com/ultimate-windows-7-ubuntu-linux-dual-boot-guide-2200653
* https://linuxize.com/post/how-to-enable-ssh-on-ubuntu-18-04/

## TODO
* Peform the dual-boot 
* Install the packages/tools/dependencies
* Configure the SSH Server for Connection Over Internet using Public IP Address (requires Port Forwarding)

---

## BLUETOOTH
### SETUP

* Log in to the server computer using Public IP Address
```
Arslans-MBP:proxyAuth arslanqamar$ ssh moon@99.251.252.240
moon@99.251.252.240's password: 
Welcome to Ubuntu 18.04.4 LTS (GNU/Linux 4.13.0-37-generic x86_64)
```

* Using the Bluez for Bluetooth in Virtual Box. Install the Bluetooth library 
`moon@moon-ThinkPad-T61:~/proxyAuth$ sudo apt install bluez`
```
moon@moon-ThinkPad-T61:~/proxyAuth$ sudo apt-get install libbluetooth-dev
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
* Verify if Bluez detects the Bluetooth adaptor
```
moon@moon-ThinkPad-T61:~/proxyAuth$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 00:1E:4C:FD:65:18  ACL MTU: 1017:8  SCO MTU: 64:8
	UP RUNNING 
	RX bytes:1018 acl:0 sco:0 events:47 errors:0
	TX bytes:939 acl:0 sco:0 commands:47 errors:0
```
* Compile the simple.c
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
* **Solution,** Perform dual-boot
* Now, run the rfcomm-server.c program, make sure your Bluetooth is turned on and paired, and connected to device with the Android App.
```
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ gcc -o server rfcomm-server.c -l bluetooth
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ ./server 
Registering UUID 00001101-0000-1000-8000-00805f9b34fb
Segmentation fault (core dumped)
```
* The seg fault occured, when callinng the sdp_record_register(). It register the Bluetooth service in SDP using BlueZ. Apparently, the seg fault is coming from broken stptool in BLuez. 
* Run this command and enter your sudo password 
```
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ sudo vim /etc/systemd/system/dbus-org.bluez.service

```
* Then change this line **FROM** `ExecStart=/usr/lib/bluetooth/bluetoothd` **TO** `ExecStart=/usr/lib/bluetooth/bluetoothd --compat`
* Restart the Blietooth and change the permissions
```
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ sudo systemctl daemon-reload
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ sudo systemctl restart bluetooth
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ sudo chmod 777 /var/run/sdp
```
*  Now connect the device again. Connection Successful, able to recieve the [Hello World!] message from the Andorid App (without pressing any keys)
```
moon@moon-ThinkPad-T61:~/proxyAuth/bluetoothBasics$ ./server
Registering UUID 00001101-0000-1000-8000-00805f9b34fb
accepted connection from 50:46:5D:1F:74:E6
received [Hello World!]
^C
```








