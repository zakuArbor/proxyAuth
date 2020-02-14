# WEEKLY LOG 2

**Author:** Ju Hong Kim
**Date:** 02/07/2020 - 02/14/2020

***

## SUMMARY OF TASKS
* Organize team to specific tasks
* Guide teammates with their tasks
* Wrote MAKE file for PAM
* Worked on integrating bluetooth with PAM
* Tested the write server Anurag wrote

## PROBLEMS
* Development must be on VM because we do not have root access to the DH Machines
* Development on VM protects us from accidentally locking ourselves permanently from the machines
* I cannot get my VM connect to my internal bluetooth on Virtualbox
* The lab machines do not have bluetooth so I cannot test if I can connect to bluetooth on VMWare

**Proposed Solutions:**
* Do testing on a spare machine that has bluetooth support
    * Spare machine must have GDM installed
* Buy a bluetooth adapter or any usb device that can allow us connect to bluetooth
* Risk your own personal machine by doing development on it
    * There's a risk of permanently locking yourself out
        * If you do not have disk encryption (which I do), you may be able to mount your harddrive onto another Linux Machine and modify PAM configuration.
    * The application being developed is probably not secure during development so you are exposing your personal computer to more vulnerabilities.

I have attempted to use a Raspberry Pi 3 B+ because it has bluetooth support but I gave up on it after my image became corrupted (i.e. Infinite loop in boot after setting up wifi). Not sure why, I'll have to investigate when I have free time. An issue I thought of is that I'll need to install GNOME Desktop on the Pi which may not be feasible due to the low specs of Raspberry Pi.

## BLUETOOTH
### SETUP
* install the bluetooth library: 
`sudo apt install bluez`
`sudo apt-get install libbluetooth-dev`
* check if BlueZ detects our bluetooth adapter: 
```
$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 1C:36:BB:23:B5:CB  ACL MTU: 1021:8  SCO MTU: 64:1
	DOWN 
	RX bytes:870 acl:0 sco:0 events:35 errors:0
	TX bytes:383 acl:0 sco:0 commands:35 errors:0
```
Looks like bluetooth is down
* turn on bluetooth (I usually disable bluetooth)
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam$ sudo hciconfig hci0 up
Can't init device hci0: Operation not possible due to RF-kill (132)
```
Seems like I cannot turn on Bluetooth. I recall reading about rfkill when working on the Raspberry Pi.
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam$ rfkill
ID TYPE      DEVICE        SOFT      HARD
 0 wlan      phy0     unblocked unblocked
 1 wlan      brcmwl-0 unblocked unblocked
 2 bluetooth hci0       blocked unblocked
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam$ rfkill unblock bluetooth
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam$ hciconfig | grep UP
	UP RUNNING 
```

