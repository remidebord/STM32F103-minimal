/*!
 * \file USB.cpp
 * \brief USB API.
 * \author Remi.Debord
 * \version 1.0
 * \date 18 avril 2021
 *
 * USB library (Virtual Com Port).
 *
 */

#include "USB.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

USB_VCP :: USB_VCP(PinName dp, PinName dm): m_dp(dp, Pin_InputFloating), m_dm(dm, Pin_AF)
{	
	MX_USB_DEVICE_Init();
}

void USB_VCP :: write(uint8_t* buffer, uint16_t length)
{
    CDC_Transmit_FS(buffer, length);
}

uint16_t USB_VCP :: read(uint8_t* buffer)
{
    uint16_t length = 0;
    
    // TODO
    
    return length;
}
