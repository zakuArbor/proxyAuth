# Sprint 4

## Date: Feb 28 - March 06

## Accomplishments


## Participants Contribution:
* Ju Hong Kim: 
    * Allow login if device is trusted and paired with the laptop (Resolves issue #5)
    * Worked with Daniel Wang on issue #18 #20 on how to approach the problem
* Daniel Wang: 
    * Fixed bugs with auto-lockout, added 10s timer for auto lock-out.
    * Made file descriptors in recv loop unblocking

* Arslan Qamar
    * Worked on closing the android socket, once the connection with server is severed and Android background services, mainly implementing the Intent services Issue #08.
    * Fixing other minor bugs within the app. 
   
* Anurag Bist
   * Implemented Android app's ability to receive messages from C server
   * Worked on Issue #21

* Sean Coutinho
  
* Areeb Siddiqui
   * Researched encryption algorithms, key exchange algorithms, and security standards to apply to our data transfer
   * Started implementing addition of SSL to our socket (w/ RSA encryption), using the C OpenSSL library      
      * Originally planned to use Diffie-Hellman for key exchange and AES for encryption, maybe revert to this if necessary


## Goals for Next Sprint (Will be subject to change)
**Android:**
   * Allow a Bluetooth Connection to be maintained in the background when app is running in the background and/or the phone is locked.
   * Start creating a message list for our Bluetooth connection. 
   * Update UI to allow the user to select prefered security level during setup.
   * Implement continuous transmission of messages to Server

 **PAM:**
   * Create a secure channel between the Linux Pam (server side) & Android app (client side) and send challenge response 
   * Rewrite Makefile and PAM to be more modular (mainly to allow testing and sanitation testing easier to perform)
   * Implement the continuous transmission of messages to Android
     
 **Security Concerns:**
   * Will be experimenting the man in the middle attacks
   * Other security issues with the current implementation.

## To Improve in the Next Sprint
   * Communicate effecctively


## PROBLEMS
   * 
