#include "main.h"

// Serial port config
Serial serial(USART1, PA_10, PA_9);
//Serial serial(USART2, PA_3, PA_2);

uint8_t buffer[128] = {0};
uint16_t length = 0;

int main(void)
{
	// Change baudrate (default: 9600)
	serial.baudrate(115200);

	serial.write((uint8_t*)("Hello world!"), 12);
	
	while(1)
	{
		length = serial.read(buffer);
		
		// echo mode
		if(length)
		{
			serial.write(buffer, length);
		}
	}
}
