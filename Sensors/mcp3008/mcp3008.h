#ifndef _MCP3008_H_
#define _MCP3008_H_
#include <stdint.h>
#define MCP3008_DEFAULT_CHIPSELECT 0
#define START_MASK 0x01
#define SINGLE_ENDED_MODE (1<<7)
#define DIFFERENTIAL_MODE (0<<7)

#define nCHAN 8
#define MCP_CHANNEL0 (0x00<<4)
#define MCP_CHANNEL1 (0x01<<4)
#define MCP_CHANNEL2 (0x02<<4)
#define MCP_CHANNEL3 (0x03<<4)
#define MCP_CHANNEL4 (0x04<<4)
#define MCP_CHANNEL5 (0x05<<4)
#define MCP_CHANNEL6 (0x06<<4)
#define MCP_CHANNEL7 (0x07<<4)

typedef struct mcp3008_t {
	int raw[nCHAN];
  unsigned int spiFd;
}mcp3008_t;

int mcp3008_init(struct mcp3008_t *mcp, int chipselect); 
void mcp3008_exit(struct mcp3008_t *mcp);
int mcp3008_capture(struct mcp3008_t *mcp, uint8_t channel, uint8_t capture_mode);
void mcp3008_print(struct mcp3008_t *mcp);
#endif
