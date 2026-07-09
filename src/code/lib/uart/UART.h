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
struct XIAO {};

class UART {
  public:
    int irAngle = 500;
    int irDistance = 254;
    int lineAngle = 500;
    int cameraAngle = 254;
    int cameraDistance = 254;
    int cameraConfidence = 254;
    int distanceX = 254;
    int distanceY = 254;

    DataReceiver irReceiver   = DataReceiver(2, 255, 100); //angle and distance
    DataReceiver lineReceiver = DataReceiver(1, 255, 100); // angle
    DataReceiver cameraReceiver = DataReceiver(3, 255, 100); // bearing, confidence
    DataReceiver distanceReceiver = DataReceiver(2, 255, 100); // x and y

    //main
    UART(HardwareSerial& irPort, HardwareSerial& linePort, HardwareSerial& cameraPort, HardwareSerial& distancePort)
      : _irSerial(&irPort), _lineSerial(&linePort), _cameraSerial(&cameraPort), _distanceSerial(&distancePort) {}

    //ir
    UART(HardwareSerial& port, IRBoard)
      : _irSerial(&port), _lineSerial(nullptr), _cameraSerial(nullptr), _distanceSerial(nullptr) {}

    //line
    UART(HardwareSerial& port, LineBoard)
      : _irSerial(nullptr), _lineSerial(&port), _cameraSerial(nullptr), _distanceSerial(nullptr) {}

    //distance sensor
    UART(HardwareSerial& port, XIAO)
      : _irSerial(nullptr), _lineSerial(nullptr), _cameraSerial(nullptr), _distanceSerial(&port) {}

    void beginIR(long baud) {
      if(_irSerial) _irSerial->begin(baud);
    }

    void beginLine(long baud) {
      if(_lineSerial) _lineSerial->begin(baud);
    }

    void beginCamera(long baud) {
      if(_cameraSerial) _cameraSerial->begin(baud);
    }

    void beginDistance(long baud) {
      if(_distanceSerial) _distanceSerial->begin(baud);
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
        cameraDistance = cameraReceiver.data[1];
        cameraConfidence = cameraReceiver.data[2];
      }
    }

    void receiveDistance() {
      if(_distanceSerial) distanceReceiver.tick(*_distanceSerial);
      if (distanceReceiver.ready) {
        distanceX = distanceReceiver.data[0];
        distanceY = distanceReceiver.data[1];
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

    void sendDistance(uint8_t x, uint8_t y) {
      if (!_distanceSerial || !_distanceSerial->available()) return;
      uint8_t received = 0;
      while (_distanceSerial->available()) received = _distanceSerial->read();
      if (received == 255) {
        _distanceSerial->write(x);
        _distanceSerial->write(y);
      }
    }

  private:
    HardwareSerial* _irSerial;
    HardwareSerial* _lineSerial;
    HardwareSerial* _cameraSerial;
    HardwareSerial* _distanceSerial;
};

#endif