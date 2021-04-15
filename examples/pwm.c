#include "main.h"

#define FREQUENCY 1000 // 1kHz

PwmOut pwm1(PA_8, FREQUENCY, TIM1, Channel_1);

int main(void)
{
	uint8_t i = 0;
	uint8_t dir = 0;
	
	while(1)
	{
		pwm1 = i;
		
		// Increment/decrement
		i = (dir == 0) ? (i + 1) : (i - 1);
		
		if((i >= PWMOUT_DUTYCYCLE_MAX) || (i == 0)) dir = ~dir;
		
		Delay(10);
	}
}

