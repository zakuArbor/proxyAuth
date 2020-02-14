# Week 01 Log
#### Author: Arslan Ahmed Qamar
#### Date: Feb. 07 2019

---

## OBJECTIVES
* Run the simple pam module
* Peform testing 

## SOURCES USED 
* https://www.tecmint.com/configure-pam-in-centos-ubuntu-linux/
* https://people.csail.mit.edu/albert/bluez-intro/
* https://github.com/beatgammit/simple-pam
## TODO
* Simple Pam Module was cloned provided by the TA. Understand the simple pam module, fix the issues, dependencies and compile the program. 
* Simple Usage would be implemented and PAM be replaced by sudo

---

## PAM Module SETUP 

### GOALS
* Install the UBUNTU on virtual machine
* Install packages/tools that we be using e.g. ssh, git, gcc and others

### STEPS
**Install SSH to login from your computer's terminal due to display size problem** `sudo apt update sudo apt` `install openssh-server`

**Check IP address for connection** `ip a` or through your browser
**Install Git to clone the repo** `sudo apt install git`
**Firewall Configuration to allow SSH connection**
```
sudo ufw allow ssh
sudo ufw enable
sudo ufw status
```

 
--- 
## PAM Module
* Cloned the simple-pam repo onto our virtual machine
* Follow the instructions on the readme.md file to compile and simply running it. There will be some issues due to our environment setup and how we fix it, that would be discussed in ISSUES.

## GOALS
* Able to compile and run the simple pam module
* Extend on it, make some changes and show some simple usage

## ISSUES 
* Build the PAM module. Run these commands in order. 
`gcc -fPIC -fno-stack-protector -c src/mypam.c`
`sudo ld -x --shared -o /lib/security/mypam.so mypam.o`

**Issue 01**
* Following error would be prompted
```
src/mypam.c:4:10: fatal error: security/pam_appl.h: No such file or directory
    4 | #include <security/pam_appl.h>
      |          ^~~~~~~~~~~~~~~~~~~~~
```
**Solution:**
Install the missing libraries 
```
sudo apt-get install libpam0g-dev
``` 

**Issue 02:**
* The directory `lib/security/` does not exist.
```
$ sudo ld -x --shared -o /lib/security/mypam.so mypam.o
ld: cannot open output file /lib/security/mypam.so: No such file or directory
```
**Solution:** 
cd to the lib directory and create the security directory
```
mkdir /lib/security/
```

* Build Test. Run one of these commands 
'g++ -o pam_test src/test.c -lpam -lpam_misc'  (will extend it to C++) OR  
'gcc -o pam_test src/test.c -lpam -lpam_misc'  (will be using this)
* No issue were found with this command since gcc was installed before and g++ won't be using it. 











