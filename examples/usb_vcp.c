#include "main.h"

USB_VCP vcp(PA_12, PA_11);

uint8_t buffer[] = "Hello World!\n";

int main(void)
{	
	while(1)
	{
		vcp.write(&buffer[0], sizeof(buffer));
		Delay(1000);
	}
}
