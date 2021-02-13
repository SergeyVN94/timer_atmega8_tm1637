#pragma once

#include "base.hpp"

#define SIGNAL_TIME 5

uint8_t signalTimer = SIGNAL_TIME;

const Pin signalPin = {
  (1 << PC4),
  &DDRC,
  &PINC,
  &PORTC
};

void initSignal() {
  *signalPin.regDDR |= signalPin.bit;
}

bool signalEnable() {
  return bool((*signalPin.regPIN) & signalPin.bit);
}

void stopSignal() {
  signalTimer = 0;
  if ((*signalPin.regPIN) & signalPin.bit) *signalPin.regPORT &= ~signalPin.bit;
}

void startSignal() {
  signalTimer = SIGNAL_TIME;
  *signalPin.regPORT |= signalPin.bit;
}

void handleTickForSignal() {
  if (signalTimer > 0) {
    signalTimer -= 1;
    if (signalTimer == 0) stopSignal();
  } signalTimer -= 1;
}
