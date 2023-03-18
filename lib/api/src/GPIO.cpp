/*!
 * \file GPIO.cpp
 * \brief GPIO API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 14 avril 2021
 *
 * GPIO library.
 *
 */

#include "GPIO.h"

GPIO :: GPIO(PinName pin, PinMode m)
{
	m_port = ((GPIO_TypeDef *)(pin & 0xFFFFFF00));
	m_pin = (uint8_t)pin;
	m_mask = ((uint32_t)0x01 << m_pin);

	if(pin != NC) {
		// GPIO clock enable
		clock((uint32_t) m_port);

		// Configure pin in output
		this->mode(m);
	}
}

void GPIO :: clock(uint32_t port)
{
	uint32_t tmp = 0;

	switch(port) {
		case GPIOA_BASE: tmp = RCC_APB2ENR_IOPAEN; break;
		case GPIOB_BASE: tmp = RCC_APB2ENR_IOPBEN; break;
		case GPIOC_BASE: tmp = RCC_APB2ENR_IOPCEN; break;
		case GPIOD_BASE: tmp = RCC_APB2ENR_IOPDEN; break;
		default: break;
	}

	RCC->APB2ENR |= tmp;
}

void GPIO :: mode(PinMode m)
{
	if(m_pin < 8) {
		m_port->CRL  &= ~((GPIO_CRL_MODE0 + GPIO_CRL_CNF0) << (m_pin * 4));
		m_port->CRL |= (((uint32_t)m) << (m_pin * 4));
	} else {
		m_port->CRH  &= ~((GPIO_CRH_MODE8 + GPIO_CRH_CNF8) << ((m_pin - 8) * 4));
		m_port->CRH |= (((uint32_t)m) << ((m_pin - 8) * 4));
	}
}

void GPIO :: type(PinType t)
{
	if(m_pin < 8) {
		m_port->CRL  &= ~(GPIO_CRL_CNF0_0 << (m_pin * 4));
		m_port->CRL |= ((GPIO_CRL_CNF0_0 & (uint32_t)t) << (m_pin * 4));
	} else {
		m_port->CRH  &= ~(GPIO_CRH_CNF8_0 << ((m_pin - 8) * 4));
		m_port->CRH |= ((GPIO_CRH_CNF8_0 & (uint32_t)t) << ((m_pin - 8) * 4));
	}
}

void GPIO :: pull(PinPull p)
{
	if(p == Pull_Up)
		m_port->BSRR |= (0x01 << m_pin);
	else
		m_port->BRR |= (0x01 << m_pin);
}

uint32_t GPIO :: port(void)
{
	return (uint32_t)m_port;
}

uint32_t GPIO :: mask(void)
{
	return m_mask;
}

uint8_t GPIO :: pin(void)
{
	return m_pin;
}

void GPIO :: write(uint32_t value)
{
	if(value == 0) m_port->BRR = m_mask;
	else m_port->BSRR = m_mask;
}

uint32_t GPIO :: read(void)
{
	return ((m_port->IDR & m_mask) ? 1 : 0);
}

GPIO :: operator uint32_t()
{
	return this->read();
}
