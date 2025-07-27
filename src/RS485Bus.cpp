#include "RS485Bus.h"

// ✳️ Abilita/disabilita debug seriale
static bool debugEnabled = false;

RS485Bus::RS485Bus(HardwareSerial* serial, int derePin)
    : serial(serial), derePin(derePin) {}

void RS485Bus::enableDebug(bool enabled) {
    debugEnabled = enabled;
}

void RS485Bus::begin(unsigned long baud) {
    if (derePin >= 0) {
        pinMode(derePin, OUTPUT);
        digitalWrite(derePin, LOW);
    }
    serial->begin(baud);
}

void RS485Bus::setTransmit(bool on) {
    if (derePin >= 0) digitalWrite(derePin, on ? HIGH : LOW);
    delayMicroseconds(50); // stabilizzazione
}

uint16_t RS485Bus::ModbusCRC16(const uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

bool RS485Bus::send(const uint8_t* data, size_t length) {
    setTransmit(true);
    serial->write(data, length);
    serial->flush();
    setTransmit(false);

    if (debugEnabled) {
        Serial.print("TX [");
        for (size_t i = 0; i < length; i++) {
            if (data[i] < 16) Serial.print("0");
            Serial.print(data[i], HEX);
            Serial.print(i < length - 1 ? " " : "");
        }
        Serial.println("]");
    }

    return true;
}

bool RS485Bus::receive(uint8_t* buffer, size_t expected) {
    unsigned long start = millis();
    size_t count = 0;
    while (millis() - start < 100 && count < expected) {
        if (serial->available()) {
            buffer[count++] = serial->read();
        }
    }

    if (debugEnabled && count > 0) {
        Serial.print("RX [");
        for (size_t i = 0; i < count; i++) {
            if (buffer[i] < 16) Serial.print("0");
            Serial.print(buffer[i], HEX);
            Serial.print(i < count - 1 ? " " : "");
        }
        Serial.println("]");
    }

    return count >= expected;
}

// ---------------- Modbus: Write Single Register (0x06) ----------------

bool RS485Bus::writeSingle(uint8_t id, uint16_t reg, uint16_t value) {
    uint8_t frame[8];
    frame[0] = id;
    frame[1] = 0x06;
    frame[2] = reg >> 8;
    frame[3] = reg & 0xFF;
    frame[4] = value >> 8;
    frame[5] = value & 0xFF;
    uint16_t crc = ModbusCRC16(frame, 6);
    frame[6] = crc & 0xFF;
    frame[7] = crc >> 8;
    return send(frame, 8) && receive(frame, 8);
}

// ---------------- Modbus: Write Multiple Registers (0x10) ----------------

bool RS485Bus::writeMultiple(uint8_t id, uint16_t startReg, const uint16_t* values, uint8_t count) {
    uint8_t frame[256];
    frame[0] = id;
    frame[1] = 0x10;
    frame[2] = startReg >> 8;
    frame[3] = startReg & 0xFF;
    frame[4] = 0x00;
    frame[5] = count;
    frame[6] = count * 2;

    for (uint8_t i = 0; i < count; i++) {
        frame[7 + i * 2] = values[i] >> 8;
        frame[8 + i * 2] = values[i] & 0xFF;
    }

    uint16_t crc = ModbusCRC16(frame, 7 + count * 2);
    frame[7 + count * 2] = crc & 0xFF;
    frame[8 + count * 2] = crc >> 8;

    size_t len = 9 + count * 2;
    return send(frame, len) && receive(frame, 8);
}

// ---------------- Modbus: Read Holding Registers (0x03) ----------------

bool RS485Bus::readHolding(uint8_t id, uint16_t startReg, uint16_t* buffer, uint8_t count) {
    uint8_t frame[8];
    frame[0] = id;
    frame[1] = 0x03;
    frame[2] = startReg >> 8;
    frame[3] = startReg & 0xFF;
    frame[4] = 0x00;
    frame[5] = count;
    uint16_t crc = ModbusCRC16(frame, 6);
    frame[6] = crc & 0xFF;
    frame[7] = crc >> 8;

    if (!send(frame, 8)) return false;

    uint8_t response[5 + count * 2];
    if (!receive(response, sizeof(response))) return false;

    for (uint8_t i = 0; i < count; i++) {
        buffer[i] = (response[3 + i * 2] << 8) | response[4 + i * 2];
    }
    return true;
}
