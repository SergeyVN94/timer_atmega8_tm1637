#pragma once

#include "base.hpp"
#include <avr/interrupt.h>

#define COUNT_PERIOD_OF_TIMER_2 868 // количество срабатываний в секунду 2-го таймера

void initTimer1() {
  /* нужно:
    1) включить срабатывание по сравнению с регистром (режим СТС)
    2) утановить предделитель
    3) указать регистр, с которым сравнивается
    4) записать число с которым сравнивать
    5) включить прерывания
  */
  TCCR1B |= (1 << WGM12); // режим СТС (сравнение с регистром)
  TCCR1B |= (1 << CS12); // предделитель частоты на 256

  TIMSK |= (1 << OCIE1A); // сравнение с регистрам А включение прерывания

  OCR1AH = (31250 >> 8);
  OCR1AL = (uint8_t)31250;

  // сброс таймера
  TCNT1L = 0;
  TCNT1H = 0;

  sei();  // включить прерывания
}

void initTimer2() {
  TCCR2 |= (1 << WGM21); // режим СТС (сравнение с регистром)
  TCCR2 |= (1 << CS22) | (1 << CS21) | (1 << CS20); // предделитель частоты на 1024

  TIMSK |= (1 << OCIE2);

  OCR2 = 8;
  TCNT2 = 0; // сброс таймера

  sei();  // включить прерывания
}
