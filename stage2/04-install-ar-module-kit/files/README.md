# AR module kit
This is the build system to compile a SD card image of the Raspian.
This image will be preconfigured and ready to run the AR module kit.
The AR module kit consists of the following:
 - PCB sheild that connects to the Raspberry pi.
 - PCB that connects to the LMX-001 AR module.
 - Cable that connects the two PCBs.

 [![N|Solid](https://www.raspberrypi.org/app/uploads/2012/02/Raspian_SD-150x150.png)](https://www.raspberrypi.org/downloads/raspbian/)

# Software

 The software needed that is added into the rootfs of the Raspbian is the following:
  - LMX I2C initalisation scripts
  - Demo
  - Brightness
  - I2C Tools (added std package)

# Configuration
The AR module kit related files are all being set-up in the stage2 directory.

# Dependencies
The same as for the original plus doxygen with doxygen.
```
apt-get install quilt parted realpath qemu-user-static debootstrap zerofree pxz zip dosfstools bsdtar libcap2-bin grep rsync doxygen graphviz qemu-user-binfmt
```
The orignal RPi-Distro/pi-gen repository can be found at github.
```
https://github.com/RPi-Distro/pi-gen
```
Current tag:
    Upstream tag                        AR module kit tag
    **2019-09-26-raspbian-buster**      v.2.2.0
    **2017-04-10-raspbian-jessie**      v.2.1.0

# Building

Once all the prerequisite is in place, you can start a build by executing the
build.sh script in the root of this project. You must be sudo to do it.
To build inside a docker container you don't have to be sudo.

```
sudo -s
./build.sh

or

./build-docker.sh
```
When its done you will find your binaries in the deploy folder.

# Flashing

You need have a micro SD card. Insert the micro SD card into your computer and find out what
device name it has. In this example it is called /dev/sd[X] where X is a letter.

```
sudo -s
unzip -p image_2020-01-01-ar-module-kit.zip | dd of=/dev/sd[X] bs=20M status=progress conv=fsync
```
When this command is done, just take the card out and put it in the Raspberry.
Make sure you have the AR module kit hardware attached to the Raspberry Pi header correctly.

When the Raspberry boots, you will get more instructions inte screen of the
AR module screen.

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

