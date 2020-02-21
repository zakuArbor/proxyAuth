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


# ACKNOWLEDGEMENTS
* [Simple PAM](https://github.com/beatgammit/simple-pam) - Helped us learn how to work with PAM
* [Albert Huang](https://people.csail.mit.edu/albert/bluez-intro/) for his documentation on working with Bluetooth on Linux. Documentation on Bluetooth is hard to find so we really appreciate Albert's work.
