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

USB_VCP :: USB_VCP(PinName dp, PinName dm): m_dp(dp, Pin_InputFloating), m_dm(dm, Pin_InputFloating)
{
	m_dp.mode(Pin_Output);
	m_dm.mode(Pin_Output);

	// Trigger a reset (10ms) of D+ and D- in order to avoid
	// cable disconnection/connection after a MCU reset
	m_dp.write(0);
	m_dm.write(0);

	HAL_Delay(10);

	m_dp.mode(Pin_InputFloating);
	m_dm.mode(Pin_InputFloating);

	MX_USB_DEVICE_Init();
}

void USB_VCP :: write(uint8_t* buffer, uint16_t length)
{
	CDC_Transmit_FS(buffer, length);
}

uint16_t USB_VCP :: read(uint8_t* buffer)
{
	uint16_t length = 0;

	length = CDC_Receive(buffer);

	return length;
}
