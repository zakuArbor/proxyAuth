# WEEKLY LOG 3

**Author:** Ju Hong Kim
**Date:** 02/15/2020 - 02/28/2020

***

## SUMMARY OF TASKS
*  
*

## PROBLEMS

***

## ISSUE 10: As a user, I want to have no lag when trying to enter my password (not using bluetooth

**GOAL:** Ensure bluetooth component of the PAM does not interfere with the normal password login process

**Idea:** 
* Remove the need of checking password on our PAM. Why reinvent the wheel for login when we can use the default login process (i.e. pam_unix.so)


### Progress: 
* Change `/etc/pam.d/gdm-password` to use `pam_unix.so` before running our own PAM proxy.

```
#proxyAuth
password    [success=1 default=ignore]  pam_unix.so obscure sha512

auth sufficient pam_proxy.so
auth required pam_warn.so
```

**TEST:** Lock the computer and try to login. Bluetooth is currently enabled.
Proof that bluetooth is enabled.
```
zaku@zaku-laptop:/var/log$ hciconfig
hci0:	Type: Primary  Bus: USB
	BD Address: 1C:36:BB:23:B5:CB  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING PSCAN ISCAN 
	RX bytes:145083 acl:0 sco:0 events:5830 errors:0
	TX bytes:7962 acl:0 sco:0 commands:996 errors:0
```

**OBSERVATIONS:** The login page comes up without any lag and prompts our password no problem. There's no lag when entering the password anymore. In addition, there's no lag after entering the password either.

**[PASSED]**

**TEST:** Can I still login via bluetooth
Lock the computer and have the device bluetooth enabled.

**Result:**
```
Wed Feb 19 18:10:29 2020: Device F0:81:73:92:2E:C2 found
Wed Feb 19 18:10:29 2020: zaku logged on
```
**[PASSED]**

**Issue 10 Resolved**

***

## ISSUE 12: Login via Bluetooth only on selective bluetooth devices
**IDEA:**
* Login via bluetooth only if a trusted device is within range. Current implementation hard-codes a specific device for all users in the system
* Each user will have their own file that keeps track of what bluetooth device is trusted
* We will have a file for each user stored in `/etc/proxy-auth/`
	* the file will be owned by the root

**SECURITY ISSUES:**
* When user account gets deleted, the user's trusted device are not deleted. This would mean a new user with the same name will inherit the trusted device.

**PROBLEMS:**

***

**Problem 1:**
* When I used `stat` to check if the file is a directory or not, my PAM stopped working.
```
Feb 19 22:31:10 zaku-laptop gdm-password]: PAM unable to dlopen(pam_proxy.so): /lib/security/pam_proxy.so: undefined symbol: stat
Feb 19 22:31:10 zaku-laptop gdm-password]: PAM adding faulty module: pam_proxy.so
```

The error seems like a linking error since it cannot find the symbol stat. This would mean that it cannot link the library. 

**Solution:** add `-lc` option at the end of the linker to link `libc.a`

***

## TESTING:
I am rewriting the code a lot so I thought I should test each small part of the program.

### Testing 1: get_login_time

This is how the test looks like:
```
int main(int argc, char **argv)
{
    /*** LOGIN TIME ***/
    char curr_time[50];
    get_login_time(curr_time);
    /*******************/
    printf("Time is: %s\n", curr_time);
}
```

**Result 1:**
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ./a.out 
Time is: Thu Feb 20 13:30:32 2020   
```
**[PASSED]**

### Testing 2: check_or_creat_dir

Test setup:
```
int main(int argc, char **argv)
{
    FILE *fp = NULL;

    if (!(fp = fopen("/var/log/pam-proxy-auth.log", "a"))) {
        perror("Failed to open file");
    }
    char *trusted_log_dir = "/etc/proxy_auth/";  

    check_or_creat_dir(trusted_log_dir, fp);
}
```

* **Case 1: No permission and directory does not exist:**
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ls /etc/proxy_auth                                                                                                    
ls: cannot access '/etc/proxy_auth': No such file or directory      
$ ./a.out 
Failed to open file                                                                                                                                                  
: Permission denied                                                                                                                                                  
Cannot access /etc/proxy_auth/   
```
I should get an error since I do not have access to the log file. The directory `/etc/proxy_auth` does not exist so I should get cannot access file and also because I don't have the sufficient permissions either.
**[PASSED]**

* **Case 2: With Permission but Directory doesn't exist:**
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ls /etc/proxy_auth                                                                                                    
ls: cannot access '/etc/proxy_auth': No such file or directory      
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$

zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ sudo ./a.out                                                                                                           
[sudo] password for zaku: 
stat returned -1
returned: 0
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ls -l /etc/proxy_auth
total 0
```
**[PASSED]**

* **Case 3: With Permission and Directory Exists:**
We should expect `check_or_creat_dir` to return a 1 since the directory exists (refer to source code documentation for details)
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ sudo ./a.out                                                                                                
stat returned 0
returned: 1
```
**[PASSED]**

### TEST 3: get_trusted_dev_file

* Test setup:
```
int main(int argc, char **argv)
{
    const char *trusted_log_dir = "/etc/proxy_auth/";

    const char *username = "zaku";

    FILE *fp = get_trusted_dev_file((char *)trusted_log_dir, (char *)username, NULL);
    if (fp) {
        printf("file found");
        fclose(fp);
    }
}
```
* **Case 1:** File does not exist or we do not have permission to access
```
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ls /etc/proxy_auth/zaku
ls: cannot access '/etc/proxy_auth/zaku': No such file or directory
zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ 

zaku@zaku-laptop:~/Documents/csc490/proxyAuth$ ./a.out                                                                                                
There are no trusted device: No such file or directory
File: /etc/proxy_auth/zaku does not exist. There are no trusted device for the user: zaku
```

### TEST 4: 
How the test looks like:
```
int main(int argc, char **argv)
{
    const char *trusted_log_dir = "/etc/proxy_auth/";
    const char *username = "zaku";

    find_trusted_devices(NULL, trusted_log_dir, username); 
}
```

* **CASE 1:** Empty file
Proof the file is empty
```
$ wc -l /etc/proxy_auth/zaku                                                                                    
0 /etc/proxy_auth/zaku
```


**Result:**
```
$ ./a.out                                                                                                       
the number of trusted_devices are: 0
```
**Expected:** 0

**Received:** 0

**[PASSED]**

* **CASE 2:** One device
Proof there's only one file
```
$ cat /etc/proxy_auth/zaku                                                                                      
C0:1A:DA:7A:30:B7
```

**Result**
```
$ ./a.out                                                                                                
the number of trusted_devices are: 1
```
**Expected:** 1

**Got:** 1

**[PASSED]**

## Design Decisions


## SIDENOTES:
* Interesting to know where the macro `NAME_MAX` is located
```
zaku@zaku-laptop:/usr/src$ grep -r "#define NAME_MAX" *
linux-headers-5.3.0-29/include/uapi/linux/limits.h:#define NAME_MAX         255	/* # chars in a file name */
linux-headers-5.3.0-40/include/uapi/linux/limits.h:#define NAME_MAX         255	/* # chars in a file name */
```