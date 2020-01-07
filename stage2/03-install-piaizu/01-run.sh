#!/bin/bash -e

####################################################################
#
#           Installation of config and dts for LMX-001
#           Enable autologin
#           Enable SSHD
#           Enable i2c-dev
#
####################################################################

############ Config.txt ############################################
if [ ! -f ${ROOTFS_DIR}/boot/config_original.txt ]; then
    echo "No backup of original config found"
    cp -v ${ROOTFS_DIR}/boot/config.txt ${ROOTFS_DIR}/boot/config_original.txt
fi

# Install the config
install -m 644 files/config.txt ${ROOTFS_DIR}/boot/

############ Install dts ############################################
install -d ${ROOTFS_DIR}/home/pi/piaizu/dts
install -t ${ROOTFS_DIR}/home/pi/piaizu/dts files/dts/*

############ LMX-001 scripts ########################################
install -m 755 files/brightness.py ${ROOTFS_DIR}/usr/sbin
install -m 755 files/lmx001_init.py ${ROOTFS_DIR}/usr/sbin

cp -v files/rc.local ${ROOTFS_DIR}/etc/rc.local

############ Enable autologin #######################################
sed -i 's\ExecStart=-/sbin/agetty --noclear %I $TERM\ExecStart=-/sbin/agetty --autologin pi --noclear %I $TERM\g' ${ROOTFS_DIR}/lib/systemd/system/getty@.service

############ Enable SSHD ############################################
touch ${ROOTFS_DIR}/boot/ssh

############ Enable i2c-dev #########################################
cp -v files/modules ${ROOTFS_DIR}/etc/modules
