#!/bin/bash -e

####################################################################
#
#           Commands to run under chroot
#
####################################################################

############ Compile and install device tree #######################
make -C /home/pi/piaizu/dts/ install

############ Chmod the example folder ##############################
chmod -R a+rw /home/pi/lmx_example

############ Build and install the lmx example #####################
make -C /home/pi/lmx_example clean install


