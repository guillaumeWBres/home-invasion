## Home Automation

This is a home intrusion detection device.

The application detects intruders using one
video camera. When an intrusion has been
detected the application shoots a short
video supposed to be sent to a remote server (ssh/scp).

When an intrusion has been detected, the user
is warned by text message.
The application uses Google Voice to send a text message.
The user should provide valid Google Voice credentials
when building the application.


### Build the application

Use the main script to do so:

```bash
	./build.sh
```

Install the generated root file system onto
target, this is found in buildroot/output/images.

More informations can be found in:

 1. buildroot/boards/beaglebone/readme.txt

Current architecture supported: Beaglebone.

Current dependencies: 

 1. opencv (video processing)
 2. googleVoice (custom lib provided)
 3. ssh 

Linux system is built using Buildroot.

### Todo

1. Support a few sensors to provide more robust
detection.

2. Test detection in rough conditions.
