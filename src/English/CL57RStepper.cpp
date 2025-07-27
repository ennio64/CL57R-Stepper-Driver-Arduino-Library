// CL57RStepper.cpp – with console messages translated to English

#include "CL57RStepper.h"
#include "CL57RMacros.h"

CL57RStepper::CL57RStepper(RS485Bus* bus, uint8_t slaveID, const String& name) {
  _bus = bus;
  _id = slaveID;
  _name = name;
}

bool CL57RStepper::begin() {
  Serial.print("🔓 Starting procedure ");
  Serial.print(_name);
  Serial.println(".begin()");

  bool ok = true;

  if (writeRegisterRaw(REG_ENABLE_MOTOR, 1)) {
    Serial.println("🟢 REG_ENABLE_MOTOR (0x0038) → OK [motor enabled]");
  } else {
    Serial.println("❌ Write error REG_ENABLE_MOTOR (0x0038)");
    ok = false;
  }

  if (writeRegisterRaw(REG_POS_TARGET_H, 0)) {
    Serial.println("📍 REG_POS_TARGET_H (0x0034) → OK");
  } else {
    Serial.println("❌ Write error REG_POS_TARGET_H (0x0034)");
    ok = false;
  }

  if (writeRegisterRaw(REG_POS_TARGET_L, 0)) {
    Serial.println("📍 REG_POS_TARGET_L (0x0035) → OK");
  } else {
    Serial.println("❌ Write error REG_POS_TARGET_L (0x0035)");
    ok = false;
  }

  uint16_t status = 0;
  if (readRegisterRaw(REG_STATUS_WORD, status)) {
    Serial.print("🧾 Initial status (0x0003): 0x");
    Serial.println(status, HEX);
  } else {
    Serial.println("❌ Read error REG_STATUS_WORD (0x0003)");
    ok = false;
  }

  if (writeRegisterRaw(REG_ZERO_MODE, 35)) {
    Serial.println("📍 REG_ZERO_MODE (0x0040) → OK");
  } else {
    Serial.println("❌ Write error REG_ZERO_MODE (0x0040)");
    ok = false;
  }

  _initialized = ok;

  if (ok) {
    Serial.print("✅ ");
    Serial.print(_name);
    Serial.println(".begin() completed successfully");
  } else {
    Serial.print("⚠️ ");
    Serial.print(_name);
    Serial.println(".begin() failed — driver not initialized");
  }

  return ok;
}

bool CL57RStepper::writeRegisterRaw(uint16_t reg, uint16_t value) {
  return _bus->writeSingle(_id, reg, value);
}

bool CL57RStepper::readRegisterRaw(uint16_t reg, uint16_t& value) {
  uint16_t buffer[1];
  if (!_bus->readHolding(_id, reg, buffer, 1)) return false;
  value = buffer[0];
  return true;
}

bool CL57RStepper::isInitialized() {
  return _initialized;
}

bool CL57RStepper::guardInit(const String& caller) {
  if (!_initialized) {
    Serial.print("🔒 Error in ");
    Serial.print(_name);
    Serial.print(".");
    Serial.print(caller);
    Serial.println(": driver not initialized. Call ");
    Serial.print(_name);
    Serial.println(".begin();");
    return false;
  }
  return true;
}

bool CL57RStepper::isConnected() {
  uint16_t dummy = 0;
  return readRegister(REG_MODEL_ID, dummy);
}

bool CL57RStepper::enableMotor() {
  if (!guardInit("enableMotor")) return false;
  return writeRegister(REG_ENABLE_MOTOR, 1);
}

bool CL57RStepper::disableMotor() {
  if (!guardInit("disableMotor")) return false;
  return writeRegister(REG_ENABLE_MOTOR, 0);
}

bool CL57RStepper::clearAlarm() {
  if (!guardInit("clearAlarm")) return false;
  return writeRegister(REG_AUX_CONTROL, 0x0004);
}

