#ifndef __COMMON_H
#define __COMMON_H

/* includes ---------------------------------------------------------------- */
#include "stm32f1xx.h"
#include "core_cm3.h"

/* define ------------------------------------------------------------------ */

/* enum -------------------------------------------------------------------- */
typedef enum {
	PA_0 = GPIOA_BASE,
	PA_1,
	PA_2,
	PA_3,
	PA_4,
	PA_5,
	PA_6,
	PA_7,
	PA_8,
	PA_9,
	PA_10,
	PA_11,
	PA_12,
	PA_13,
	PA_14,
	PA_15,

	PB_0 = GPIOB_BASE,
	PB_1,
	PB_2,
	PB_3,
	PB_4,
	PB_5,
	PB_6,
	PB_7,
	PB_8,
	PB_9,
	PB_10,
	PB_11,
	PB_12,
	PB_13,
	PB_14,
	PB_15,

	PC_0 = GPIOC_BASE,
	PC_1,
	PC_2,
	PC_3,
	PC_4,
	PC_5,
	PC_6,
	PC_7,
	PC_8,
	PC_9,
	PC_10,
	PC_11,
	PC_12,
	PC_13,
	PC_14,
	PC_15,

	PD_0 = GPIOD_BASE,
	PD_1,
	PD_2,

	// Not connected
	NC = 0xFFFFFFFF
} PinName;

typedef enum {
	Pin_Input  = 0x08, // Input with pull-up / pull-down (1000)
	Pin_Output = 0x03, // General purpose output push-pull (0011)
	Pin_AF     = 0x0B, // Alternate function output Push-pull (1011)
	Pin_AN     = 0x00  // Input Analog mode (0000)
} PinMode;

typedef enum {
	Push_Pull  = 0x00,
	Open_Drain = 0x01
} PinType;

typedef enum {
	Pull_Up      = 0x01,
	Pull_Down    = 0x00,
} PinPull;

typedef enum {
	Speed_Low    = 0x00,
	Speed_Medium = 0x01,
	Speed_High   = 0x03
} PinSpeed;

typedef enum {
	None = 0x00,
	Even = 0x02,
	Odd  = 0x03
} SerialParity;

typedef enum {
	Channel_1 = 0x01,
	Channel_2 = 0x02,
	Channel_3 = 0x03,
	Channel_4 = 0x04
} TimerChannel;

/* functions --------------------------------------------------------------- */
uint8_t bcd2bin(uint8_t bcd);
uint8_t bin2bcd(uint8_t bin);

#endif
