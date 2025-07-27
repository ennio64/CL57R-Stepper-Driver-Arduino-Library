#ifndef RS485BUS_H
#define RS485BUS_H

#include <Arduino.h>

class RS485Bus {
public:
    RS485Bus(HardwareSerial* serial, int derePin = -1);

    void begin(unsigned long baud = 9600);
    void enableDebug(bool enabled);

    bool send(const uint8_t* data, size_t length);
    bool receive(uint8_t* buffer, size_t expected);

    bool writeSingle(uint8_t id, uint16_t reg, uint16_t value);
    bool writeMultiple(uint8_t id, uint16_t startReg, const uint16_t* values, uint8_t count);
    bool readHolding(uint8_t id, uint16_t startReg, uint16_t* buffer, uint8_t count);

private:
    HardwareSerial* serial;
    int derePin;

    void setTransmit(bool on);
    uint16_t ModbusCRC16(const uint8_t* data, uint8_t length);
};

#endif // RS485BUS_H
