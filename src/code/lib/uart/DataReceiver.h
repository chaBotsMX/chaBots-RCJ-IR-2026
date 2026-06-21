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
  uint8_t data[3];
  uint8_t expectedBytes;
  uint8_t requestByte;
  uint32_t timeout;
  bool ready;

  DataReceiver(uint8_t numBytes, uint8_t request, uint32_t timeoutMs = 100)
    : expectedBytes(numBytes), requestByte(request), timeout(timeoutMs), ready(false),
      _state(IDLE), _count(0), _start(0) {}

  void tick(HardwareSerial& serial) {
    ready = false;

    switch (_state) {
      case IDLE:
        while (serial.available()) {
          // Flush stale data before issuing a new request
          serial.read();
        }

        serial.write(requestByte);
        _count = 0;
        _start = millis();
        _state = RECEIVING;
        break;

      case RECEIVING:
        // timeout check
        if ((millis() - _start) > timeout) {
          _state = IDLE;
          break;
        }

        // read incoming data
        while (serial.available() && _count < expectedBytes) {
          _buf[_count++] = serial.read();
        }

        // save data if we have received the expected number of bytes
        if (_count == expectedBytes) {
          for (uint8_t i = 0; i < expectedBytes; i++) {
            data[i] = _buf[i];
          }
          ready  = true;
          _state = IDLE;
        }
        break;
    }
  }

  private:
    enum State : uint8_t { IDLE, RECEIVING };
    State   _state;
    uint8_t _count;
    uint8_t _buf[3];
    uint32_t _start;
};

#endif