bool CL57RStepper::resetAbsolutePosition() {
  if (!guardInit("resetAbsolutePosition")) return false;
  bool ok = true;

  if (writeRegister(REG_AUX_CONTROL, 0x0008)) {
    Serial.println("📍 Current position reset via AUX (0x0008)");
  } else {
    Serial.println("❌ Error resetting current position");
    ok = false;
  }

  if (writeRegister(REG_POS_TARGET_H, 0)) {
    Serial.println("📍 Target HIGH (0x0034) reset");
  } else {
    Serial.println("❌ Write error REG_POS_TARGET_H");
    ok = false;
  }

  if (writeRegister(REG_POS_TARGET_L, 0)) {
    Serial.println("📍 Target LOW (0x0035) reset");
  } else {
    Serial.println("❌ Write error REG_POS_TARGET_L");
    ok = false;
  }

  return ok;
}

bool CL57RStepper::restoreFactorySettings() {
  if (!guardInit("restoreFactorySettings")) return false;
  return writeRegister(REG_AUX_CONTROL, 0x0001);
}

bool CL57RStepper::moveToAbsolutePosition(int32_t target) {
  if (!guardInit("moveToAbsolutePosition")) return false;
  uint32_t pos = static_cast<uint32_t>(target);
  writeRegister(0x003A, 1);
  if (!writeDoubleRegister(REG_POS_TARGET_H, REG_POS_TARGET_L, pos)) return false;
  return writeRegister(REG_MOTION_CTRL_WORD, 0x0003);
}

bool CL57RStepper::moveToRelativePosition(int32_t offset) {
  if (!guardInit("moveToRelativePosition")) return false;
  uint32_t pos = static_cast<uint32_t>(offset);
  writeRegister(0x003A, 0);
  if (!writeDoubleRegister(REG_POS_TARGET_H, REG_POS_TARGET_L, pos)) return false;
  return writeRegister(REG_MOTION_CTRL_WORD, 0x0001);
}

bool CL57RStepper::startSpeed() {
  if (!guardInit("startSpeed")) return false;
  return writeRegister(REG_MOTION_CTRL_WORD, 0x0008);
}

bool CL57RStepper::stopSmooth() {
  if (!guardInit("stopSmooth")) return false;
  return writeRegister(REG_MOTION_CTRL_WORD, 0x0020);
}

bool CL57RStepper::stopEmergency() {
  if (!guardInit("stopEmergency")) return false;
  return writeRegister(REG_MOTION_CTRL_WORD, 0x0040);
}

int32_t CL57RStepper::getActualPosition() {
  if (!guardInit("getActualPosition")) return 0;
  uint16_t hi = 0, lo = 0;
  if (!readRegister(REG_POS_ACTUAL_H, hi)) return 0;
  if (!readRegister(REG_POS_ACTUAL_L, lo)) return 0;
  uint32_t raw = ((uint32_t)hi << 16) | lo;
  return static_cast<int32_t>(raw);
}

uint16_t CL57RStepper::getStatusWord() {
  if (!guardInit("getStatusWord")) return 0;
  uint16_t val = 0;
  readRegister(REG_STATUS_WORD, val);
  return val;
}

uint16_t CL57RStepper::getErrorCode() {
  if (!guardInit("getErrorCode")) return 0;
  uint16_t val = 0;
  readRegister(REG_ERROR_CODE, val);
  return val;
}

uint16_t CL57RStepper::getWorkingMode() {
  if (!guardInit("getWorkingMode")) return 0;
  uint16_t val = 0;
  readRegister(REG_WORKING_MODE, val);
  return val;
}

bool CL57RStepper::readRegister(uint16_t reg, uint16_t& value) {
  if (!guardInit("readRegister")) return false;
  uint16_t buffer[1];
  if (!_bus->readHolding(_id, reg, buffer, 1)) return false;
  value = buffer[0];
  return true;
}

bool CL57RStepper::writeRegister(uint16_t reg, uint16_t value) {
  if (!guardInit("writeRegister")) return false;
  return _bus->writeSingle(_id, reg, value);
}

