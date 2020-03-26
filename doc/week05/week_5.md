# Sprint 5

## Date: March 07 - March 13

## Accomplishments
   * Implement continuous transmission of messages to Server
   * Allow a Bluetooth Connection to be maintained in the background when app is running in the background
   * Rewrite Makefile and PAM to be more modular (mainly to allow testing and sanitation testing easier to perform)

## Participants Contribution:
* Ju Hong Kim:
    * Wrote an installation script in Perl to download Dependencies PAM has
    * **Modularize PAM:** Place code to header and updated Makefile to reflect those changes. 
        * Placing code to header allows me to be able to test PAM code on a program before deploying to PAM to avoid lock out and fix any memory leaks
    * Remove Bluetooth Pairing
    
* Daniel Wang: 
    * Initial Ubertooth One testing - sniffing advertisement packets. 
   
* Arslan Qamar
   * Worked on closing the android socket (Input/Output stream of the bluetooth socket)and background services
   * Testing needs to be done still for background service and recieve command (as the app is crashing upon calling recieve command)
   * Recieved the UberTooth hardware, read some documentation, partially setup it up. 
   * Tested modularize PAM
    
* Anurag Bist
  * Worked on troubleshooting of continuous communication between server and android app
 
* Sean Coutinho
  * Created test environment capable of running both the android and server code at the same time.
  * Resolved andriod crash on disconnect error 
  
* Areeb Siddiqui:
     * Continued SSL implementation on socket (switched to EVP_PKEY struct key gen, old functions deprecated).
  

## Goals for Next Sprint (Will be subject to change)
**Android:**
   * Start creating a message list for our Bluetooth connection. 
   * Update UI to allow the user to select prefered security level during setup.
   * Needs to test the Background services and recieve command 
    
 **PAM:**
   * Create a secure channel between the Linux Pam (server side) & Android app (client side) and send challenge response 
   * Implement the continuous transmission of messages to Android
     
 **Security Concerns:**
   * Will be experimenting the man in the middle attacks
   * Other security issues with the current implementation.
   * Will be setting up the UberTooth

## To Improve in the Next Sprint
   * Communicate & work effecctively
   * Must prioritize the issues that fulfill the sprint.


## PROBLEMS
   
