#include <RS485Bus.h>
#include "CL57RStepper.h"
#include "CL57RMacros.h"

RS485Bus rs485(&Serial2);  // modulo RS485 con Rx Tx (senza DE/RE)
CL57RStepper_(motorA, &rs485, 1);
CL57RStepper_(motorB, &rs485, 2);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);  // RX, TX
  rs485.begin(115200);
  rs485.enableDebug(false);

  delay(200);

  motorA.begin();
  delay(100);
  motorB.begin();
  delay(100);

  if (motorA.isConnected()) {
    Serial.println("✅ Driver Motore A rilevato!");
  } else {
    Serial.println("❌ Nessun motore trovato o comunicazione fallita.");
    return;
  }
  delay(100);
  if (motorB.isConnected()) {
    Serial.println("✅ Driver Motore B rilevato!");
  } else {
    Serial.println("❌ Nessun motore trovato o comunicazione fallita.");
    return;
  }

  // ░ Configurazione motore A
  motorA.clearAlarm();
  motorA.writeRegister(REG_LOOP_MODE, 1);                   // 0=open, 1=closed
  motorA.writeRegister(REG_ENCODER_RESOLUTION, 0);          // 0=1000, 1=2500
  motorA.writeRegister(REG_TRACKING_ERROR_THRESHOLD, 400);  // (400-65535)
  motorA.writeRegister(REG_CURRENT_BASE_RATIO, 2);          // 200 mAmpere
  motorA.writeRegister(REG_CURRENT_CLOSED_RATIO, 30);       // 3 Ampere
  motorA.writeRegister(REG_MICROSTEP, 1000);
  motorA.writeRegister(REG_VSTART, 5);
  motorA.writeRegister(REG_VMAX, 400);
  motorA.writeRegister(REG_ACCEL_TIME, 150);
  motorA.writeRegister(REG_DECEL_TIME, 150);

  // ░ Configurazione motore B
  motorB.clearAlarm();
  motorB.writeRegister(REG_LOOP_MODE, 0);            // 0=open, 1=closed
  motorB.writeRegister(REG_CURRENT_BASE_RATIO, 2);   // 200 mAmpere
  motorB.writeRegister(REG_CURRENT_OPEN_RATIO, 30);  // 3 Ampere
  motorB.writeRegister(REG_MICROSTEP, 51200);
  motorB.writeRegister(REG_VSTART, 5);
  motorB.writeRegister(REG_VMAX, 400);
  motorB.writeRegister(REG_ACCEL_TIME, 150);
  motorB.writeRegister(REG_DECEL_TIME, 150);

  motorA.printMotorSettings();
  motorA.printMotorStatus();
  motorB.printMotorSettings();
  motorB.printMotorStatus();
  delay(2500);

  // ░ 1 giro con movimento a posizione relativa
  motorA.moveToRelativePosition(1000);
  motorB.moveToRelativePosition(51200);
  delay(2500);
  // ░ Aspetta che siano fermi
  motorA.printMotorStatus();
  motorB.printMotorStatus();
  delay(2500);
  // ░ Torna 0 con movimento a posizione assoluta
  motorA.moveToAbsolutePosition(0);
  motorB.moveToAbsolutePosition(0);
  delay(2500);
  // ░ Aspetta che siano fermi
  motorA.printMotorStatus();
  motorB.printMotorStatus();
}

void loop() {
  delay(10);
}
