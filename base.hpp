#pragma once

#define __AVR_ATmega8A__
#define F_CPU 8000000UL

#include <avr/io.h>

/*
  bit - bit in port
  regDDR - &DDRx
  regPIN - &PINx
  regPORT - &PORTx
*/
struct Pin {
  uint8_t bit;
  volatile uint8_t *regDDR;
  volatile uint8_t *regPIN;
  volatile uint8_t *regPORT;
};
