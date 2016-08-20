USE_OPENCV=No
USE_GOOGLE_VOICE=No

BUILDROOT_DIR = buildroot
BUILDROOT_CC_PREFIX = arm-buildroot-linux-uclibcgnueabihf
BUILDROOT_OUTPUT_DIR = $(BUILDROOT_DIR)/output
BUILDROOT_STAGING_DIR = $(BUILDROOT_OUTPUT_DIR)/staging
BUILDROOT_HOST_DIR = $(BUILDROOT_OUTPUT_DIR)/host
BUILDROOT_TOOLCHAIN_DIR = $(BUILDROOT_HOST_DIR)/usr/bin
BUILDROOT_CROSS_COMPILE = $(BUILDROOT_TOOLCHAIN_DIR)/$(BUILDROOT_CC_PREFIX)

CC = $(BUILDROOT_CROSS_COMPILE)-gcc
CXX = $(BUILDROOT_CROSS_COMPILE)-g++

OBJ = main

SRCS = Sensors/mcp3008/spi.c
SRCS += Sensors/mcp3008/mcp3008.c

CFLAGS = -O3 -Wall

ifeq ($(USE_GOOGLE_VOICE),Yes)
CFLAGS += -DUSE_GOOGLE_VOICE
SRCS += GoogleVoice/gvoice.cpp
SRCS += GoogleVoice/send_text.cpp
LDFLAGS = -lcurl
LDFLAGS += -lboost_regex
endif


ifeq ($(USE_OPENCV),Yes)
CFLAGS += -DUSE_OPENCV
SRCS += OpenCV/opencv_lib.cpp
CFLAGS += -I$(BUILDROOT_STAGING_DIR)/usr/include
LDFLAGS = -L$(BUILDROOT_STAGING_DIR)/usr/lib
LDFLAGS += -lopencv_core
LDFLAGS += -lopencv_imgproc
LDFLAGS += -lhighgui
endif

LDFLAGS += -lm

$(OBJ): $(OBJ).cpp $(SRCS)
	$(CXX) $(OBJ).cpp $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)

clean:
	rm $(OBJ)