bool CL57RStepper::writeDoubleRegister(uint16_t regH, uint16_t regL, uint32_t value) {
  if (!guardInit("writeDoubleRegister")) return false;
  uint16_t high = (value >> 16) & 0xFFFF;
  uint16_t low  = value & 0xFFFF;
  return writeRegister(regH, high) && writeRegister(regL, low);
}

bool CL57RStepper::isMovementComplete() {
  if (!guardInit("isMovementComplete")) return false;
  uint16_t status = getStatusWord();
  return (status & (1 << 0)) && !(status & (1 << 1));
}

void CL57RStepper::printDiagnostics() {
  if (!guardInit("printDiagnostics")) return;
  uint16_t error = getErrorCode();
  uint16_t model = 0;
  uint16_t firmware = 0;

  readRegister(REG_MODEL_ID, model);

  Serial.println("\n🔧 DRIVER DIAGNOSTICS [" + _name + "]");
  Serial.println("🩺 CL57R Stepper Driver:");
  Serial.print("🆔 Model / ID: "); Serial.println(model);

  if (readRegister(REG_SOFTWARE_VERSION, firmware)) {
    uint8_t major = firmware / 100;
    uint8_t minor = (firmware / 10) % 10;
    uint8_t patch = firmware % 10;
    Serial.print("🧬 Firmware Version: v");
    Serial.print(major); Serial.print(".");
    Serial.print(minor); Serial.print(".");
    Serial.println(patch);
  } else {
    Serial.println("❌ Firmware read error");
  }

  Serial.print("🚨 Error Code: "); Serial.print(error);
  Serial.print(" "); Serial.println(decodeErrorCode(error));
}

void CL57RStepper::printMotorSettings() {
  if (!guardInit("printMotorSettings")) return;
  uint16_t microstep = 0;
  uint16_t base = 0, closed = 0, open = 0;
  uint16_t orientation = 0, loop = 0, current_gain = 0;
  uint16_t encRes = 0, trackingErr = 0;

  readRegister(REG_MICROSTEP, microstep);
  readRegister(REG_ORIENTATION, orientation);
  readRegister(REG_LOOP_MODE, loop);
  readRegister(REG_CURRENT_BASE_RATIO, base);
  readRegister(REG_CURRENT_CLOSED_RATIO, closed);
  readRegister(REG_CURRENT_OPEN_RATIO, open);
  readRegister(REG_GAIN_CURRENT_P, current_gain);

  Serial.println("\n🔧 STEPPER CONFIGURATION [" + _name + "]");
  Serial.print("🔄 Microsteps/rev: "); Serial.println(microstep);

  Serial.print("🧭 Orientation: ");
  Serial.println(orientation == 0 ? "CCW (counterclockwise)" : "CW (clockwise)");

  Serial.print("🔁 Operating Mode: ");
  switch (loop) {
    case 0: Serial.println("Open loop"); break;
    case 1: Serial.println("Closed loop"); break;
    default: Serial.println("⚠️ Undefined mode"); break;
  }

  Serial.print("🔋 Base current: "); Serial.print(base); Serial.println(" mA");
  Serial.print("🔋 Closed-loop current: "); Serial.print(closed); Serial.println(" mA");
  Serial.print("🔋 Open-loop current: "); Serial.print(open); Serial.println(" mA");
  Serial.print("🔋 Current proportional gain: "); Serial.println(current_gain);

  if (loop == 1) {
    readRegister(0x0051, encRes);
    readRegister(0x0052, trackingErr);

    Serial.println("🎯 Closed-loop settings:");
    Serial.print("  • Encoder resolution: ");
    if (encRes == 0) Serial.println("1000 lines (4000 pulses)");
    else if (encRes == 1) Serial.println("2500 lines (10000 pulses)");
    else Serial.println("⚠️ Invalid value");

    Serial.print("  • Tracking alarm threshold: ±");
    Serial.print(trackingErr);
    Serial.println(" pulses");
  }
}

