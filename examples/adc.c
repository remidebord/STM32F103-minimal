#include "main.h"
	
uint16_t value[5] = {0};

AnalogIn ain1(PB_1); // Channel 9
AnalogIn ain2(PA_4); // Channel 4
AnalogIn ain3(PA_0); // Channel 0
AnalogIn ain4(PB_0); // Channel 8
AnalogIn ain5(PA_2); // Channel 2

int main(void)
{	
	while(1)
	{
		value[0] = ain1.read_b();
		value[1] = ain2.read_b();
		value[2] = ain3.read_b();
		value[3] = ain4.read_b();
		value[4] = ain5.read_b();
	}
}
