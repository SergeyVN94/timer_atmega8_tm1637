#pragma once

#include "base.hpp"
#include <util/delay.h>

#define TM1637_SETTINGS 0x44
#define TM1637_BRIGHTNESS 0x8E
const uint8_t tm1637Numbers[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
};

/*
DDRx	Настройка разрядов порта x на вход или выход.
PORTx	Управление состоянием выходов порта x (если соответствующий разряд настроен как выход), или подключением внутреннего pull-up резистора (если соответствующий разряд настроен как вход).
PINx	Чтение логических уровней разрядов порта x.
*/

// Настраивать пины здесь ***********
#define TM1637_DDR DDRB
#define TM1637_PORT PORTB
#define TM1637_PIN PINB

#define TM1637_CLK PB2
#define TM1637_SDA PB1
// *********************************

#define CLK_OUTPUT (TM1637_DDR |= (1 << TM1637_CLK))
#define SDA_OUTPUT (TM1637_DDR |= (1 << TM1637_SDA))
#define SDA_INPUT (TM1637_DDR &= ~(1 << TM1637_SDA))

#define CLK_LOW (TM1637_PORT &= ~(1 << TM1637_CLK))
#define CLK_HIGH (TM1637_PORT |= (1 << TM1637_CLK))
#define SDA_LOW (TM1637_PORT &= ~(1 << TM1637_SDA))
#define SDA_HIGH (TM1637_PORT |= (1 << TM1637_SDA))

#define READ_SDA (TM1637_PIN & (1 << TM1637_SDA))

void _tm1637Start() {
  CLK_HIGH;
  SDA_HIGH;
  SDA_LOW;
}

bool _tm1637Ask() {
  SDA_INPUT;
  SDA_HIGH;

  CLK_LOW;
  CLK_HIGH;
  bool ask = READ_SDA;
  SDA_OUTPUT;
  return !ask;
}

void _tm1637Stop() {
  CLK_LOW;
  SDA_LOW;
  CLK_HIGH;
  SDA_HIGH;
}

void _tm1637WriteByte(uint8_t data) {
  for (uint8_t i = 0; i < 8; i += 1) {
    CLK_LOW;
    if (data & (1 << i)) { SDA_HIGH; }
    else { SDA_LOW; }
    CLK_HIGH;
  }
}

bool _tm1637SendCommand(uint8_t command) {
  _tm1637Start();
  _tm1637WriteByte(command);
  bool ask = _tm1637Ask();
  _tm1637Stop();
  return ask;
}

void tm1637Init() {
  CLK_OUTPUT;
  SDA_OUTPUT;
  _tm1637SendCommand(TM1637_SETTINGS);
  _tm1637SendCommand(TM1637_BRIGHTNESS);
}

bool tm1637PrintSymbol(uint8_t position, uint8_t symbol) {
  // position to address
  if (position < 1) position = 1;
  if (position > 4) position = 4;
  position--;
  position += 0xC0;

  _tm1637Start();
  _tm1637WriteByte(position);
  _tm1637Ask();
  _tm1637WriteByte(symbol);
  bool ask = _tm1637Ask();
  _tm1637Stop();
  return ask;
}

void tm1637PrintTime(uint32_t seconds, bool pointsOn = true) {
  uint16_t minutes = seconds / 60;

  const uint8_t hours = minutes / 60;
  tm1637PrintSymbol(1, (hours > 9) ? tm1637Numbers[hours / 10] : 0);
  const uint8_t hoursL = tm1637Numbers[hours % 10] | (pointsOn ? 0x80 : 0);
  tm1637PrintSymbol(2, hoursL);

  minutes = minutes % 60;
  tm1637PrintSymbol(3, tm1637Numbers[minutes / 10]);
  tm1637PrintSymbol(4, tm1637Numbers[minutes % 10]);
}
