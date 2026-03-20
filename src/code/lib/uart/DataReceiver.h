/**
 * @file DataReviever.h
 * @brief Logic for receiving and processing UART data
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <Arduino.h>

struct DataReceiver {
  uint8_t data[2];
  uint8_t expectedBytes;
  uint8_t count;
  bool waiting;
  bool ready;

  DataReceiver(uint8_t numBytes)
    : expectedBytes(numBytes), count(0), waiting(true), ready(false) {}

  void feed(uint8_t byte) {
    ready = false;

    if (waiting) {
      if (byte == 255) waiting = false; //start to recieve data
      return;
    }

    data[count++] = byte; //save data in array

    if (count == expectedBytes) {
      ready = true;
      count = 0;
      waiting = true;
    }
  }
};

#endif