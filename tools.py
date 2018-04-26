#! /usr/bin/env python3

import sys
import time
import serial

# opens serial port
def open_serial(tty, baudrate):
	ser = serial.Serial(tty)
	ser.baudrate = baudrate
	ser.flushInput()
	ser.flushOutput()
	# setup
	ser.bytesize = serial.EIGHTBITS
	ser.parity = serial.PARITY_NONE
	ser.stopbits = serial.STOPBITS_ONE
	ser.timeout = 0
	ser.xonxoff = 0
	ser.rtscts = False
	ser.dsrdtr = False
	ser.writeTimeout = 4
	return ser

def read_xbee_settings(tty, baud):
	# reads all xbee module settings 
	# connected to specified serial port
	ser = open_serial("/dev/ttyUSB0", 9600)
	
	print("reading settings..")

	# enter settings
	ser.write(bytes("+++",encoding="utf-8"))
	time.sleep(2)
	print("entered settings mode..")

	string = "\n----- XBEE settings -----\n"
	commands = ["ATBD\r", "ATID\r", "ATMY\r", "ATDH\r", "ATDL\r", "ATNJ\r"]
	names = ["Baud", "Pan ID", "MY", "DH", "DL", "Network join time"]
	for command in commands:
		ser.write(bytes(command, encoding="utf-8"))
		time.sleep(2)
		read = ser.readline().decode("utf-8")
		string += "{:s}: {:s}\n".format(names[commands.index(command)], read)
	string += "-------------------------\n"
	print(string)

	ser.close()

def main(argv):
	argv=argv[1:]
	if (len(argv) == 0):
		print("./tools --port=p --baud=b")
		return 0
	
	else:
		for arg in argv:
			key = arg.split("=")[0]
			value = arg.split("=")[-1]
			if key == "--port":
				tty = value
			elif key == "--baud":
				baud = value
			else:
				print("Unknown flag {:s}".format(key))

	read_xbee_settings(tty,baud)

main(sys.argv)