void CL57RStepper::printMotorStatus() {
  if (!guardInit("printMotorStatus")) return;

  Serial.println("\n🔧 MOTOR STATUS [" + _name + "]");

  uint16_t status = getStatusWord();
  uint16_t mode = getWorkingMode();
  uint16_t motion = 0;
  uint16_t rpmActual = 0;
  uint16_t posHi = 0, posLo = 0;
  uint16_t tgtHi = 0, tgtLo = 0;

  readRegister(REG_POS_MODE, motion);
  readRegister(REG_SPEED_ACTUAL, rpmActual);
  readRegister(REG_POS_ACTUAL_H, posHi);
  readRegister(REG_POS_ACTUAL_L, posLo);
  readRegister(REG_POS_TARGET_H, tgtHi);
  readRegister(REG_POS_TARGET_L, tgtLo);

  int32_t pos = ((uint32_t)posHi << 16) | posLo;
  int32_t tgt = ((uint32_t)tgtHi << 16) | tgtLo;

  bool inPlace    = status & (1 << 0);
  bool zeroDone   = status & (1 << 1);
  bool running    = status & (1 << 2);
  bool alarm      = status & (1 << 3);
  bool enabled    = status & (1 << 4);

  Serial.print("🟢 Motor enabled: "); Serial.println(enabled ? "Yes" : "No");

  Serial.print("🧭 Active mode: ");
  switch (mode) {
    case 0: Serial.println("🚫 None"); break;
    case 1: Serial.println("📍 Positioning"); break;
    case 2: Serial.println("⚡ Speed"); break;
    case 3: Serial.println("🎯 Return to zero"); break;
    case 4: Serial.println("↕️ Manual jog"); break;
    case 5: Serial.println("🔄 Multi-position"); break;
    case 6: Serial.println("📊 Multi-speed"); break;
    default: Serial.println("❓ Unknown"); break;
  }

  if (mode == 1) {
    Serial.print("🔁 Positioning Type: ");
    switch (motion) {
      case 0: Serial.println("🔁 Relative"); break;
      case 1: Serial.println("📍 Absolute"); break;
      default: Serial.print("❓ Invalid ("); Serial.print(motion); Serial.println(")");
    }
  }

  Serial.print("🚨 Alarm active: "); Serial.println(alarm ? "Yes" : "No");
  Serial.print("✅ In position: "); Serial.println(inPlace ? "Yes" : "No");
  Serial.print("🔄 In motion: "); Serial.println(running ? "Yes" : "No");
  Serial.print("🎯 Zero completed: "); Serial.println(zeroDone ? "Yes" : "No");
  Serial.print("📍 Current position: "); Serial.println(pos);
  Serial.print("🎯 Target position:  "); Serial.println(tgt);
  Serial.print("⚙️ Current speed:  "); Serial.print(rpmActual); Serial.println(" rpm");
}

void CL57RStepper::printMotionStatus() {
  if (!guardInit("printMotionStatus")) return;

  int32_t pos = getActualPosition();
  uint16_t tgtHi = 0, tgtLo = 0;
  uint16_t rpmActual = 0, rpmTarget = 0;
  uint16_t accelTime = 0, decelTime = 0;

  readRegister(REG_POS_TARGET_H, tgtHi);
  readRegister(REG_POS_TARGET_L, tgtLo);
  readRegister(REG_SPEED_ACTUAL, rpmActual);
  readRegister(REG_VMAX, rpmTarget);
  readRegister(REG_ACCEL_TIME, accelTime);
  readRegister(REG_DECEL_TIME, decelTime);

  int32_t targetPos = ((uint32_t)tgtHi << 16) | tgtLo;

  Serial.println("\n📊 MOTION PARAMETERS [" + _name + "]");
  Serial.print("📍 Current position: "); Serial.println(pos);
  Serial.print("🎯 Target position:  "); Serial.println(targetPos);
  Serial.print("⚙️ Current speed:  "); Serial.print(rpmActual); Serial.println(" rpm");
  Serial.print("🏁 Target speed:   "); Serial.print(rpmTarget); Serial.println(" rpm");
  Serial.print("⏱️ Acceleration:   "); Serial.print(accelTime); Serial.println(" ms");
  Serial.print("⏱️ Deceleration:   "); Serial.print(decelTime); Serial.println(" ms");
}

