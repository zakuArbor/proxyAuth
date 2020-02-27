# WEEK 3 LOG
#### Author: Arslan Ahmed Qamar

---

## OBJECTIVES
* Setup the bluetooth adaptor that can allow us to connect to bluetooth in virtual environment
* Peform testing 

## SOURCES USED 
* https://www.tecmint.com/configure-pam-in-centos-ubuntu-linux/
* https://people.csail.mit.edu/albert/bluez-intro/

## TODO
* We have to used the bluetooth adaptor since Virtual machine doesn't scan the bluetooth hardware from the machine. 
* 

---

## BLUETOOTH
### SETUP
* We will be using the bluez for bluetooth in vitual box. Install the bluetooth library
`sudo apt install bluez`
`sudo apt-get install libbluetooth-dev`
* Verify if bluez detects the bluetooth adaptor 
```
$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 5C:F3:70:99:36:59  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING PSCAN ISCAN INQUIRY 
	RX bytes:2528 acl:0 sco:0 events:114 errors:0
	TX bytes:4944 acl:0 sco:0 commands:70 errors:0
```
* Ccompile the simple.c
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



