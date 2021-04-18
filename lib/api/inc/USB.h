#ifndef __USB_H
#define __USB_H

/* includes ---------------------------------------------------------------- */
#include "GPIO.h"

/* defines ----------------------------------------------------------------- */

/* class ------------------------------------------------------------------- */
class USB_VCP
{
	private:
	
		GPIO m_dp;
		GPIO m_dm;
	
	public:
	
		USB_VCP(PinName dp, PinName dm);
	
		void write(uint8_t* buffer, uint16_t length);
		uint16_t read(uint8_t* buffer);
};

#endif
