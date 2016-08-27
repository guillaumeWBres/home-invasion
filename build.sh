#! /bin/bash

#TODO: gvoice Credentials
#TODO: server IP address & login => scp script
#TODO: ssh-key gen accept password
#TODO: SSH key with server IP => usermod / file perm

SUPPORTED_ARCHITECTURES=("beaglebone")
CUSTOM_DEFCONFIGS=("bbone_defconfig")

GIT_DIR=`pwd`
BASE_STATION_DIR=$GIT_DIR/BaseStation
NOSE_STATION_DIR=$GIT_DIR/Node

BUILDROOT_GIT_URL=https://github.com/buildroot/buildroot.git
BUILDROOT_DIR=$BASE_STATION_DIR/buildroot
BUILDROOT_CONFIG_DIR=$BUILDROOT_DIR/configs
BUILDROOT_BOARD_DIR=$BUILDROOT_DIR/board
BUILDROOT_OUTPUT_DIR=$BUILDROOT_DIR/output
BUILDROOT_IMAGE_DIR=$BUILDROOT_OUTPUT_DIR/images
BUILDROOT_OVERLAY_DIR=$BASE_STATION_DIR/overlay
		
AVR_REQUIRED_PACKAGES=("arduino" "arduino-core" "arduino-mk")

function install_buildroot(){
	if [ ! -e $BUILDROOT_DIR ]; then
		cd $BASE_STATION_DIR
		git clone $BUILDROOT_GIT_URL 
		cd $GIT_DIR
	fi;
}

function build_embedded_os(){_
	if [ ! -e $BUILDROOT_IMAGE_DIR/MLO ]; then
		cp ${CUSTOM_DEFCONFIG[0]} $BUILDROOT_CONFIG_DIR 
		make -C $CUSTOM_DEFCONFIG
		make -C $BUILDROOT_DIR

		echo "image has been built, install it with:"
		echo "sudo dd if=$BUILDROOT_DIR/output/images/sdcard.img of=dev/sdc"
	fi;
}

function test_avr_gcc(){
	avr_cc_exists=`which avr-gcc`
	if [[ -z "$avr_cc_exists" ]]; then
		sudo apt-get install ${AVR_REQUIRED_PACKAGES[@]}
	fi;
}

ARCH=beaglebone
CUSTOM_DEFCONFIG=bbone_defconfig

install_buildroot
build_embedded_os
make -C $BASE_STATION_DIR

test_avr_gcc
make -C $NODE_STATION_DIR
