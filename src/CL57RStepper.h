#ifndef CL57RSTEPPER_H
#define CL57RSTEPPER_H

#include <Arduino.h>
#include "RS485Bus.h"
#include "CL57R_registers.h"

class CL57RStepper {
public:
  CL57RStepper(RS485Bus* bus, uint8_t slaveID, const String& name = "Stepper");

  // 🟢 Inizializzazione obbligatoria
  bool begin();                            // Abilita motore + reset target
  bool isInitialized();                   // Controlla stato
  bool guardInit(const String& caller);   // Messaggio centralizzato

  // 🔧 Controllo motore
  bool enableMotor();
  bool disableMotor();
  bool clearAlarm();
  bool resetAbsolutePosition();
  bool restoreFactorySettings();

  // 🏁 Movimento
  bool moveToAbsolutePosition(int32_t target);
  bool moveToRelativePosition(int32_t offset);
  bool startSpeed();
  bool stopSmooth();
  bool stopEmergency();

  // 🧪 Lettura
  int32_t getActualPosition();
  uint16_t getStatusWord();
  uint16_t getErrorCode();
  uint16_t getWorkingMode();

  // 📋 Accesso Modbus
  bool readRegister(uint16_t reg, uint16_t& value);
  bool writeRegister(uint16_t reg, uint16_t value);
  bool writeDoubleRegister(uint16_t regH, uint16_t regL, uint32_t value);

  // 🔍 Stato logico
  bool isConnected();
  bool isMovementComplete();

  // 🖨️ Diagnostica
  void printDiagnostics();
  void printMotorSettings();
  void printMotorStatus();
  void printMotionStatus();
  void printIOStatus();

  // 🔠 Utility
  String decodeErrorCode(uint16_t code);
  String getDIFunctionName(uint16_t code);
  String getDOFunctionName(uint16_t code);

private:
  String _name = "Stepper";  // Nome predefinito per log
  RS485Bus* _bus;
  uint8_t _id;
  bool _initialized = false;
  // 🔧 Accesso diretto ai registri senza protezione
  bool writeRegisterRaw(uint16_t reg, uint16_t value);
  bool readRegisterRaw(uint16_t reg, uint16_t& value);
};

#endif
