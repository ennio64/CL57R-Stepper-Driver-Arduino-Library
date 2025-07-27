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

  delay(100);

  motorA.begin();

  delay(100);

  motorA.printDiagnostics();
  delay(2000);
  motorA.printMotorSettings();
  delay(2000);
  motorA.printMotorStatus();
  delay(2000);
  motorA.printMotionStatus();
  delay(2000);
  motorA.printIOStatus();
  delay(2000);
  dumpRegisters(motorA);
}

void loop() {
  delay(10);
}

void dumpRegisters(CL57RStepper& stepper) {
  Serial.println("\n📦 Dump registri ");

  for (uint16_t addr = 0x0000; addr <= 0x00DF; addr++) {
    uint16_t val = 0;
    if (stepper.readRegister(addr, val)) {
      Serial.print("0x");
      if (addr < 0x0100) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.print(" → HEX: 0x");
      if (val < 0x0010) Serial.print("000");
      else if (val < 0x0100) Serial.print("00");
      else if (val < 0x1000) Serial.print("0");
      Serial.print(val, HEX);
      Serial.print(" | DEC: ");
      Serial.println(val);
    } else {
      Serial.print("0x");
      if (addr < 0x0100) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println(" → ❌ Errore lettura");
    }
  }
}