* Look at the starter code for [simplescan](https://people.csail.mit.edu/albert/bluez-intro/c404.html#simplescan.c)
As you can see, there's problems with this implementation. It doesn't detect bluetooth devices very well. For instance, it only detected my phone only once even after running a scan for  more than 10 times. Even running `hcitool inq` could not find my device very well.
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
0C:54:15:F1:25:6F  VTPC
20:DA:22:DE:0F:68  kimju10
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
0C:54:15:F1:25:6F  VTPC
```

There's no issue detecting my phone on Ubuntu's bluetooth setting. I believe BlueZ may not optimized in userspace.
This may present a problem. I'll have to see if using a bluetooth adapter make things better. 

One interesting behavior I noticed when testing is that expensive phones are constantly detectable. I have tried detecting two friend's phone (not in the output), and the program was able to detect the phones.
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ src/simplescan 
bash: src/simplescan: No such file or directory
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
zaku@zaku-laptop:~/Documents/csc490/proxyAuth/pam/src$ ./simplescan 
04:B1:67:1A:75:B7  Redmi
```

## Bluetooth Auto Authentication
Since the scanner is unable to scan all bluetooth devices, I am going to work on Issue #1, authenticating based on detecting a hard-coded MAC Address, by choosing some bluetooth device that I can detect when I am working on it.

Working based on `simplescan.c` written by [**Albert Huang**](https://people.csail.mit.edu/albert/bluez-intro/index.html), I modified the code to findmy iphone (the scanner consistently detects my iphone 5s but not my cheap Huawei P30 Lite).

```
$ ./a.out 
Try to find device: C0:1A:DA:7A:30:B7
Found Device: C0:1A:DA:7A:30:B7
Device founddevice_found: 1
```

Some problems I had integrating the bluetooth scanner to PAM was compiling the program. After checking the logs to see why my PAM wasn't being run, I soon found out the program crashed due to unknown symbols. 

In `/var/log/auth.log/`:
```
Feb 11 01:45:30 zaku-laptop sudo: PAM unable to dlopen(pam_proxy.so): /lib/security/pam_proxy.so: undefined symbol: hci_inquiry
Feb 11 01:45:30 zaku-laptop sudo: PAM adding faulty module: pam_proxy.so
```

Manually compiling the file, I see the following errors:
```
$ gcc -lbluetooth pam_proxy.c 
/usr/bin/ld: /tmp/cc3ecGz7.o: in function `find_device':
pam_proxy.c:(.text+0x47): undefined reference to `hci_get_route'
/usr/bin/ld: pam_proxy.c:(.text+0x54): undefined reference to `hci_open_dev'
/usr/bin/ld: pam_proxy.c:(.text+0xd2): undefined reference to `hci_inquiry'
/usr/bin/ld: pam_proxy.c:(.text+0x11c): undefined reference to `ba2str'
/usr/bin/ld: /tmp/cc3ecGz7.o: in function `pam_sm_authenticate':
pam_proxy.c:(.text+0x23f): undefined reference to `pam_get_user'
collect2: error: ld returned 1 exit status
```

What is bizzare is that the order of the flag `-lbluetooth` actually matters. It must be placed after the c file.
```
$ gcc pam_proxy.c -lbluetooth
/usr/bin/ld: /tmp/ccLidfre.o: in function `pam_sm_authenticate':
pam_proxy.c:(.text+0x23f): undefined reference to `pam_get_user'
collect2: error: ld returned 1 exit status
```

After reading up some material and playing around with compiling and linking files, I found the proper way to compile and link the files:
```
gcc -c src/pam_proxy.c -fPIC -fno-stack-protector -o pam_proxy.o
sudo ld -x --shared -o /lib/security/pam_proxy.so pam_proxy.o -l bluetooth
```

### Replacing sudo
Similar to last week, I'll be first trying my PAM module on `sudo` to avoid locking myself. However, the rules we shall write onto the PAM modules should not be risky at all. But I am approaching development on the catious side.

On `/etc/pam.d/sudo`, I added the following rule:
```
#proxyAuth
auth sufficient pam_proxy.so
auth required pam_warn.so
```

#### Test: `sudo` should work without any passwords as long as my iphone is within range of my bluetooth's range
```
$ sudo echo Hello World
Try to find device: C0:1A:DA:7A:30:B7
Found Device: C0:1A:DA:7A:30:B7
Device foundWelcome zaku
This is a simple PAM says Pikachu
Hello World
```
**[PASSED]**


### Replace GDM Login
On `/etc/pam.d/gdm-password`, I shall add the following to near the top of the file:
```
#proxyAuth
auth sufficient pam_proxy.so
auth required pam_warn.so
```

#### Test with iPhone Bluetooth On:
**TIME:** 02:56
I should not require to write a password.
```
Tue Feb 11 02:56:52 2020: Device C0:1A:DA:7A:30:B7 found
Tue Feb 11 02:56:52 2020: zaku logged on
```
**[PASSED]**

#### Test with iPhone Bluetooth Off:
**TIME:** 03:06
I shoulld not be able to log in via bluetooth. The screen should turn black and I should still be able to log on with my password.
```
Tue Feb 11 03:06:42 2020: Device C0:1A:DA:7A:30:B7 NOT found
Tue Feb 11 03:06:42 2020: zaku logged on
```
**[PASSED]**


### Test with no Bluetooth on the Machine
I am curious what would happen if I turn off bluetooth.

First I will run the simple bluetooth scanner with bluetooth adapter turned off. I just want to make sure there's no seg fault.
```
$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 1C:36:BB:23:B5:CB  ACL MTU: 1021:8  SCO MTU: 64:1
	DOWN 
	RX bytes:870 acl:0 sco:0 events:35 errors:0
	TX bytes:383 acl:0 sco:0 commands:35 errors:0

#Bluetooth is turned off

$ gcc simple.c -l bluetooth
$ ./a.out 
opening socket: No such device
```

**Test GDM Login Result:** I was locked out of my laptop. GDM Login is refusing to accept any input of my password and eventually refuses to let me have the option to type in my password.

**Solution:** There are a few remediations I suggested but the answer Furkan Alaca, the professor overseeing our project, was the best solution. On my mac laptop, I need to press in `crtl + alt + fn + f3` to switch to terminal mode in which I can use to log in. Since I have not overwritten the PAM modules for login, I was able to successfully log in.

**Problem:**
Looking at the logs `/var/log/syslog`, I see the following:
```
Feb 11 16:48:59 zaku-laptop systemd[1]: Starting Fingerprint Authentication Daemon...
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1798]: opening socket: No such device
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1798]: Try to find device: C0:1A:DA:7A:30:B7
Feb 11 16:48:59 zaku-laptop gdm3: Freeing conversation 'gdm-password' with active job
Feb 11 16:48:59 zaku-laptop dbus-daemon[1052]: [system] Successfully activated service 'net.reactivated.Fprint'
Feb 11 16:48:59 zaku-laptop systemd[1]: Started Fingerprint Authentication Daemon.
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1812]: opening socket: No such device
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1812]: Try to find device: C0:1A:DA:7A:30:B7
Feb 11 16:48:59 zaku-laptop gdm3: Freeing conversation 'gdm-password' with active job
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1816]: opening socket: No such device
Feb 11 16:48:59 zaku-laptop gdm-session-worker[1816]: Try to find device: C0:1A:DA:7A:30:B7
Feb 11 16:48:59 zaku-laptop gdm3: Freeing conversation 'gdm-password' with active job
Feb 11 16:49:00 zaku-laptop gdm-session-worker[1820]: opening socket: No such device
```

As expected, there is nothing out of ordinary. However, `pam_proxy.so` is supposed to also prompt the user for the password if it fails to locate the device which it did not. The program must be terminating early.

Looking at the source code under `pam/src/pam_proxy.c`, the problem code is:
```
if (dev_id < 0 || sock < 0) {
    perror("opening socket");
    exit(1);
}

