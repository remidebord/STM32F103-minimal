/*!
 * \file Digital.cpp
 * \brief Digital API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 15 avril 2021
 *
 * Digital library (DigitalIn, DigitalOut and InterruptIn).
 *
 */

#include "Digital.h"

extern "C"
{
	void (*extiCallback[16])(void);
}

DigitalOut :: DigitalOut(PinName pin) : GPIO(pin, Pin_Output)
{
	this->type(Push_Pull);
	
	this->write(0);
}

DigitalOut& DigitalOut :: operator= (uint32_t value)
{
	this->write(value);
	return *this;
}

/////////////////////

DigitalIn :: DigitalIn(PinName pin) : GPIO(pin, Pin_Input)
{
	this->pull(Pull_Down);
}

/////////////////////

InterruptIn :: InterruptIn(PinName pin) : GPIO(pin, Pin_Input)
{
	uint32_t port = 0;
	uint8_t shift = 0;
	uint8_t index = 0;
	
	IRQn_Type irq = EXTI0_IRQn;
	
	// Alternate Function I/O clock enable
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	port  = (((((uint32_t)m_port) - APB2PERIPH_BASE) >> 10) - 2);
	shift = (0x04 * (m_pin & 0x03));
	index = m_pin >> 0x02;
	
	// AFIO external interrupt configuration
	AFIO->EXTICR[index] &= ~(0x0F << shift);
	AFIO->EXTICR[index] |= (port << shift);
	
	// Clear EXTI line configuration
	EXTI->IMR &= ~m_mask;
	EXTI->EMR &= ~m_mask;
	
	// Select interrupt mode
	EXTI->IMR |= m_mask;
	
	// Clear Rising Falling edge configuration
	EXTI->RTSR &= ~m_mask;
	EXTI->FTSR &= ~m_mask;
	
	// NVIC configuration
	if (m_pin == 0) irq = EXTI0_IRQn;
	else if (m_pin == 1) irq = EXTI1_IRQn;
	else if (m_pin == 2) irq = EXTI2_IRQn;
	else if (m_pin == 3) irq = EXTI3_IRQn;
	else if (m_pin == 4) irq = EXTI4_IRQn;
	else if (m_pin <= 9) irq = EXTI9_5_IRQn;
	else irq = EXTI15_10_IRQn;
	
	NVIC_SetPriority(irq, 1); // High: 0, Low: 3
	NVIC_EnableIRQ(irq);
}

void InterruptIn :: rise(void(*f)(void))
{
	extiCallback[m_pin] = f;
	
	EXTI->RTSR |= m_mask;
}

void InterruptIn :: fall(void(*f)(void))
{	
	extiCallback[m_pin] = f;
	
	EXTI->FTSR |= m_mask;
}

void InterruptIn :: risefall(void(*f)(void))
{	
	extiCallback[m_pin] = f;
	
	EXTI->RTSR |= m_mask;
	EXTI->FTSR |= m_mask;
}

extern "C"
{
	void EXTI0_IRQHandler(void)
	{
			// Check pending register
			if ((EXTI->PR & ((uint32_t)0x01 << 0)) != 0)
			{
				// Callback ?
				if(extiCallback[0] != 0) (*extiCallback[0])();
				
				// Clear bit
				EXTI->PR = ((uint32_t)0x01 << 0);
			}
	}
	
	void EXTI1_IRQHandler(void)
	{
			// Check pending register
			if ((EXTI->PR & ((uint32_t)0x01 << 1)) != 0)
			{
				// Callback ?
				if(extiCallback[1] != 0) (*extiCallback[1])();
				
				// Clear bit
				EXTI->PR = ((uint32_t)0x01 << 1);
			}
	}
	
	void EXTI2_IRQHandler(void)
	{
			// Check pending register
			if ((EXTI->PR & ((uint32_t)0x01 << 2)) != 0)
			{
				// Callback ?
				if(extiCallback[2] != 0) (*extiCallback[2])();
				
				// Clear bit
				EXTI->PR = ((uint32_t)0x01 << 2);
			}
	}
	
	void EXTI3_IRQHandler(void)
	{
			// Check pending register
			if ((EXTI->PR & ((uint32_t)0x01 << 3)) != 0)
			{
				// Callback ?
				if(extiCallback[3] != 0) (*extiCallback[3])();
				
				// Clear bit
				EXTI->PR = ((uint32_t)0x01 << 3);
			}
	}
	
	void EXTI4_IRQHandler(void)
	{
			// Check pending register
			if ((EXTI->PR & ((uint32_t)0x01 << 4)) != 0)
			{
				// Callback ?
				if(extiCallback[4] != 0) (*extiCallback[4])();
				
				// Clear bit
				EXTI->PR = ((uint32_t)0x01 << 4);
			}
	}
	
	void EXTI9_5_IRQHandler(void)
	{
		uint32_t extiLine = 0;
		uint8_t i = 0;
		
		for(i = 5; i <= 9; i++)
		{
			extiLine = ((uint32_t)0x01 << i);
			
			// Check pending register
			if ((EXTI->PR & (extiLine)) != 0)
			{
				// Callback ?
				if(extiCallback[i] != 0) (*extiCallback[i])();
				
				// Clear bit
				EXTI->PR = extiLine;
			}			
		}
	}
	
	void EXTI15_10_IRQHandler(void)
	{
		uint32_t extiLine = 0;
		uint8_t i = 0;
		
		for(i = 10; i <= 15; i++)
		{
			extiLine = ((uint32_t)0x01 << i);
			
			// Check pending register
			if ((EXTI->PR & (extiLine)) != 0)
			{
				// Callback ?
				if(extiCallback[i] != 0) (*extiCallback[i])();
				
				// Clear bit
				EXTI->PR = extiLine;
			}			
		}
	}
}

