TARGET = Host 

BUILDROOT_DIR = buildroot
BUILDROOT_CC_PREFIX = arm-buildroot-linux-uclibcgnueabihf
BUILDROOT_OUTPUT_DIR = $(BUILDROOT_DIR)/output
BUILDROOT_STAGING_DIR = $(BUILDROOT_OUTPUT_DIR)/staging
BUILDROOT_HOST_DIR = $(BUILDROOT_OUTPUT_DIR)/host
BUILDROOT_TOOLCHAIN_DIR = $(BUILDROOT_HOST_DIR)/usr/bin
BUILDROOT_CROSS_COMPILE = $(BUILDROOT_TOOLCHAIN_DIR)/$(BUILDROOT_CC_PREFIX)

ifeq ($(TARGET),Beaglebone)
CC = $(BUILDROOT_CROSS_COMPILE)-gcc
CXX = $(BUILDROOT_CROSS_COMPILE)-g++
else
CC = gcc
CXX = g++
endif

OBJ = main
SRCS = GoogleVoice/gvoice.cpp

CFLAGS = -O3 -Wall

ifeq ($(TARGET),Beaglebone)
CFLAGS += -I$(BUILDROOT_STAGING_DIR)/usr/include
LDFLAGS = -L$(BUILDROOT_STAGING_DIR)/usr/lib
LDFLAGS += -lopencv_core
LDFLAGS += -lopencv_imgproc
LDFLAGS += -lhighgui
else
CFLAGS += `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`
endif

LDFLAGS += -lcurl
LDFLAGS += -lboost_regex
LDFLAGS += -lm

$(OBJ): $(OBJ).cpp
	$(CXX) $(OBJ).cpp $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)

clean:
	rm $(OBJ)