```

We should not be terminating the program but rather exiting the function (`return`).

**TEST: Does the fix work**
* Check bluetooth is disabled
```
$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 1C:36:BB:23:B5:CB  ACL MTU: 1021:8  SCO MTU: 64:1
	DOWN 
	RX bytes:870 acl:0 sco:0 events:35 errors:0
	TX bytes:383 acl:0 sco:0 commands:35 errors:0
```

Was able to log in via password without bluetooth enabled with no issue.

Output in `/var/log/syslog`
```
Feb 11 18:59:15 zaku-laptop systemd[1]: Started Fingerprint Authentication Daemon.
Feb 11 18:59:15 zaku-laptop gdm-session-worker[4501]: opening socket: No such device
Feb 11 18:59:19 zaku-laptop gdm-session-worker[4501]: Try to find device: C0:1A:DA:7A:30:B7
Feb 11 18:59:19 zaku-laptop gdm-session-worker[4501]: Welcome zaku
Feb 11 18:59:19 zaku-laptop gdm-session-worker[4501]: This is a simple PAM says Pikachu
```
**[PASSED]**

## Testing Android App

As Areeb and the others write the server to read input from the Android application, I will be verifying the Android application does work with an already implemented bluetooth server that I can find on the internet. I will be looking at a [Python Bluetooth Server](https://github.com/simondlevy/PythonBluetoothServer) implementation written by Simon D. Levy.

I'll be using my Fire Tablet to test the Android Application
**Fire Tablet Bluetooth Mac:** F0:81:73:92:2E:C2

The `ON` button from the Android Application sends `a`
The `OFF` button from the Android Application sends `b`

```
Waiting for connection on RFCOMM channel 1
Accepted connection from ('F0:81:73:92:2E:C2', 1)
pika received data
a
pika received data
b
pika received data
a
pika received data
b
pika received data
b
pika received data
a
```

The Android Application works. Now it is the matter of writing a server in C.

## REFLECTION
* The login via bluetooth takes a while to login.
