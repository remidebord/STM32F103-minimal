#include "main.h"
	
Ticker tick(TIM2);
DigitalOut led1(PA_8);
DigitalOut led2(PC_13);

void blink(void)
{
	led1 = !led1;
}

int main(void)
{
	tick.attach_ms(&blink, 1000);
	
	while(1)
	{
		led2 = !led2;
		Delay(500);
	}
}