#include "main.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

uint8_t buffer[] = "Hello World!\n";

int main(void)
{
	/* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	
	MX_USB_DEVICE_Init();
	
	while(1)
	{
		CDC_Transmit_FS(&buffer[0], sizeof(buffer));
		Delay(1000);
	}
}
