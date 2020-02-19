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

