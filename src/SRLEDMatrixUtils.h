//     Shift Register LED Matrix Project - Utils
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
// 
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
#ifndef __SRLEDMATRIXUTILS_H__
#define __SRLEDMATRIXUTILS_H__
#include <Arduino.h>

namespace SRLEDMatrixUtils {
	void * memcpy_smart(
				void * dest,
				const void * src,
				size_t n,
				bool isSrcProgMem
			);

	float fmod(float value, float modulo);

}

//
// Platform specific defines and functions
//

#if defined(__AVR_ATmega8535__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
//Register names
#define OCR0A   OCR0
#define EIMSK   GICR
#define EIFR    GIFR
#define TIMSK0  TIMSK
#define TIMSK1  TIMSK
#define TIMSK2  TIMSK
#define TIFR0   TIFR
#define TIFR1   TIFR
#define TIFR2   TIFR
#define SPMCSR  SPMCR
#define SMCR    MCUCR
#define EICRA   MCUCR
#define MCUSR   MCUCSR
#define TCCR0A  TCCR0
#define ADCSRB  SFIOR
#define TCCR2A  TCCR2
#define TCCR2B  TCCR2
#define OCR2A   OCR2
#define WDTCSR  WDTCR
#define UBRR0H  UBRRH
#define UCSR0C  UCSRC
#define UDR0    UDR
#define UCSR0A  UCSRA
#define UCSR0B  UCSRB
#define UBRR0L  UBRRL

//Bit names
#define OCIE2A  OCIE2
#define ICIE1   TICIE1
#define OCIE0A  OCIE0
#define OCF2A   OCF2
#define OCF0A   OCF0
#define ISC20   ISC2
#define FOC0A   FOC0
#define COM0A1  COM01
#define COM0A0  COM00
#define PSRASY  PSR2
#define PSRSYNC PSR10
#define FOC2A   FOC2
#define COM2A1  COM21
#define COM2A0  COM20
#define OCR2AUB OCR2UB
#define TCR2AUB TCR2UB
#define UMSEL00 UMSEL
#define UPM10   UPM1
#define UPM00   UPM0
#define USBS0   USBS
#define UCSZ00  UCSZ1
#define UCSZ10  UCSZ2
#define UCPOL0  UCPOL
#define RXC0    RXC
#define TXC0    TXC
#define UDRE0   UDRE
#define FE0     FE
#define DOR0    DOR
#define UPE0    PE
#define U2X0    U2X
#define MPCM0   MPCM
#define RXCIE0  RXCIE
#define TXCIE0  TXCIE
#define UDRIE0  UDRIE
#define RXEN0   RXEN
#define TXEN0   TXEN
#define UCSZ02  UCSZ2
#define RXB80   RXB8
#define TXB80   TXB8

#ifndef WGM22
#define WGM22 3
#endif

//Interrupt vectors
#define TIMER2_COMPA_vect			TIMER2_COMP_vect
#define TIMER2_COMPA_vect_num	TIMER2_COMP_vect_num
#define TIMER0_COMPA_vect		 	TIMER0_COMP_vect
#define TIMER0_COMPA_vect_num	TIMER0_COMP_vect_num 
#define USART0_RX_vect				USART_RXC_vect
#define USART0_RX_vect_num		USART_RXC_vect_num
#define USART0_UDRE_vect			USART_UDRE_vect
#define USART0_UDRE_vect_num	USART_UDRE_vect_num
#define USART0_TX_vect				USART_TXC_vect
#define USART0_TX_vect_num		USART_TXC_vect_num
#define ANALOG_COMP_vect			ANA_COMP_vect
#define ANALOG_COMP_vect_num	ANA_COMP_vect_num
#endif // defined(__AVR_ATmega8535__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)

#endif // __RGBLEDMATRIXUTILS_H__

