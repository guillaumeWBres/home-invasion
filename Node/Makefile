MCU = avrtiny 
FMCU_HZ = 4000000

TARGET = main

CC = avr-gcc

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET) -mmcu=$(MCU) -DF_CPU=$(FMCU_HZ)

clean:
	rm $(TARGET)
