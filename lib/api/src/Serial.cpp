/*!
 * \file Serial.cpp
 * \brief USART API.
 * \author Remi.Debord
 * \version 1.0
 * \date 17 avril 2021
 *
 * USART library.
 *
 */

#include "Serial.h"

extern "C"
{
	static CircularBuffer* bufferRx[2];
	static CircularBuffer* bufferTx[2];
}

Serial :: Serial(USART_TypeDef* usart, PinName rx, PinName tx): m_rx(rx, Pin_InputFloating), m_tx(tx, Pin_AF),
																m_circularRx(&m_bufferRx[0], USART_BUFFER_SIZE),
																m_circularTx(&m_bufferTx[0], USART_BUFFER_SIZE)
{
	uint8_t index = 0;
	uint8_t prescaler = 0;

	IRQn_Type irq = USART1_IRQn;

	m_usart = usart;

	// Enable USART clock
	switch((uint32_t)usart)
	{
		case USART1_BASE: RCC->APB2ENR |= RCC_APB2ENR_USART1EN; break;
		case USART2_BASE: RCC->APB1ENR |= RCC_APB1ENR_USART2EN; break;
		default: break;
	}

	// Configure Tx pin
	if(tx != NC) Serial::pin(&m_tx);

	// Link circular buffer
	if(usart == USART1) index = 0;
	else if(usart == USART2) index = 1;

	bufferRx[index] = &m_circularRx;
	bufferTx[index] = &m_circularTx;

	// USART clock: USART1 64MHz (APB2), USART2 32 MHz (APB1)
	if(m_usart == USART2) prescaler = 2;
	else prescaler = 1;

	// USART configuration (9600 8 N 1)
	m_usart->BRR = SystemCoreClock / (prescaler * USART_BAUDRATE_DEFAULT); // Baudrate = fck (48MHz) / USARTDIV
	m_usart->CR1 &= ~USART_CR1_M;                                          // Databits: 8
	m_usart->CR1 &= ~(USART_CR1_PCE | USART_CR1_PS);                       // Parity: None
	m_usart->CR2 &= ~USART_CR2_STOP;                                       // Stopbits: 1

	// USART mode (Rx and/or Tx)
	m_usart->CR1 &= ~(USART_CR1_RE | USART_CR1_TE);

	if(rx != NC) m_usart->CR1 |= USART_CR1_RE;
	if(tx != NC) m_usart->CR1 |= USART_CR1_TE;

	// Disable flow control
	m_usart->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE);

	// USART interrupt (Rx and/or Tx)
	m_usart->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_TXEIE);

	if(rx != NC) m_usart->CR1 |= USART_CR1_RXNEIE;
	//if(tx != NC) m_usart->CR1 |= USART_CR1_TXEIE;

	// NVIC configuration
	if(usart == USART1) irq = USART1_IRQn;
	else if(usart == USART2) irq = USART2_IRQn;

	NVIC_SetPriority(irq, 1); // High: 0, Low: 3
	NVIC_EnableIRQ(irq);

	// Enable USART
	m_usart->CR1 |= USART_CR1_UE;
}

void Serial :: pin(GPIO* gpio)
{
	// Set pin in push-pull, pull-down
	gpio->type(Push_Pull);
	gpio->pull(Pull_Down);
}

void Serial :: baudrate(uint32_t value)
{
	uint8_t prescaler = 0;

	// Disable USART
	m_usart->CR1 &= ~USART_CR1_UE;

	// USART clock: USART1 64MHz (APB2), USART2 32 MHz (APB1)
	if(m_usart == USART2) prescaler = 2;
	else prescaler = 1;

	// Baudrate = fck (64MHz) / USARTDIV
	m_usart->BRR = SystemCoreClock / (prescaler * value);

	// Enable USART
	m_usart->CR1 |= USART_CR1_UE;
}

void Serial :: format(uint8_t databits, SerialParity parity, uint8_t stopbits)
{
	uint32_t tmp = 0;

	// Disable USART
	m_usart->CR1 &= ~USART_CR1_UE;

	// Databits
	switch(databits)
	{
		case 8: tmp = ~USART_CR1_M; break;
		case 9: tmp = USART_CR1_M; break;
		default: break; // 8 bits
	}

	m_usart->CR1 &= ~USART_CR1_M;
	m_usart->CR1 |= tmp;

	// Parity
	m_usart->CR1 &= ~(USART_CR1_PCE | USART_CR1_PS);

	switch(parity)
	{
		case None: break;
		case Even: m_usart->CR1 |= (USART_CR1_PCE); break;
		case Odd : m_usart->CR1 |= (USART_CR1_PCE | USART_CR1_PS); break;
		default: break;
	}

	// Stopbits
	switch(stopbits)
	{
		case 1: tmp = 0; break;
		case 2: tmp = USART_CR2_STOP_1; break;
		case 15: tmp = (USART_CR2_STOP_1 | USART_CR2_STOP_0); break;
		default: break; // 1 bit
	}

	m_usart->CR2 &= ~USART_CR2_STOP;
	m_usart->CR2 |= tmp;

	// Enable USART
	m_usart->CR1 |= USART_CR1_UE;
}

uint8_t Serial :: write(uint8_t* buffer, uint16_t length)
{
	uint16_t i = 0;
	uint8_t result = 0;

	// Tx buffer available ?
	if(m_circularTx.count() == 0) {
		for(i = 0; i < length; i++)
			m_circularTx.put(buffer[i]);

		// Enable Tx interrupt
		m_usart->CR1 |= USART_CR1_TXEIE;

		result = 1;
	}

	return result;
}

uint16_t Serial :: read(uint8_t* buffer)
{
	uint16_t length = 0;
	uint16_t i = 0;

	// Rx operation ongoing ?
	if((m_usart->SR & USART_SR_IDLE) != 0) {
		length = m_circularRx.count();

		if(length != 0) {
			for(i = 0; i < length; i++)
				buffer[i] = m_circularRx.get();
		}
	}

	return length;
}

extern "C"
{
	void USART1_IRQHandler(void)
	{
		if((USART1->SR & USART_SR_TXE) != 0) {
			// Data to send ?
			if(bufferTx[0]->count() && (bufferTx[0] != 0)) {
				USART1->DR = bufferTx[0]->get();
			} else {
				// Disable TXE interrupt
				USART1->CR1 &= ~USART_CR1_TXEIE;
			}
		}

		if((USART1->SR & USART_SR_RXNE) != 0) {
			if(bufferRx[0] != 0)
				bufferRx[0]->put(USART1->DR);
		}
	}

	void USART2_IRQHandler(void)
	{
		if((USART2->SR & USART_SR_TXE) != 0) {
			// Data to send ?
			if(bufferTx[1]->count() && (bufferTx[1] != 0)) {
				USART2->DR = bufferTx[1]->get();
			} else {
				// Disable TXE interrupt
				USART2->CR1 &= ~USART_CR1_TXEIE;
			}
		}

		if((USART2->SR & USART_SR_RXNE) != 0) {
			if(bufferRx[1] != 0)
				bufferRx[1]->put(USART2->DR);
		}
	}
}
