#pragma once

#include "base.hpp"
#include <avr/interrupt.h>

#define BTN_SET 0
#define BTN_PLUS 1
#define BTN_MINUS 2

#define BTN_SET_PIN (1 << PB0)
#define BTN_PLUS_PIN (1 << PD7)
#define BTN_MINUS_PIN (1 << PD6)

#define BTN_COUNT_MAX 200
#define BTN_COUNT_HIGH_LEVEL 150
#define BTN_COUNT_LOW_LEVEL 40
#define BTN_PRESSED_COUNT 300
#define BTN_LONG_PRESS_TRIGGER_COUNT 500
#define BTN_LONG_PRESS_PERIOD_COUNT 100

#define BTN_PRESSED true
#define BTN_NOT_PRESSED false

void handleButtonPushdown(uint8_t button);
void handleButtonPushup(uint8_t button);
void handleLongPress(uint8_t button);

struct Button {
  uint8_t index;
  Pin pin;
  bool state = BTN_NOT_PRESSED;
  uint8_t counter = 0;
  uint16_t longPressTriggerCounter = BTN_LONG_PRESS_TRIGGER_COUNT; // счетчик до срабатывания долгого нажатия
  uint16_t longPressPeriodCounter = 0; // счетчик между срабатываниями долгого нажатия
};

Button buttonsState[] = {
  { 0, { (1 << PB0), &DDRB, &PINB, &PORTB } },
  { 1, { (1 << PD7), &DDRD, &PIND, &PORTD } },
  { 2, { (1 << PD6), &DDRD, &PIND, &PORTD } },
};

void initButtons() {
  for (uint8_t i = 0; i < 3; i += 1) {
    Button *btn = &buttonsState[i];
    (*buttonsState[i].pin.regDDR) &= ~btn->pin.bit;
  }
}

void updateButtonsCounters() {
  for (uint8_t i = 0; i < 3; i += 1) {
    Button *btn = &buttonsState[i];
    bool pinHIGH = (*btn->pin.regPIN) & btn->pin.bit;

    if (pinHIGH && (btn->counter < BTN_COUNT_MAX)) btn->counter += 1;
    if (!pinHIGH && (btn->counter > 0)) btn->counter -= 1;
  }
}

void updateButtonsState() {
  for (uint8_t i = 0; i < 3; i += 1) {
    Button *btn = &buttonsState[i];
    bool newBtnState = btn->state;

    if (btn->counter > BTN_COUNT_HIGH_LEVEL) newBtnState = BTN_PRESSED;
    if (btn->counter < BTN_COUNT_LOW_LEVEL) newBtnState = BTN_NOT_PRESSED;

    if (newBtnState != btn->state) {
      btn->state = newBtnState;
      if (newBtnState == BTN_PRESSED) handleButtonPushdown(i);
      if (newBtnState == BTN_NOT_PRESSED) {
        handleButtonPushup(i);
        btn->longPressPeriodCounter = 0;
        btn->longPressTriggerCounter = BTN_LONG_PRESS_TRIGGER_COUNT;
      }
    } else if (btn->state == BTN_PRESSED) {
      if (btn->longPressTriggerCounter) {
        btn->longPressTriggerCounter -= 1;
      } else if (btn->longPressPeriodCounter) {
        btn->longPressPeriodCounter -= 1;
      } else {
        handleLongPress(i);
        btn->longPressPeriodCounter = BTN_LONG_PRESS_PERIOD_COUNT;
      }
    }
  }
}
  