#/bin/bash -e

####################################################################
#
#           Installation of LMX-001 example app and documentation
#
####################################################################

############ Insert git tag and related into docs ##################
GITREV=$(git describe --tags)
NOW=$(date)
echo $GITREV
sed -i -r "s/(Build date:)(.*)/\1 $NOW/g" files/fb/documentation.txt
sed -i -r "s/(Git describe:)(.*)/\1 $GITREV/g" files/fb/documentation.txt

############ Install demo, example and documentation files #########
install -d ${ROOTFS_DIR}/home/pi/piaizu/demo
install -t ${ROOTFS_DIR}/home/pi/piaizu/demo files/demo/*

install -d ${ROOTFS_DIR}/home/pi/lmx_example
cp -rfv files/fb/* ${ROOTFS_DIR}/home/pi/lmx_example/

make -C ${ROOTFS_DIR}/home/pi/lmx_example doc
cp -rfv ${ROOTFS_DIR}/home/pi/lmx_example/docs/html/* ${ROOTFS_DIR}/var/www/html

############ Setup terminal #######################
cp -v files/.profile ${ROOTFS_DIR}/home/pi/.profile
cp -v files/.bashrc ${ROOTFS_DIR}/home/pi/.bashrc
############ Create copy of this buildsystem on target fs#################
buildsystem_path=piaizu_$GITREV
pth=$(readlink -e ../..)
#if [ ! -d "${ROOTFS_DIR}/home/pi/$buildsystem_path" ]; then
#	git clone file://$pth --depth=1 ${ROOTFS_DIR}/home/pi/$buildsystem_path
#fi

