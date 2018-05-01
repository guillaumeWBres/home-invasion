#ifndef __XBEE_H__
#define __XBEE_H__

#define BUFSIZE 64

// initializes UART for XBEE communcations
// notifies BS we're live when done
void xbee_init(void);

// sends given command to XBEE module
void xbee_send_command(const char *cmd, const int size);

// sends given command to XBEE module
// waits for answer terminated by '\r'
// returns -1 otherwise
int xbee_confirmed_command(const char *cmd, const int size); 

// sets XBEE module up using given parameters
void xbee_setup(const char *ID, const char *MY, const char *DL);

// puts XBEE module in hibernation mode (PIN mode)
// or wakes it up
void xbee_sleep(void);
void xbee_wakeup(void);

// sends given message as broadcast
void xbee_broadcast(const char *payload, const int size);

// sends given message to specified dest ADDR
void xbee_unicast(const char *payload, const char *dl, const int size);

// waits until a valid frame has been received
void xbee_wait_instructions(char *buffer);

// calculates checksum of given API frame
uint8_t checksum(const char *API_frame);

#endif
