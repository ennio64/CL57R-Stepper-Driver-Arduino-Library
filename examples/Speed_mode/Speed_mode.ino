#include <RS485Bus.h>
#include "CL57RStepper.h"
#include "CL57RMacros.h"

RS485Bus rs485(&Serial2);         // modulo RS485 con Rx Tx (senza DE/RE)
CL57RStepper_(motor, &rs485, 1);  // ID Modbus = 1

#define RX_PIN 17
#define TX_PIN 16

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  rs485.begin(115200);
  rs485.enableDebug(false);  //true abilità il debug sul bus RS485

  delay(100);

  motor.begin();

  // 🔍 Verifica presenza motore
  if (!motor.isConnected()) {
    Serial.println("❌ Driver CL57R non rilevato!");
    return;
  }

  motor.clearAlarm();

  // 🔧 Configurazione profilo motore
  motor.writeRegister(REG_LOOP_MODE, 0);            // Open loop
  motor.writeRegister(REG_CURRENT_BASE_RATIO, 2);   // 200 mAmpere
  motor.writeRegister(REG_CURRENT_OPEN_RATIO, 30);  // 3 Ampere
  motor.writeRegister(REG_MICROSTEP, 51200);
  // 🔧 Configurazione profilo velocità
  motor.writeRegister(REG_VSTART, 5);
  motor.writeRegister(REG_VMAX, 400);
  motor.writeRegister(REG_ACCEL_TIME, 150);
  motor.writeRegister(REG_DECEL_TIME, 150);

  // ▶️ Avvio rotazione continua
  motor.startSpeed();

  // 🔍 Monitor fase attiva (5s)
  monitorRPM(10, 1000, "Accelerazione + Velocità regime");

  // 🛑 Stop graduale
  motor.stopSmooth();
  delay(20);
  motor.printMotorStatus();
  // 🔻 Monitor fase decelerazione (1s)
  monitorRPM(10, 1000, "Decelerazione");

  delay(1000);
  motor.printMotorStatus();
  delay(100);
  motor.resetAbsolutePosition();  // Reset posizione attuale a zero
  delay(100);

  changeMode();

  motor.printMotorStatus();
  delay(1000);

  motor.moveToRelativePosition(51200);
  motor.printMotorStatus();

  delay(2500);

  // ░ Torna 0 con movimento a posizione assoluta
  motor.moveToAbsolutePosition(0);
  motor.printMotorStatus();
  delay(2500);

  motor.printMotorStatus();
  delay(200);

  motor.printIOStatus();
}

void loop() {
  // Vuoto
}

void monitorRPM(uint16_t intervalMs, uint16_t durationMs, const char* fase) {
  Serial.print("📊 Monitor RPM → ");
  Serial.println(fase);
  uint16_t rpm = 0;
  uint16_t steps = durationMs / intervalMs;
  for (int i = 0; i < steps; i++) {
    motor.readRegister(REG_SPEED_ACTUAL, rpm);  // Registro 0x0009
    Serial.print("⏱️ ");
    Serial.print(i * intervalMs);
    Serial.print(" ms → ");
    Serial.print(rpm);
    Serial.println(" rpm");
    delay(intervalMs);
  }
}

void changeMode() {
  // necessario per passare da speed mode a position mode altrimenti il bit0 del registro 0x03 non si resetta (indica se in posizione)
  motor.disableMotor();
  delay(30);
  motor.enableMotor();
  delay(30);
}
