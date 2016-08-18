#! /bin/sh

if [ ! -e buildroot ]; then # buildroot non installe'
	git clone https://github.com/buildroot/buildroot.git
fi;

if [ ! -e buildroot/output/images/MLO ]; then # image non construite
	cp bbone_defconfig buildroot/configs
	cd buildroot
	make bbone_defconfig
	make
	cd ..
fi;
