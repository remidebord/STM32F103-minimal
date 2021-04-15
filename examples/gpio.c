#include "main.h"

DigitalOut led1(PC_13);
DigitalOut led2(PA_8);
	
// DigitalIn pushButton(PB_13);
InterruptIn pushButton(PB_13);

void Push(void)
{
	led2 = !led2;
}

int main(void)
{
	pushButton.pull(Pull_Up);
	pushButton.rise(&Push);
	
	while(1)
	{
		led1 = 1;
		Delay(1000);
		led1 = 0;
		Delay(1000);
		
// 		if(pushButton) led2 = 1;
// 		else led2 = 0;
	}
}