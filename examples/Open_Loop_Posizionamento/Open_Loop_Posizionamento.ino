#include <RS485Bus.h>
#include "CL57RStepper.h"
#include "CL57RMacros.h"

RS485Bus rs485(&Serial2);  // modulo RS485 con Rx Tx (senza DE/RE)
CL57RStepper_(motorA, &rs485, 1);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);  // RX, TX
  rs485.begin(115200);
  rs485.enableDebug(false);

  delay(200);

  motorA.begin();

  delay(100);

  if (motorA.isConnected()) {
    Serial.println("✅ Driver Motore rilevato!");
  } else {
    Serial.println("❌ Nessun motore trovato o comunicazione fallita.");
    return;
  }

  // ░ Configurazione motore
  motorA.clearAlarm();
  motorA.writeRegister(REG_LOOP_MODE, 0);            // 0=open, 1=closed
  motorA.writeRegister(REG_CURRENT_BASE_RATIO, 2);   // 200 mAmpere
  motorA.writeRegister(REG_CURRENT_OPEN_RATIO, 30);  // 3 Ampere
  motorA.writeRegister(REG_MICROSTEP, 51200);
  motorA.writeRegister(REG_VSTART, 5);
  motorA.writeRegister(REG_VMAX, 400);
  motorA.writeRegister(REG_ACCEL_TIME, 150);
  motorA.writeRegister(REG_DECEL_TIME, 150);

  motorA.printMotorSettings();
  motorA.printMotorStatus();
  delay(2500);

  // ░ 1 giro con movimento a posizione relativa
  motorA.moveToRelativePosition(51200);
  motorA.printMotorStatus();
  delay(2500);
  // ░ Aspetta che sia fermo
  motorA.printMotorStatus();
  delay(2500);
  // ░ Torna 0 con movimento a posizione assoluta
  motorA.moveToAbsolutePosition(0);
  motorA.printMotorStatus();
  delay(2500);
  // ░ Aspetta che sia fermo
  motorA.printMotorStatus();
}

void loop() {
  delay(10);
}
