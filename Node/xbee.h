#ifndef __XBEE_H__
#define __XBEE_H__

// initializes UART for XBEE communcations
// notifies BS we're live when done
void xbee_init(void);

// notifies base station with given status phrase
// expects const char terminated by '\r\n'
void xbee_send_base(const char *sts);

#endif
