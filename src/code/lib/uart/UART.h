/**
 * @file UART.h
 * @brief Communication between controllers using uart protocol
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "DataReceiver.h"

struct IRBoard   {};
struct LineBoard {};

class UART {
  public:
    int irAngle = 500;
    int irClose = 0;
    int lineAngle = 500;

    DataReceiver irReceiver   = DataReceiver(2); //angle and distance
    DataReceiver lineReceiver = DataReceiver(1); // angle

    //main
    UART(HardwareSerial& irPort, HardwareSerial& linePort)
      : _irSerial(&irPort), _lineSerial(&linePort) {}

    //ir
    UART(HardwareSerial& port, IRBoard)
      : _irSerial(&port), _lineSerial(nullptr) {}

    //line
    UART(HardwareSerial& port, LineBoard)
      : _irSerial(nullptr), _lineSerial(&port) {}

    void begin(long baud) {
      if(_irSerial) _irSerial->begin(baud);
      //if(_lineSerial) _lineSerial->begin(baud);
    }

    void receive() {
      while (_irSerial->available()) {
        irReceiver.feed(_irSerial->read());
        if (irReceiver.ready) {
          irAngle = irReceiver.data[0];
          irClose = irReceiver.data[1];
        }
      }

/*       while (_lineSerial->available()) {
        lineReceiver.feed(_lineSerial->read());
        if (lineReceiver.ready) {
          lineAngle = lineReceiver.data[0];
        }
      } */
    }

    void sendIR(uint8_t angle, uint8_t distance) {
      if(_irSerial == nullptr) return;
      _irSerial->write(255);
      _irSerial->write(angle);
      _irSerial->write(distance);
    }

    void sendLine(uint8_t angle) {
      if(_lineSerial == nullptr) return;
      _lineSerial->write(255);
      _lineSerial->write(angle);
    }

  private:
    HardwareSerial* _irSerial;
    HardwareSerial* _lineSerial;
};

#endif