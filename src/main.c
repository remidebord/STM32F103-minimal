#include "main.h"

USB_VCP vcp(PA_12, PA_11);

uint8_t hello[] = "Hello World!\n";

uint8_t buffer[256] = {0};
uint16_t length = 0;

int main(void)
{	
	vcp.write(&hello[0], sizeof(hello));
	Delay(1000);
	
	while(1)
	{
		length = vcp.read(&buffer[0]);
		
		if(length != 0)
		{
			vcp.write(&buffer[0], length);
		}
	}
}

