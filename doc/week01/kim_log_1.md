# WEEKLY LOG 1
#### Author: Ju Hong Kim
#### Date: 02/06/2020

---

## OBJECTIVES
* Replace sudo with a simple PAM
* Create a C Program that can detect bluetooth devices

## EXTERNAL TOOLS AND PURCHASES
* PAM Mastery (book) - Arrival Date on Friday so I will be unable to read and reference the book for this week's update
* Yubikey: A two authenticator device to play around with PAM. I think setting up a Yubikey onto my personal laptop will let me know how PAM works and the experience for a user to configure PAM. We want setup and integration to be painless so seeing how a similar product works will let us guage how difficult the experience should be.

## TODO
* Clone the repo [Simple PAM](https://github.com/beatgammit/simple-pam) and see what dependencies and set up are needed to get the PAM working
* Configure PAM to replace sudo
    * **NOTE:** Ensure to open a root shell before attempting changing `/etc/pam.d/sudo` so I can revert my changes and gain access to root shell if I mess up
* If replacing sudo works, create a logging system for debug purposes
* Research a bluetooth library and try to implement a bluetooth scanning program in C

---

## SETUP
#### Commentary
* I tried playing around with PAM on my personal laptop (Running Ubuntu 19.10) and I encountered a problem where I cannot install programs on the Ubuntu Store because of the PAM config changes to `common-password`
* Therefore, all development on PAM will be done on a VM and to test bluetooth capabilities, we will be running on a Raspberry Pi
* We will be working on Ubuntu 18.04 LTS

### GOALS
* install Ubuntu onto a VM
* Have git and ssh installed

### STEPS
#### Install ssh-server
**Install SSH SERVER**
`sudo apt install openssh-server`

**Configure Firewall**
```
sudo ufw allow ssh
sudo ufw enable
sudo ufw status
```

**Note:** To get your ip address: `ip a`

**Installing Git**
`sudo apt install git`

---

## Working With PAM
* We will be cloning a Simple PAM for the following repo: [https://github.com/beatgammit/simple-pam](https://github.com/beatgammit/simple-pam) onto our VM
* there will be no modification to the source code other than the scripts. We will simply follow the instructions as written in `README.md`. The author has done a great job explaining how to compile and get the PAM working.
* The errors encountered in this step is merely due to our setup (i.e. missing dependencies or directories)

### GOALS
* Compile and have a simple PAM working

### ISSUES

**Issue 1:**
* When compiling, you may see the following error:
```
src/mypam.c:4:10: fatal error: security/pam_appl.h: No such file or directory
    4 | #include <security/pam_appl.h>
      |          ^~~~~~~~~~~~~~~~~~~~~
```
**Solution:**
The solution is to install the missing libraries:
```
sudo apt-get install libpam0g-dev
``` 

**Issue #2:**
When compiling, the directory `lib/security/` does not exist.
```
$ sudo ld -x --shared -o /lib/security/mypam.so mypam.o
ld: cannot open output file /lib/security/mypam.so: No such file or directory
```
Simply create the directory
```
mkdir /lib/security/
```

**Issue #3:**
When running `buildTest.sh`, you may get the following error:
```
Command 'g++' not found, but can be installed with:

apt install g++
```
**Solution:**
Replace the compiler to use gcc. We will not be using C++ for this project.


## Get the simple PAM to replace current authentication scheme for sudo
Before doing any work, if you follow the guide, you should see a print message on the terminal every time an authentication occurs whether that be `ssh`, `sudo`, and etc. This is because we modified the file `/etc/pam.d/common-auth`.

Here's an example:
```
zaku@zaku-laptop:~$ ssh 138.51.236.200
zaku@138.51.236.200's password: 
Welcome zaku
This is a simple PAM says Pikachu
Acct mgmt
```

We are going to change this behavior by having only sudo print those output. Once this is accomplished, we shall log the activity to a file and see if we can augment the current Desktop GUI Login to log our entry.

1. Comment out the changes made on `/etc/pam.d/common-auth`
2. Edit the file `/etc/pam.d/sudo` with the following contents:
```
#start of simple pam
auth sufficient mypam.so
account sufficient mypam.so
```

### TESTING
**Test 1: When ssh, our PAM output should not be displayed**
```
ssh 138.51.236.200
zaku@138.51.236.200's password: 
Welcome to Ubuntu 18.04.4 LTS (GNU/Linux 5.3.0-28-generic x86_64)
```
**[PASSED]**

**Test 2: Our PAM output should be displayed when we authenticate sudo**
```
zaku@zaku-VirtualBox:~$ sudo echo
Welcome zaku
This is a simple PAM says Pikachu
[sudo] password for zaku: 
Acct mgmt
```
**[PASSED]**

### BREAKDOWN
* I am going to explain what is happening. PAM, Pluggable Authentication Module, is an authentication scheme or framework that allows programs to authenticate users without knowing the underlying authentication scheme. This allows for a flexible authentication without the need of changing an entire program each time the authentication scheme changes.

* `.so` - these are shared object files that we generate after compiling our PAM module. Essentially it is a compiled library that is linked during runtime so we do not need to recompile our main program (i.e. ssh or sudo) whenever we make changes to our PAM.

**GCC OPTIONS**
---
**COMMAND:** `gcc -fPIC -fno-stack-protector -c src/mypam.c`

* `-fPIC`: According to the MAN page:
```
           If supported for the target machine, emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of
           the global offset table.  This option makes a difference on AArch64, m68k, PowerPC and SPARC.

           Position-independent code requires special support, and therefore works only on certain machines.

           When this flag is set, the macros "__pic__" and "__PIC__" are defined to 2.
```
    * essentially how I see it is that it generates a dynamic library (i.e. a library that can be dyanmically linked)
    * `The -fpic option converts absolute addresses to relative addresses, which allows for different processes to load the library at different virtual addresses and share memory.` - Nick Desaulniers


* `-fno-stack-protector`: This flag disables stack protection.
```
NOTE: In Ubuntu 14.10 and later versions, -fstack-protector-strong is enabled by default for C, C++, ObjC, ObjC++, if none of
           -fno-stack-protector, -nostdlib, nor -ffreestanding are found.
```
    * still unclear as to why though: [https://stackoverflow.com/questions/10712972/what-is-the-use-of-fno-stack-protector](https://stackoverflow.com/questions/10712972/what-is-the-use-of-fno-stack-protector)

* `-c`: to compile but not link. For our purpose, we do want to create a library that is dynmically linked or at least that is how I understand it.
```
    Compile or assemble the source files, but do not link. The linking stage simply is not done. The ultimate output is in the form of an object file for each source file. 
```

**ld**
---
**Command:** `sudo ld -x --shared -o /lib/security/mypam.so mypam.o`

* `ld is a linker. For those not familiar with how Compilers work (I am not knowledgeable on this topic either. It's a shame I will not be taking a Compiler course in my undergraduate studies), is a process sometime during the build process where you link a few object files into one executable.

* `--shared`: Create a shared library
* `-x`: discard all local symbols. Not sure why we want to do so unless symbols are attached to specific addresses or location in code. I'll have to read up on compilers.

**Code:**
**PAM_EXTERN:** `#define PAM_EXTERN static`

**pam_get_user:** 
```
PAM_GET_USER(3)                                                  Linux-PAM Manual                                                  PAM_GET_USER(3)

NAME
       pam_get_user - get user name

SYNOPSIS
       #include <security/pam_modules.h>

       int pam_get_user(const pam_handle_t *pamh, const char **user, const char *prompt);
```
* when `pam_start()` gets called, it creates a handle. This handle should be used to get data directly but use `pam_get_item` and `pam_set_item` instead, at least according to the man pages from `pam_start()`

According to logical flow of how PAM works, our application will call `pam_authenticate()` which will route to the a PAM method `pam_sm_authnenticate`

### LOGGING
* pam_warn.so - should use to send a syslog message to auth.notice

### Modification to PAM to allow the user pikachu login automatically as proof that we replace sudo
---
```
zaku@zaku-VirtualBox:~/simple-pam$ git diff
diff --git a/src/mypam.c b/src/mypam.c
index 31b3247..e6d5985 100644
--- a/src/mypam.c
+++ b/src/mypam.c
@@ -22,6 +22,12 @@ PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, cons
        retval = pam_get_user(pamh, &pUsername, "Username: ");
 
        printf("Welcome %s\n", pUsername);
+    printf("This is a simple PAM says Pikachu\n");
+    
+    if (!strcmp(pUsername, "pikachu")) {
+        printf("Pikachu has backdoor\n");
+        return PAM_SUCCESS;
+    }
 
        if (retval != PAM_SUCCESS) {
                return retval;
```
**TEST:**
Check if user exists
```
zaku@zaku-VirtualBox:~$ cat /etc/passwd | grep pikachu
pikachu:x:1001:1001:Pikachu,,,:/home/pikachu:/bin/bash
```

Add user to sudoer group:
```
usermod -aG sudo pikachu
```

Now try to run sudo:
```
zaku@zaku-VirtualBox:~$ whoami
zaku
zaku@zaku-VirtualBox:~$ su pikachu
Password: 
pikachu@zaku-VirtualBox:/home/zaku$ whoami
pikachu
pikachu@zaku-VirtualBox:/home/zaku$ sudo echo "This is a test on echoing as sudo"
This is a test on echoing as sudo
```
**[success]** - Does not ask for password unlike for other users

## Look into how log in for Desktop GUI works
* Ubuntu uses GNOME by default. Looking into Arch wiki, GDM is the graphical GUI that handles login

Let's enable gdm debug:
On `/etc/gdm3/custom.conf`: Uncomment `Enable=true` under `[debug]`

Let's look at what config files there are for gdm:
```
# ls /etc/pam.d | grep gdm
gdm-autologin
gdm-fingerprint
gdm-launch-environment
gdm-password

```

Parsing through the logs after logging onto the VM via GUI, the only interesting information can be found in `/var/log/syslog`

```
    247 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: queuing setup for user: zaku (null)
    248 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: attempting to change state to SETUP_COMPLETE
    249 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: initializing PAM; service=gdm-password username=zaku seat=seat0
    250 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'XDG_SEAT=seat0'
    251 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: state SETUP_COMPLETE
    252 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: attempting to change state to AUTHENTICATED
    253 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: authenticating user zaku
    254 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: 1 new messages received from PAM
    255 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: username is 'zaku'
    256 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: old-username='zaku' new-username='zaku'
    257 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: GdmSessionWorker: received pam message of type 1 with payload 'Password: '
    258 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: trying to get updated username
    259 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: PAM conversation returning 0: Success
    260 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: state AUTHENTICATED
    261 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: trying to get updated username
    262 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: username is 'zaku'
    263 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: old-username='zaku' new-username='zaku'
    264 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: attempting to change state to AUTHORIZED
    265 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: determining if authenticated user (password required:0) is authorized to sess        ion
    266 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: state AUTHORIZED
    267 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: attempting to change state to ACCREDITED
    268 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'LOGNAME=zaku'
    269 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'USER=zaku'
    270 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'USERNAME=zaku'
    271 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'HOME=/home/zaku'
    272 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'PWD=/home/zaku'
    273 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'SHELL=/bin/bash'
    274 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: Set PAM environment variable: 'PATH=/usr/local/bin:/usr/bin:/bin:/usr/games'
    275 Feb  7 00:12:00 zaku-VirtualBox gdm-password]: GdmSessionWorker: state ACCREDITED
```

Seems like it is `GDM-Password` that is doing the work. Interestingly enough, from the logs (not displayed in this file), I am seeing `gdm-password` iterates through the list of users and tracks new users.
```
    150 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: AccountsService: ActUserManager: user pikachu is now loaded
    151 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: AccountsService: ActUserManager: user pikachu was not yet known, adding it
    152 Feb  7 00:11:58 zaku-VirtualBox gdm-password]: AccountsService: ActUserManager: tracking user 'pikachu'
```

Before we attempt to modify PAM config for `gdm-password`, let's create a logging system since our current PAM only prints to stdout. We need to write to a file to make it more obvious that our PAM is the one replacing the current authentication scheme.

```
+    FILE *fp = NULL;
+    if (!(fp = fopen("/var/log/simple-pam.log", "a"))) {
+        perror("Failed to open file\n");
+    }
+    else {
+        time_t raw_time;
+        struct tm *info_time;
+        time(&raw_time);
+        info_time = localtime(&raw_time);                
+        char curr_time[50];
+        int time_len = strlen(asctime(info_time));
+        strncpy(curr_time, asctime(info_time), time_len);
+        curr_time[(time_len - 1)] = '\0'; //asctime appends a \n to the string
+
+        char status[15];
+        if (retval == PAM_SUCCESS) {
+            strncpy(status, "Successfully", 12);
+            status[12] = '\0';
+        }
+        else {
+            strncpy(status, "unsuccessfully", 14);
+            status[14] = '\0';
+        }
+
+        fprintf(fp, "%s: %s logged on %s\n", curr_time, pUsername, status);
+        fclose(fp);
+    }
+ 
```

**Test:**

```
zaku@zaku-VirtualBox:~$ sudo echo "Test if PAM write to file"
Welcome zaku
This is a simple PAM says Pikachu
[sudo] password for zaku: 
Test if PAM write to file
zaku@zaku-VirtualBox:~$ cat /var/log/simple-pam.log 
Fri Feb  7 01:52:08 2020: zaku logged on Successfully
```
**[SUCCESS]**

The next step is to replace `gdm-password` to use our simple PAM.

On `/etc/pam.d/gdm-password`, add the following:
```
auth    sufficient  simple_pam.so
auth    required    pam_warn.so
```

**Test:** Log onto the VM via GUI
Current time: `02:04`

```
cat /var/log/simple-pam.log 
Fri Feb  7 02:04:11 2020: zaku logged on Successfully
```
**[SUCCESS]**
`simple-pam.log` did not get updated.

## Reflection
Not much was accomplished from the team meetings. Those of us doing PAM that came have very limited knowledge on the subject, systems programming or the general Linux Architecture in general. I believe it is more productive to attempt on the tasks ourselves before meeting up, at least for the beginning to get each of us on top of things.

```
"To feel affection for people even when they make mistakes is uniquely human. You can do it, if you simply recognize: that they're human too, that they act out of ignorance, against their will,  and that you'll both be dead before long. And, above all, that they haven't really hurt you. They haven't diminished your ability to choose." - Marcus Aurelius
```
Special Thanks to Riddle from The White Van for suggesting this

## BLUETOOTH

### SETUP
* install the bluetooth library: (need to check if it's part of the kernel)
`sudo apt install bluez`

## Resources

http://nickdesaulniers.github.io/blog/2016/11/20/static-and-dynamic-libraries/

https://github.com/beatgammit/simple-pam

https://www.sans.org/reading-room/whitepapers/unix/pam-its-non-stick-cooking-spray-329

https://wiki.archlinux.org/index.php/GDM
