# proxyAuth
PAMxAndroid via Bluetooth proximity based login

## PAM SETUP
**Requirements:**
* GDM (GNOME Display Manager)
* gcc compiler

**Dependencies:**
Instructions are for Debian based systems that have GNOME.
1. Need to install the PAM libraries
`sudo apt-get install libpam0g-dev`
`sudo apt-get install libglib2.0-dev`

2. Install `bluez`
```
sudo apt install bluez
sudo apt-get install libbluetooth-dev
```

3. Create the directory `/lib/security` if it does not exist
`mkdir /lib/security/`
 
4. Clone the repo:
`git clone https://github.com/Sxx125/proxyAuth.git`

5. Compile:
Assuming you are in the root of the project (i.e. `proxyAuth/`)
`cd pam`
`make`
 
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
## Running Bluetooth Server

**Requirements:**

* Follow the instructions from [here](https://raspberrypi.stackexchange.com/questions/41776/failed-to-connect-to-sdp-server-on-ffffff000000-no-such-file-or-directory). I'll list the steps though:

1. `sudo vi /etc/systemd/system/dbus-org.bluez.service`

2. Change `ExecStart=/usr/lib/bluetooth/bluetoothd` to `ExecStart=/usr/lib/bluetooth/bluetoothd --compat`

3. Restart bluetooth: 
```
sudo systemctl daemon-reload
sudo systemctl restart bluetooth
```

4. Change the permission of `/var/run/sdp`: `sudo chmod 777 /var/run/sdp`

This is to avoid segmentation fault when running sdp tools. According to the link above, sdptool is broken in Bluez 5

**Running:**
* Compile the server: `gcc -o server rfcomm-server.c -lbluetooth`

# ACKNOWLEDGEMENTS
* [Simple PAM](https://github.com/beatgammit/simple-pam) - Helped us learn how to work with PAM
* [Albert Huang](https://people.csail.mit.edu/albert/bluez-intro/) for his documentation on working with Bluetooth on Linux. Documentation on Bluetooth is hard to find so we really appreciate Albert's work.
