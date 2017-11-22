# PiAIZU
This is the build system to compile a SD card image of the Raspian.
This image will be preconfigured already to be ready to run the PiAIZU setup.
The PiAIZU setup consists of the following:
 - RbPi (Raspberry Pi), [3 or Zero]
 - PCB sheild that connects to the RbPi
 - PCB that connects to the LMX-001 display
 - Cable that connects the two PCBs

 [![N|Solid](https://www.raspberrypi.org/app/uploads/2012/02/Raspian_SD-150x150.png)](https://www.raspberrypi.org/downloads/raspbian/)

# Software

 The software needed that is added into the rootfs of the Raspbian is the following:
  - LMX I2C initalisation scripts
  - Demo (TBD)
  - Brightness
  - I2C Tools (added std package)

# Configuration
The PiAIZU related files are all being set-up in the stage2 directory.

# Dependencies
The same as for the original plus doxygen with doxygen.
```
apt-get install quilt parted realpath qemu-user-static debootstrap zerofree pxz zip dosfstools bsdtar libcap2-bin grep rsync doxygen graphviz qemu-user-binfmt
```
The orignal RPi-Distro/pi-gen repository can be found at github.
```
https://github.com/RPi-Distro/pi-gen
```
Current tag is:
 	**2017-04-10-raspbian-jessie**

# Building

Once all the prerequisite is in place, you start a build by executing the
build.sh script in the root of this project. You must be sudo to do it.
```
sudo -s
./build.sh
```
When its done you will find your binaries in the deploy/ folder.

# Flashing

You need have and microSD card, insert it into your computer and find out what
device name it has.
```
sudo -s
unzip -p image_2017-06-29-PiAIZU-lite.zip | dd of=/dev/sd[X] bs=20M && sync
```
When this command is done, just take the card out and put it in the Raspberry.
Make sure you have the PiAIZU hardware attached.

When the Raspberry boots, you will get more instructions inte screen of the
LMX-001.

### Encountered issues
At some points it has been seen that the qemu-arm hasn't been properly installed.
if debootstart fails due to something similiar to:
```
chroot: failed to run command ‘/bin/bash’: Exec format error
```
try to remove and reinstall:
```
apt-get remove qemu-user-static
apt-get install qemu-user-static
```

