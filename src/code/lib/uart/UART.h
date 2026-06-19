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
    int irDistance = 254;
    int lineAngle = 500;
    int cameraAngle = 200;

    DataReceiver irReceiver   = DataReceiver(2, 255, 100); //angle and distance
    DataReceiver lineReceiver = DataReceiver(1, 255, 100); // angle
    DataReceiver cameraReceiver = DataReceiver(1, 255, 100); // approximate angle

    //main
    UART(HardwareSerial& irPort, HardwareSerial& linePort, HardwareSerial& cameraPort)
      : _irSerial(&irPort), _lineSerial(&linePort), _cameraSerial(&cameraPort) {}

    //ir
    UART(HardwareSerial& port, IRBoard)
      : _irSerial(&port), _lineSerial(nullptr), _cameraSerial(nullptr) {}

    //line
    UART(HardwareSerial& port, LineBoard)
      : _irSerial(nullptr), _lineSerial(&port), _cameraSerial(nullptr) {}

    void beginIR(long baud) {
      if(_irSerial) _irSerial->begin(baud);
    }

    void beginLine(long baud) {
      if(_lineSerial) _lineSerial->begin(baud);
    }

     void beginCamera(long baud) {
      if(_cameraSerial) _cameraSerial->begin(baud);
    }

    void receiveIR() {
      if(_irSerial) irReceiver.tick(*_irSerial);

      if (irReceiver.ready) {
        irAngle = irReceiver.data[0];
        irDistance = irReceiver.data[1];
      }
    }

    void receiveLine() {
      if(_lineSerial) lineReceiver.tick(*_lineSerial);
      if (lineReceiver.ready) {
        lineAngle = lineReceiver.data[0];
      }
    }

     void receiveCamera() {
      if(_cameraSerial) cameraReceiver.tick(*_cameraSerial);
      if (cameraReceiver.ready) {
        cameraAngle = cameraReceiver.data[0];
      }
    }

    void sendIR(uint8_t angle, uint8_t distance) {
      if (!_irSerial || !_irSerial->available()) return;
      uint8_t received = 0;
      while (_irSerial->available()) received = _irSerial->read(); // drain FIFO
      if (received == 255) {
        _irSerial->write(angle);
        _irSerial->write(distance);
      }
    }

    
    void sendLine(uint8_t angle) {
      if (!_lineSerial || !_lineSerial->available()) return;
      uint8_t received = 0;
      while (_lineSerial->available()) received = _lineSerial->read();
      if (received == 255) {
        _lineSerial->write(angle);
      }
    }

  private:
    HardwareSerial* _irSerial;
    HardwareSerial* _lineSerial;
    HardwareSerial* _cameraSerial;
};

#endif