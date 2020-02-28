# Sprint 3

## Date: Feb 14 - Feb 28

## Accomplishments
* Authenticate trusted devices only on PAM
* Authenticate trusted devices only on Android

## Participants Contribution:
* Ju Hong Kim: 
    * Restructure PAM and ensure no memory leaks
    * Authenticate trusted devices on PAM
* Daniel Wang: 

* Arslan Qamar 

* Anurag Bist

    ## 1. Worked on bluetooth adaptor connection

    ## 2. Researched the use of bluetooth in the background (for the app)

    ## Bluetooth adaptor

      Setup
      - Followed instructions on bluetooth adaptor packaging 
      - Installed drivers 

      VirtualBox setup
      - Select settings (for the virtual machine)
      - In 'USB' tab, select USB controller (i.e. 2.0 or 3.0) 
      - Then add the filter by selecting the bluetooth adapter (BCM920702 in this case)

      Challenges and Errors
      - Even after installing drivers for the bluetooth adaptor, it seems to have errors
      - The drivers for the bluetooth adaptor interfere with the laptop's built in bluetooth's adaptor
      - The adaptor did not appear inside the VirtualBox
      - Need to find a solution to this (because the adaptor worked with MAC OS)

    ## Bluetooth in the background

      START_STICKY flag
      - Service is restarted when the parent app terminates when using the START_STICKY flag

      App Life Cycle
      - There is no way to continue the bluetooth connection once the app is closed
      - All app processes have the same lifetime as the app itself

* Sean Coutinho
   * Read/Write a paired device's information in Android app internal storage. (Improved to read/write hashval instead)
   * Automatically connected to a paired device stored in memory.
  
* Areeb Siddiqui


## Goals for Next Sprint (Will be subject to change)
**PAM:**
* [Login if device is paired](https://github.com/Sxx125/proxyAuth/issues/5)
* [Toggle between password and bluetooth scan](https://github.com/Sxx125/proxyAuth/issues/10)

**Android:**

## To Improve in the Next Sprint
* Have members work earlier and communicate directly to memebers on their progress

## PROBLEMS
* Bluetooth setup on Virtual Machine

## Extra Information
* Read `doc/week03/kim_log_3.md` for details about what Kim did for the week
