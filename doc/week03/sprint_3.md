# Sprint 2

## Accomplishments


## Participants Contribution:
* Ju Hong Kim: 
    
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
  
* Areeb Siddiqui


## Goals for Next Sprint (Will be subject to change)
**PAM:**


**Android:**

## To Improve in the Next Sprint


## PROBLEMS

## Extra Information
* Read `doc/week02/kim_log_2.md` for details about what Kim did for the week
