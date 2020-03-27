# Sprint 6

## Date: March 13 - March 20

## Accomplishments
* **[Issue 13]** Test and Fix Bugs with Bluetooth Sanitization when reading from Trusted Config File
* **[Issue 19]** Redirect and notify user that it failed to connect to a server
* **[Issue 21]** Once user disconnects on the app, redirect user to main page
* **[Issue 22]** Execute Deauthentication server once user logs on
* **[Issue 27]** rfcomm-server: Memory Leaks
## Participants Contribution:
* Ju Hong Kim:
    * **[Issue 13]** *Test and Fix Bugs with Bluetooth Sanitization when reading from Trusted Config File* - Accepted other special characters instead of a colon in the Bluetooth Address
    * **[Issue 19]** *Redirect and notify user that it failed to connect to a server* - used to pretend that it is connected with the server when there's no connection
    * **[Issue 21]** *Once user disconnects on the app, redirect user to main page* - previously it was not intuitive what was happening. Once you disconnected, you are still in the same page and there would be no indication that you are disconnected from the server other than that the server no longer receives messages.
    * **[Issue 22]** *Execute Deauthentication server once user logs on*
    * **[Issue 27]** rfcomm-server: Memory Leaks
* Daniel Wang: 
    * Testing MiTM attack tool [Gattacker](https://github.com/securing/gattacker)
* Arslan Qamar
    
* Anurag Bist:
    * **[Issue 16]** *Worked on issue 16 which is the receive message functionality of the android app*
* Sean Coutinho
  
* Areeb Siddiqui:

## Goals for Next Sprint (Will be subject to change)

**Android:**
* Android app's menu toolbar
* Helper function used for handling messages received from C server
* Testing and possible more implementation is still required for Background services
    
 **PAM:**
* Issue #29: Allow connection to deauthentication server on trusted device only
* Issue #32: Terminate Deauthentication server if the user manually locks their system
* Bandwidth and latency checks in the PAM module and authentication service
    
 **Security Concerns:**
 * Security protocol ECDH + authentic encryption using AES 256 GCM partially.
 * Different UberTooth mode including repeater needs to be tested including testing MiTM attack tool [Gattacker]
 
## To Improve in the Next Sprint
 
## PROBLEMS
   
