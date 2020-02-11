Usage
=====

This is the bluetooth module of our PAM application, which will use BlueZ - part of the official linux stack. http://www.bluez.org/
simple.c - scans and detects nearby bluetooth devices (buggy with my current bluetooth adapter) and returns their MAC address 
Compile with 
gcc -o simple simple.c -lbluetooth



Resources
=========

https://ukbaz.github.io/howto/beacon_scan_cmd_line.html
https://people.csail.mit.edu/albert/bluez-intro/c404.html

