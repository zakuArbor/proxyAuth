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
* Daniel Wang: 
    * wrote C program that read messages from BlueTerm (3rd party android application)
    * Program opened and read from serial rfcomm channel with BlueTerm (allowing you to write to the channel in realtime)
    * Later extended by Kim to read input from our Android application
* Arslan Qamar 
    * Understand the basics of Bluetooth server, client for Pam module 
    * Worked on Bluetooth server 
* Anurag Bist
    * Worked with the Android App
    * Implemented the app's functionality to establish RFCOMM channel with paired bluetooth device 
    * Implemented the app's functionality to send data to the connected bluetooth device
* Sean Coutinho
   * Worked with the Android App
   * Created base listview interface for displaying bluetooth device data
   * Added initial bluetooth error handling and code for checking if bluetooth is enabled and identifying bluetooth paired devices
* Areeb Siddiqui
   * Wrote C Server application that establishes an rfcomm (bluetooth) socket connection with a client
   * Wrote mock C client (instead of the eventual Android app client) to test server
   * Blocked by inability to test bluetooth as both Linux VMs and Linux Subsystem on Windows don't support complete bluetooth capabilites.


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
* Remember a selected device and connect to it, rather than have the user re-select a device.
* Allow a Bluetooth connection to be maintained in the background when app is out of focus or the phone is locked.
* Update UI to allow the user to select prefered security level during setup.
* Start creating a message list for our Bluetooth connection. 


## PROBLEMS
* Bluetooth **[BLOCKING]**: Memebers need an external bluetooth adapter to be able to work on their assigned tasks.
    * we cannot get our VM connected with our built in bluetooth adapter and we believe that we need an external bluetooth adapter to have our VM detect our bluetooth adapter.

## Extra Information
* Read `doc/week02/kim_log_2.md` for details about what Kim did for the week
