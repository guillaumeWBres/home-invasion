#! /bin/bash

#TODO: gvoice Credentials
#TODO: server IP address & login => scp script
#TODO: ssh-key gen accept password
#TODO: SSH key with server IP => usermod / file perm

SUPPORTED_ARCH=("beaglebone")
CUSTOM_DEFCONFIG=("bbone_defconfig")

BUILDROOT_DIR=buildroot
BUILDROOT_CONFIG_DIR=buildroot/configs

DEFAULT_VIDEO_FILE="detection.avi"
DEFAULT_SCP_SCRIPT="scp_to_server.sh"

function print_help(){
	echo -e "This script builds the application,
	supported architectures: ${SUPPORTED_ARCH[@]}."
}

function build_scp_script(){
	server_ip_address=$1
	login_to_server=$2
	echo "scp $DEFAULT_VIDEO_FILE $login_to_server@$server_ip_address" >> $DEFAULT_SCP_SCRIPT
}

function generate_ssh_key(){
	local email_address=$2
	local password=$3

	
	ssh_key_gen_exists=`which ssh-keygen`
	if [[ -z "$ssh_key_gen_exists" ]]; then
		sudo apt-get install ssh
	fi;

	ssh-keygen -t rsa -C "$email_address"
}

function build_project(){

	if [ ! -e buildroot ]; then # buildroot non installe'
		git clone https://github.com/buildroot/buildroot.git
	fi;

	if [ ! -e buildroot/output/images/MLO ]; then # image non construite
		cp ${CUSTOM_DEFCONFIG[0]} $BUILDROOT_CONFIG_DIR 
		cd $BUILDROOT_DIR
		make ${CUSTOM_DEFCONFIG[0]}
		make
		echo "image has been built"
		echo "install $BUILDROOT_DIR/output/images/{am335x-bone.dtb,MLO,u-boot.img,uEnv.txt,zImage} onto sdb1 (FAT16)"
		echo "then sudo dd if=$BUILDROOT_DIR/output/images/rootfs.ext4 /dev/sdb2 (EXT4)"
		cd ..
	fi;

	make
}

build_project
