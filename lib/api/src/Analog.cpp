/*!
 * \file Analog.cpp
 * \brief Analog API.
 * \author Remi.Debord
 * \version 1.0
 * \date 17 avril 2021
 *
 * Analog library (ADC and DAC).
 *
 */

#include "Analog.h"

__IO uint8_t AnalogIn::m_ranks = 0;
__IO uint16_t AnalogIn::m_value[ADC_CHANNELS_MAX] = {0};

AnalogIn :: AnalogIn(PinName pin) : GPIO(pin, Pin_AN)
{	
	// ADC up ?
	if((ADC1->CR2 & ADC_CR2_ADON) != 0)
	{
		// Stop ADC/conversion
		ADC1->CR2 &= ~ADC_CR2_ADON;
		
		// Wait ADC
		while((ADC1->CR2 & ADC_CR2_ADON) != 0);
	}
	
	// ADC configuration needed ?
	if(ADC1->CR2 == 0)
	{
		// DMA access: disable
		ADC1->CR2 &= (uint32_t)(~ADC_CR2_DMA);
	
		AnalogIn::adc();
	}
	
	// Get ADC channel
	m_channel = AnalogIn::channel(pin);
	
	// Store channel index
	m_rank = m_ranks++;
	
	// DMA configuration
	AnalogIn::dma();	
	
	// Channel configuration
	
	// Regular sequence configuration
	if(m_ranks <= 7)
	{
		ADC1->SQR3 |= (m_channel << (m_rank * 5));
	}
	else if(m_ranks <= 13)
	{
		ADC1->SQR2 |= (m_channel << ((m_rank - 7) * 5));
	}
	else
	{
		ADC1->SQR1 |= (m_channel << ((m_rank - 13) * 5));
	}
	
	// Set the number of channels to convert
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 |= ((m_ranks - 1) << ADC_SQR1_L_Pos);
	
	// Channel sampling time configuration (239.5 ADC clock cycles)
	if(m_channel >= 10)
	{
		ADC1->SMPR1 |= (ADC_SMPR1_SMP10 << ((m_channel - 10) * 3));
	}
	else
	{
		ADC1->SMPR2 |= (ADC_SMPR2_SMP0 << (m_channel * 3));
	}
	
	// Conversion time = (239.5 + 12.5) x (1 / 8MHz) = 31us
	
	// Enable ADC
	ADC1->CR2 |= ADC_CR2_ADON;
	
	// Wait ADC
  while((ADC1->CR2 & ADC_CR2_ADON) == 0);
	
	// Start conversion
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

void AnalogIn :: adc(void)
{	
	// Enable ADC1 clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	// Clear configuration registers
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	
	// Default:
	// - Scan mode: enabled
	// - Data alignment: right
	// - Data resolution: 12 bits
	// - DMA access : enabled
	
	// Scan mode: enabled
	ADC1->CR1 |= ADC_CR1_SCAN;
	
	// End Of Sequence conversion Interrupt (EOSI): enabled
	ADC1->CR1 |= ADC_CR1_EOSIE;
	
	// Conversion mode: continuous
	ADC1->CR2 |= ADC_CR2_CONT;
	
	// DMA access: enable (only ADC1)
	ADC1->CR2 |= ADC_CR2_DMA;
	
	// ADC start: software event
	ADC1->CR2 |= (ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2);
}

void AnalogIn :: dma(void)
{
	// Disable DMA1 channel
	DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR_EN);
	
	// Enable DMA clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	// Clear DMA1 channel1 configuration register
	DMA1_Channel1->CCR = 0;
	
	// Channel priority: high
	DMA1_Channel1->CCR |= DMA_CCR_PL_1;
	
	// Memory size: 16 bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
	
	// Peripheral size: 16 bits
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
	
	// Memory increment method: enabled
	DMA1_Channel1->CCR |= DMA_CCR_MINC;
	
	// Circular mode: enabled
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	
	// Data transfer direction: peripheral to memory
	
	// Number of data to transfer
	DMA1_Channel1->CNDTR = (uint32_t)m_ranks;
	
	// Peripheral data register address
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	
	// Memory address to which the data will be written
	DMA1_Channel1->CMAR = (uint32_t)&m_value[0];
	
	// Enable DMA1 channel1
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}

uint8_t AnalogIn :: channel(PinName pin)
{
	uint32_t port = pin & 0xFFFFFF00;
	uint8_t in = (uint8_t)pin;
	uint8_t channel = 0;
	
	switch(port)
	{
		case GPIOA_BASE: channel = (in + ADC_CHANNEL_GPIOA_OFFSET) ; break;
		case GPIOB_BASE: channel = (in + ADC_CHANNEL_GPIOB_OFFSET); break;
		case GPIOC_BASE: channel = (in + ADC_CHANNEL_GPIOC_OFFSET); break;
		default: break;
	}
	
	return channel;
}

uint16_t AnalogIn :: read_b(void)
{
	// End of sequence conversion (EOS = EOC) ?
	while((ADC1->SR & ADC_SR_EOC) == 0); 
    
	// Clear flags
	ADC1->SR |= ADC_SR_EOC;
    
	return m_value[m_rank];
}

uint16_t AnalogIn :: read(void)
{
	return m_value[m_rank];
}

AnalogIn :: operator uint16_t()
{
	return m_value[m_rank];
}
