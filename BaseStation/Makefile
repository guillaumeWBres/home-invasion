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
endif

OBJ = main
SRCS = Network.cc
SRCS += Node.cc

#SRCS = OpenCV/opencv_lib.cpp

CFLAGS += -Wall
#CFLAGS = -O3 
#CFLAGS += -I$(BUILDROOT_STAGING_DIR)/usr/include
#LDFLAGS = -L$(BUILDROOT_STAGING_DIR)/usr/lib
#LDFLAGS += -lopencv_core -lopencv_imgproc -lopencv_objdetect
#LDFLAGS += -lopencv_video -lopencv_videoio -lopencv_highgui
#LDFLAGS += -lm

ifeq ($(USE_GOOGLE_VOICE),Yes)
CFLAGS += -DUSE_GOOGLE_VOICE
LDFLAGS += -lcurl
LDFLAGS += -lboost_regex
SRCS += GoogleVoice/gvoice.cpp
SRCS += GoogleVoice/send_text.cpp
endif

$(OBJ): $(OBJ).cc $(SRCS)
	$(CXX) $(OBJ).cc $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(OBJ)

clean:
	rm $(OBJ)
