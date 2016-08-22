MCU = atmega2561
FMCU_HZ = 4000000

TARGET = main

CC = avr-gcc
CXX = avr-g++
CPP = avr-cpp
LD = avr-ld
NM = avr-nm
AR = avr-ar
AS = avr-as
SIZE = avr-size
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

CFLAGS = -funsigned-char 
CFLAGS += -funsigned-bitfields 
CFLAGS += -fpack-struct 
CFLAGS += -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -O3
CFLAGS += -std=gnu99
CFLAGS += -mmcu=$(MCU) 
CFLAGS += -DF_CPU=$(FMCU_HZ)UL

SRCS = adc.c
SRCS += i2c.c

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c $(SRCS) $(CFLAGS) -o $(TARGET)

install: $(TARGET)
	sudo avrdude -q -V -D -p$(MCU) -P/dev/ttyUSB0 -c arduino -U flash:w:$(TARGET)

clean:
	rm $(TARGET)
