#include "base.hpp"
#include <avr/interrupt.h>
#include "timer.cpp"
#include "tm1637.cpp"
#include "button.cpp"
#include "Timer.cpp"
#include "signal.cpp"

/*
DDRx	Настройка разрядов порта x на вход или выход.
PORTx	Управление состоянием выходов порта x (если соответствующий разряд настроен как выход), или подключением внутреннего pull-up резистора (если соответствующий разряд настроен как вход).
PINx	Чтение логических уровней разрядов порта x.
*/

#define FLAG_TIMER_1 (1 << 0)
#define FLAG_TIMER_2 (1 << 1)
#define FLAG_DOTS_OF_DISPLAY_ON (1 << 2)

#define CHECK_FLAG(FLAG) (flags & FLAG)
#define FLAG_ON(FLAG) (flags |= FLAG)
#define FLAG_OFF(FLAG) (flags &= ~FLAG)

void handlerTimeEnd();
void handlerTimeChange(uint32_t seconds);

volatile uint8_t flags = 0;
Timer timer(&handlerTimeChange, &handlerTimeEnd);

ISR(TIMER1_COMPA_vect) {
  FLAG_ON(FLAG_TIMER_1);
}

ISR(TIMER2_COMP_vect) {
  FLAG_ON(FLAG_TIMER_2);
}

void handlerTimeChange(uint32_t seconds) {
  tm1637PrintTime(seconds, CHECK_FLAG(FLAG_DOTS_OF_DISPLAY_ON));

  if (timer.getState() == STATE_RUNNING) {
    if (CHECK_FLAG(FLAG_DOTS_OF_DISPLAY_ON)) FLAG_OFF(FLAG_DOTS_OF_DISPLAY_ON);
    else FLAG_ON(FLAG_DOTS_OF_DISPLAY_ON);
  }
}

void handlerTimeEnd() {
  FLAG_ON(FLAG_DOTS_OF_DISPLAY_ON);
  startSignal();
}

void handleButtonPushdown(uint8_t button) {
  if (signalEnable()) stopSignal();
}

void handleButtonPushup(uint8_t button) {
  timer.handlePressedButton(button);
}

void handleLongPress(uint8_t button) {
  timer.handlePressedButton(button);
}

int main() {
  tm1637Init();
  initTimer1();
  initTimer2();
  initButtons();
  initSignal();

  tm1637PrintSymbol(1, 0);
  tm1637PrintSymbol(2, tm1637Numbers[0]);
  tm1637PrintSymbol(3, tm1637Numbers[0]);
  tm1637PrintSymbol(4, tm1637Numbers[0]);

  while (1) {
    if (CHECK_FLAG(FLAG_TIMER_1)) {
      FLAG_OFF(FLAG_TIMER_1);
      timer.handleTick();
      handleTickForSignal();
    }

    if (CHECK_FLAG(FLAG_TIMER_2)) {
      FLAG_OFF(FLAG_TIMER_2);
      updateButtonsState();
    }

    updateButtonsCounters();
  }
}
