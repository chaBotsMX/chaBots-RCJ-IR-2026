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

class UART {
  public:
    DataReceiver irReceiver   = DataReceiver(2); //angle and distance
    DataReceiver lineReceiver = DataReceiver(2); // angle and flags

    int irAngle = 500;
    int irDistance = 100;
    int lineAngle = 500;
    int lineFlag = 0;

    UART(HardwareSerial& irPort, HardwareSerial& linePort)
      : _irSerial(irPort), _lineSerial(linePort) {}

    void begin(long baud) {
      _irSerial.begin(baud);
      _lineSerial.begin(baud);
    }

    void receive() {
      while (_irSerial.available()) {
        irReceiver.feed(_irSerial.read());
        if (irReceiver.ready) {
          irAngle = irReceiver.data[0];
          irDistance = irReceiver.data[1];
        }
      }

      while (_lineSerial.available()) {
        lineReceiver.feed(_lineSerial.read());
        if (lineReceiver.ready) {
          lineAngle = lineReceiver.data[0];
          lineFlag = lineReceiver.data[1];
        }
      }
    }

    void sendIR(uint8_t angle, uint8_t distance) {
      _irSerial.write(255);
      _irSerial.write(angle);
      _irSerial.write(distance);
    }

    void sendLine(uint8_t angle, uint8_t flag) {
      _lineSerial.write(255);
      _lineSerial.write(angle);
      _lineSerial.write(flag);
    }

  private:
    HardwareSerial& _irSerial;
    HardwareSerial& _lineSerial;
};

#endif