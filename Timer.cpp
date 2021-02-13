#pragma once

#include <avr/eeprom.h>

#include "base.hpp"
#include "button.cpp"

#define SECONDS_IN_HOUR uint32_t(3600)
#define SECONDS_IN_MINUTES 60
#define TIMER_MAX_SECONDS uint32_t(359940)
#define EEPROM_ADDRES 0x42

// состояния таймера
#define STATE_SET_HOURS 0
#define STATE_SET_MINUTS 2
#define STATE_RUNNING 3
#define STATE_PAUSE 4

class Timer {
 private:
  uint32_t seconds;
  uint8_t hours;
  uint8_t minutes;
  uint8_t state;
  void (*handlerTimeChange)(uint32_t);
  void (*handlerTimerEnd)();
  // void saveTime();
  // void readTime();

 public:
  Timer(void (*handlerTimeChange)(uint32_t), void (*handlerTimerEnd)());
  ~Timer();

  void handlePressedButton(uint8_t button);
  void handleTick();
  void reset();
  uint8_t getState();
};

Timer::Timer(void (*handlerTimeChange)(uint32_t), void (*handlerTimerEnd)())
    : state(STATE_SET_HOURS),
      handlerTimeChange(handlerTimeChange),
      handlerTimerEnd(handlerTimerEnd) {
  // readTime();
  (*handlerTimeChange)(seconds);
}

Timer::~Timer() {}

uint8_t Timer::getState() {
  return state;
}

void Timer::reset() {
  state = STATE_SET_HOURS;
  // readTime();
  (*handlerTimeChange)(seconds);
}

void Timer::handlePressedButton(uint8_t button) {
  if (button == BTN_SET) {
    if (state == STATE_SET_HOURS) state = STATE_SET_MINUTS;
    else if (state == STATE_SET_MINUTS) state = (seconds > 0) ? STATE_RUNNING : STATE_SET_HOURS;
    else if (state == STATE_RUNNING || state == STATE_PAUSE) {
      reset();
      (*handlerTimerEnd)();
    } 
  }

  if (button == BTN_PLUS) {
    if (state == STATE_SET_HOURS) {
      if (seconds < uint32_t(99 * SECONDS_IN_HOUR)) seconds += SECONDS_IN_HOUR;
      else seconds = 0;
    } else if (state == STATE_SET_MINUTS) {
      if (seconds < TIMER_MAX_SECONDS) seconds += SECONDS_IN_MINUTES;
      else seconds = 0;
    } else if (state == STATE_RUNNING) state = STATE_PAUSE;
    else if (state == STATE_PAUSE) state = STATE_RUNNING;
  }

  if (button == BTN_MINUS) {
    if (state == STATE_SET_HOURS) {
      if (seconds > 0) seconds -= SECONDS_IN_HOUR;
      else seconds = uint32_t(99 * SECONDS_IN_HOUR);
    } else if (state == STATE_SET_MINUTS) {
      if (seconds > 0) seconds -= SECONDS_IN_MINUTES;
      else seconds = TIMER_MAX_SECONDS;
    } else if (state == STATE_RUNNING || state == STATE_PAUSE) {
      reset();
      (*handlerTimerEnd)();
    } 
  }

  (*handlerTimeChange)(seconds);
}

void Timer::handleTick() {
  if (state == STATE_RUNNING && seconds > 0) {
    seconds -= 1;
    (*handlerTimeChange)(seconds);

    if (seconds == 0) {
      reset();
      (*handlerTimerEnd)();
    } 
  }
}

// void Timer::saveTime() {
//   uint8_t hours = (seconds / SECONDS_IN_HOUR);
//   uint8_t minutes = ((seconds % SECONDS_IN_HOUR) / SECONDS_IN_MINUTES);
// 
//   eeprom_write_byte((uint8_t *)EEPROM_ADDRES, hours);
//   eeprom_write_byte((uint8_t *)EEPROM_ADDRES + 2, minutes);
// }

// void Timer::readTime() {
//   seconds = eeprom_read_byte((uint8_t *)EEPROM_ADDRES) * SECONDS_IN_HOUR;
//   seconds += eeprom_read_byte((uint8_t *)EEPROM_ADDRES + 2) * SECONDS_IN_MINUTES;
// }
