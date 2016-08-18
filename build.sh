#! /bin/sh

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
	email_address=$2
	password=$3

	
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
		cd ..
	fi;

	make
}
