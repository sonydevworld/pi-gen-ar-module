#!/bin/bash -e

####################################################################
#
#           Configuration of hostapd dnsmasq
#           Fix network config
#
####################################################################

############ Install lighttpd, host and dnsmasq config #######################
install -m 644 files/lighttpd.conf ${ROOTFS_DIR}/etc/lighttpd/
install -m 644 files/hostapd.conf ${ROOTFS_DIR}/etc/hostapd
install -m 644 files/dnsmasq.conf ${ROOTFS_DIR}/etc/

sed -i 's\#DAEMON_CONF=""\DAEMON_CONF="/etc/hostapd/hostapd.conf"\g' ${ROOTFS_DIR}/etc/default/hostapd

############ Network setup #########################################
cp -v files/interfaces ${ROOTFS_DIR}/etc/network/interfaces
