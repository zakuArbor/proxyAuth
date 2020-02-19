# Sprint 2

## Accomplishments
* PAM that logs in via bluetooth after detecting the device
* Android App that sends a simple message via bluetooth to the desktop
* C program that reads a single message sent by the Android app via bluetooth

## Participants Contribution:
* Ju Hong Kim: 
    * wrote C program that reads a single message sent by the Android App
    * wrote the PAM to login after detecting the device
    * tested and fixed a minor bug on the Android communication with the C program

## Goals for Next Sprint (Will be subject to change)
**PAM:**
* Remove the lag associated with the PAM when entering the password (i.e. when logging into the machine via password, there's a signficiant delay)
* Improve the speed of login to the system via bluetooth
* Store Trusted Bluetooth Device to a file (we are not going to check how a device is trusted, that’s for another sprint)
* Improve the C bluetooth server to be able to read and send messages to the Android device.
* Modularize PAM to allow different levels of authentication (i.e. In the near future, we will allow users to choose different levels of authentication, i.e. zero knowledge, one factor, two factor, etc)
* Create a background script to keep reading input from Android App + implement deauthentication 


**Android:**

## To Improve in the Next Sprint

## PROBLEMS
* Bluetooth **[BLOCKING]**: Memebers need an external bluetooth adapter to be able to work on their assigned tasks.
    * we cannot get our VM connected with our built in bluetooth adapter and we believe that we need an external bluetooth adapter to have our VM detect our bluetooth adapter.

## Extra Information
* Read `doc/week02/kim_log_2.md` for details about what Kim did for the week
