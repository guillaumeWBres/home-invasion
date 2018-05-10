DEBUG = 1

ifeq ($(DEBUG),1)
CC = gcc
CXX = g++
else
BUILDROOT_DIR = buildroot
BUILDROOT_CC_PREFIX = arm-buildroot-linux-uclibcgnueabihf
BUILDROOT_OUTPUT_DIR = $(BUILDROOT_DIR)/output
BUILDROOT_STAGING_DIR = $(BUILDROOT_OUTPUT_DIR)/staging
BUILDROOT_HOST_DIR = $(BUILDROOT_OUTPUT_DIR)/host
BUILDROOT_TOOLCHAIN_DIR = $(BUILDROOT_HOST_DIR)/usr/bin
BUILDROOT_CROSS_COMPILE = $(BUILDROOT_TOOLCHAIN_DIR)/$(BUILDROOT_CC_PREFIX)
CC = $(BUILDROOT_CROSS_COMPILE)-gcc
CXX = $(BUILDROOT_CROSS_COMPILE)-g++
#CFLAGS = -O3 
endif

OBJ = main
SRCS = BaseStation.cc
SRCS += Node.cc
SRCS += Network.cc
SRCS += TimeZone.cc

LDFLAGS += -I/usr/include/mysql
LDFLAGS += -L/usr/include/mysql

CFLAGS += -Wall
CFLAGS += -lmysqlclient

$(OBJ): $(OBJ).cc $(SRCS)
	$(CXX) $(OBJ).cc $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)

clean:
	rm $(OBJ)
