# proxyAuth
PAMxAndroid via Bluetooth proximity based login

## PAM SETUP
**Requirements:**
* GDM (GNOME Display Manager)
* gcc compiler

**SETUP:**
Instructions are for Debian based systems that have GNOME.

1. Need to install all the dependencies for PAM and the deauthentication server:
```
cd pam
sudo make install
```

It'll install various dependencies such as:
```
#PAM libaries
sudo apt-get install libpam0g-dev
sudo apt-get install libglib2.0-dev

#Install bluez
sudo apt install bluez
sudo apt-get install libbluetooth-dev
```

3. Create the directory `/lib/security` if it does not exist
`mkdir /lib/security/`
 
4. Clone the repo:
`git clone https://github.com/Sxx125/proxyAuth.git`

5. Compile:
Assuming you are in the pam directory of the project (i.e. `$(INSTALLATION FOLDER)/proxyAuth/pam`)
```
sudo make
```

6. Change PAM GDM Password Configuration:
```
sudo vi /etc/pam.d/gdm-password
```
and add the following near or at the top of the file:
```
#proxyAuth
auth sufficient pam_proxy.so
auth required pam_warn.so
```

**To add Trusted Device**
1. Create the directory if it does not exist: `/etc/proxy_auth/`
    * This is where all the user's MAC Addresses are stored
2. Create a file that lists the user's trusted bluetooth devices: `echo <bluetooth_address> >> /etc/proxy_auth/$USER`

## Running Bluetooth Server

**Requirements:**

* Follow the instructions from [here](https://raspberrypi.stackexchange.com/questions/41776/failed-to-connect-to-sdp-server-on-ffffff000000-no-such-file-or-directory). I'll list the steps though:

1. `sudo vi /etc/systemd/system/dbus-org.bluez.service`

2. Change `ExecStart=/usr/lib/bluetooth/bluetoothd` to `ExecStart=/usr/lib/bluetooth/bluetoothd --compat`

3. Add the following line to the file: `ExecStartPost=/bin/chmod 777 /var/run/sdp`

The permissions of /var/run/sdp changes on each reboot which is why we add the following line to update the permission each time we boot up.

This is to avoid segmentation fault when running sdp tools. According to the link above, sdptool is broken in Bluez 5. 

3. Restart bluetooth: 
```
sudo systemctl daemon-reload
sudo systemctl restart bluetooth
```

**Running:**
* Compile the server: `gcc -o server rfcomm-server.c -lbluetooth`

# ACKNOWLEDGEMENTS
* [Simple PAM](https://github.com/beatgammit/simple-pam) - Helped us learn how to work with PAM
* [Albert Huang](https://people.csail.mit.edu/albert/bluez-intro/) for his documentation on working with Bluetooth on Linux. Documentation on Bluetooth is hard to find so we really appreciate Albert's work.
* [Ryan Scott](https://github.com/RyanGlScott/BluetoothTest) I have based the starter code of the bluetooth server from his work.