void CL57RStepper::printIOStatus() {
  if (!guardInit("printIOStatus")) return;
  uint16_t inputFlags = 0, outputFlags = 0;
  uint16_t diFunc[7] = {0};  // DI function codes
  uint16_t doFunc[3] = {0};  // DO function codes

  if (!readRegister(0x0005, inputFlags)) {
    Serial.println("❌ Error reading digital inputs.");
    return;
  }

  if (!readRegister(0x0006, outputFlags)) {
    Serial.println("❌ Error reading digital outputs.");
    return;
  }

  // Read DI function codes (0x0013 – 0x0019)
  for (uint8_t i = 0; i < 7; i++) {
    readRegister(0x0013 + i, diFunc[i]);
  }

  // Read DO function codes (0x001A – 0x001C)
  for (uint8_t i = 0; i < 3; i++) {
    readRegister(0x001A + i, doFunc[i]);
  }

  Serial.println("\n🔧 DIGITAL I/O STATUS [" + _name + "]");
  Serial.println("🔌 Inputs (DI0–DI6):");

  for (uint8_t i = 0; i < 7; i++) {
    bool state = (inputFlags >> i) & 0x01;
    Serial.print("   DI"); Serial.print(i);
    Serial.print(": ");
    Serial.print(state ? "Closed 🔒" : "Open 🔓");
    Serial.print(" — Function: ");
    Serial.println(getDIFunctionName(diFunc[i]));
  }

  Serial.println("⚡ Outputs (DO0–DO2):");

  for (uint8_t i = 0; i < 3; i++) {
    bool state = (outputFlags >> i) & 0x01;
    Serial.print("   DO"); Serial.print(i);
    Serial.print(": ");
    Serial.print(state ? "Active 🟢" : "Inactive ⚪");
    Serial.print(" — Function: ");
    Serial.println(getDOFunctionName(doFunc[i]));
  }
}
String CL57RStepper::decodeErrorCode(uint16_t code) {
  if (!guardInit("decodeErrorCode")) return "Driver not initialized";
  switch (code) {
    case 0x0000: return "✅ No error";
    case 0x0001: return "🔌 Overcurrent";
    case 0x0002: return "⚡ Overvoltage";
    case 0x0003: return "📉 Step loss (closed loop)";
    case 0x0004: return "📡 Feedback / encoder error";
    case 0x0005: return "🛑 Motor stall";
    default:     return "❓ Unknown error: " + String(code);
  }
}

String CL57RStepper::getDIFunctionName(uint16_t code) {
  if (!guardInit("getDIFunctionName")) return "Driver not initialized";
  switch (code) {
    case 1: return "Origin (ORG)";
    case 2: return "Limit +";
    case 3: return "Limit −";
    case 4: return "Motor release";
    case 5: return "Clear alarms";
    case 6: return "Stop";
    case 7: return "Emergency";
    case 8: return "Jog forward";
    case 9: return "Jog reverse";
    case 10: return "Zero trigger";
    case 11: return "Position trigger";
    case 12: return "Speed trigger";
    case 13: return "Path address 0";
    case 14: return "Path address 1";
    case 15: return "Path address 2";
    case 16: return "Path address 3";
    default: return "Unassigned";
  }
}

String CL57RStepper::getDOFunctionName(uint16_t code) {
  if (!guardInit("getDOFunctionName")) return "Driver not initialized";
  switch (code) {
    case 1: return "Alarm signal";
    case 2: return "Motor running";
    case 3: return "Return to zero OK";
    case 4: return "Motor in position";
    case 5: return "Motor brake";
    default: return "Unassigned";
  }
}
