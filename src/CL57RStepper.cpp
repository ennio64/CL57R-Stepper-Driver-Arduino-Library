	#include "CL57RStepper.h"
#include "CL57RMacros.h"

CL57RStepper::CL57RStepper(RS485Bus* bus, uint8_t slaveID, const String& name) {
  _bus = bus;
  _id = slaveID;
  _name = name;
}

bool CL57RStepper::begin() {
  Serial.print("🔓 Inizio procedura ");
  Serial.print(_name);
  Serial.println(".begin()");

  bool ok = true;

  // 🟢 Abilita il motore
  if (writeRegisterRaw(REG_ENABLE_MOTOR, 1)) {
    Serial.println("🟢 REG_ENABLE_MOTOR (0x0038) → OK [motore abilitato]");
  } else {
    Serial.println("❌ Errore scrittura REG_ENABLE_MOTOR (0x0038)");
    ok = false;
  }

  // 📍 Azzera posizione target HIGH
  if (writeRegisterRaw(REG_POS_TARGET_H, 0)) {
    Serial.println("📍 REG_POS_TARGET_H (0x0034) → OK");
  } else {
    Serial.println("❌ Errore scrittura REG_POS_TARGET_H (0x0034)");
    ok = false;
  }

  // 📍 Azzera posizione target LOW
  if (writeRegisterRaw(REG_POS_TARGET_L, 0)) {
    Serial.println("📍 REG_POS_TARGET_L (0x0035) → OK");
  } else {
    Serial.println("❌ Errore scrittura REG_POS_TARGET_L (0x0035)");
    ok = false;
  }

  // 🧾 Lettura dello stato iniziale
  uint16_t status = 0;
  if (readRegisterRaw(REG_STATUS_WORD, status)) {
    Serial.print("🧾 Status iniziale (0x0003): 0x");
    Serial.println(status, HEX);
  } else {
    Serial.println("❌ Errore lettura REG_STATUS_WORD (0x0003)");
    ok = false;
  }

 // 📍 Imposta modalità zero → 35 = punto attuale come origine
 if (writeRegisterRaw(REG_ZERO_MODE, 35)) {
   Serial.println("📍 REG_ZERO_MODE (0x0040) → OK");
 } else {
   Serial.println("❌ Errore scrittura REG_ZERO_MODE (0x0040)");
   ok = false;
 }

  _initialized = ok;

if (ok) {
  Serial.print("✅ ");
  Serial.print(_name);
  Serial.println(".begin() completato con successo");
} else {
  Serial.print("⚠️ ");
  Serial.print(_name);
  Serial.println(".begin() fallito — driver non inizializzato");
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
    Serial.print("🔒 Errore in ");
    Serial.print(_name);  // ← nome dello stepper nell'istanza
    Serial.print(".");
    Serial.print(caller);
    Serial.println(": driver non inizializzato. Chiamare ");
    Serial.print(_name);  // ← nome corretto nel messaggio di aiuto
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

  // 🧹 Reset posizione attuale via comando AUX
  if (writeRegister(REG_AUX_CONTROL, 0x0008)) {
    Serial.println("📍 Posizione attuale azzerata via AUX (0x0008)");
  } else {
    Serial.println("❌ Errore nel reset posizione attuale");
    ok = false;
  }

  // 🎯 Azzeramento posizione target H/L
  if (writeRegister(REG_POS_TARGET_H, 0)) {
    Serial.println("📍 Target HIGH (0x0034) azzerato");
  } else {
    Serial.println("❌ Errore scrittura REG_POS_TARGET_H");
    ok = false;
  }

  if (writeRegister(REG_POS_TARGET_L, 0)) {
    Serial.println("📍 Target LOW (0x0035) azzerato");
  } else {
    Serial.println("❌ Errore scrittura REG_POS_TARGET_L");
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
 
  Serial.println("\n🔧 DIAGNOSTICA DRIVER [" + _name + "]");  
  Serial.println("🩺 Driver Motore CL57R:");
  Serial.print("🆔 Modello / Identificativo: "); Serial.println(model);

  if (readRegister(REG_SOFTWARE_VERSION, firmware)) {
	uint8_t major = firmware / 100;
  	uint8_t minor = (firmware / 10) % 10;
	uint8_t patch = firmware % 10;
	Serial.print("🧬 Versione Firmware: v");
  	Serial.print(major); Serial.print(".");
  	Serial.print(minor); Serial.print(".");
  	Serial.println(patch);
} else {
  Serial.println("❌ Errore lettura firmware");
}
  Serial.print("🚨 Codice Errore: "); Serial.print(error); 
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

  Serial.println("\n🔧 CONFIGURAZIONE STEPPER [" + _name + "]");
  Serial.print("🔄 Microstep/giro: "); Serial.println(microstep);

  Serial.print("🧭 Orientamento: ");
  Serial.println(orientation == 0 ? "CCW (antiorario)" : "CW (orario)");

  Serial.print("🔁 Modalità funzionamento: ");
   switch (loop) {
    case 0: Serial.println("Open loop"); break;
    case 1: Serial.println("Closed loop"); break;
    default: Serial.println("⚠️ Modalità non definita"); break;
   }
  Serial.print("🔋 Corrente base: "); Serial.print(base); Serial.println(" mA");
  Serial.print("🔋 Corrente closed-loop: "); Serial.print(closed); Serial.println(" mA");
  Serial.print("🔋 Corrente open-loop: "); Serial.print(open); Serial.println(" mA");
  Serial.print("🔋 Gain proporzionale corrente: "); Serial.println(current_gain);

 if (loop == 1) { // Closed loop attivo
  readRegister(0x0051, encRes);       // Encoder resolution
  readRegister(0x0052, trackingErr);  // Tracking error alarm threshold

  Serial.println("🎯 Closed-loop settings:");
  Serial.print("  • Risoluzione encoder: ");
  if (encRes == 0) Serial.println("1000 linee (4000 impulsi)");
  else if (encRes == 1) Serial.println("2500 linee (10000 impulsi)");
  else Serial.println("⚠️ Valore non valido");

  Serial.print("  • Soglia allarme tracking: ±");
  Serial.print(trackingErr);
  Serial.println(" impulsi");
 }
}

void CL57RStepper::printMotorStatus() {
  if (!guardInit("printMotorStatus")) return;

  Serial.println("\n🔧 STATO MOTORE [" + _name + "]");

  // 🧾 Lettura registri
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

  int32_t pos = static_cast<int32_t>(((uint32_t)posHi << 16) | posLo);
  int32_t tgt = static_cast<int32_t>(((uint32_t)tgtHi << 16) | tgtLo);

  // 🎯 Interpretazione bit Status Word
  bool inPlace    = status & (1 << 0);
  bool zeroDone   = status & (1 << 1);
  bool running    = status & (1 << 2);
  bool alarm      = status & (1 << 3);
  bool enabled    = status & (1 << 4);

  // 📝 Stampa stato motore
  Serial.print("🟢 Motore abilitato: "); Serial.println(enabled ? "Sì" : "No");
  // 📌 Modalità operativa
  Serial.print("🧭 Modalità attiva: ");
  switch (mode) {
    case 0: Serial.println("🚫 Nessuna"); break;
    case 1: Serial.println("📍 Posizionamento"); break;
    case 2: Serial.println("⚡ Velocità"); break;
    case 3: Serial.println("🎯 Ritorno a zero"); break;
    case 4: Serial.println("↕️ Jog manuale"); break;
    case 5: Serial.println("🔄 Multi-posizione"); break;
    case 6: Serial.println("📊 Multi-velocità"); break;
    default: Serial.println("❓ Sconosciuta"); break;
  }
// 🧭 Tipo posizionamento (solo se in modalità 1)
if (mode == 1) {
  Serial.print("🔁 Tipo Posizionamento: ");
  switch (motion) {
    case 0: Serial.println("🔁 Relativo"); break;
    case 1: Serial.println("📍 Assoluto"); break;
    default: Serial.print("❓ Non valido ("); Serial.print(motion); Serial.println(")");
  }
}
  Serial.print("🚨 Allarme attivo: ");    Serial.println(alarm ? "Sì" : "No");
  Serial.print("✅ In posizione: ");      Serial.println(inPlace ? "Sì" : "No");
  Serial.print("🔄 In movimento: ");      Serial.println(running ? "Sì" : "No");
  Serial.print("🎯 Zero completato: ");   Serial.println(zeroDone ? "Sì" : "No");
  // 📍 Posizione e velocità
  Serial.print("📍 Posizione attuale: "); Serial.println(pos);
  Serial.print("🎯 Posizione target:  "); Serial.println(tgt);
  Serial.print("⚙️ Velocità attuale:  "); Serial.print(rpmActual); Serial.println(" rpm");
}

void CL57RStepper::printMotionStatus() {
  if (!guardInit("printMotionStatus")) return;

  // Lettura valori chiave
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

  Serial.println("\n📊 PARAMETRI MOVIMENTO [" + _name + "]");
  Serial.print("📍 Posizione attuale: "); Serial.println(pos);
  Serial.print("🎯 Posizione target:  "); Serial.println(targetPos);
  Serial.print("⚙️ Velocità attuale:  "); Serial.print(rpmActual); Serial.println(" rpm");
  Serial.print("🏁 Velocità target:   "); Serial.print(rpmTarget); Serial.println(" rpm");
  Serial.print("⏱️ Accelerazione:     "); Serial.print(accelTime); Serial.println(" ms");
  Serial.print("⏱️ Decelerazione:     "); Serial.print(decelTime); Serial.println(" ms");
}


void CL57RStepper::printIOStatus() {
  if (!guardInit("printIOStatus")) return;
  uint16_t inputFlags = 0, outputFlags = 0;
  uint16_t diFunc[7] = {0};  // Funzione DIx
  uint16_t doFunc[3] = {0};  // Funzione DOx

  if (!readRegister(0x0005, inputFlags)) {
    Serial.println("❌ Errore lettura ingressi digitali.");
    return;
  }

  if (!readRegister(0x0006, outputFlags)) {
    Serial.println("❌ Errore lettura uscite digitali.");
    return;
  }

  // Leggi funzioni DIx (0x0013 – 0x0019)
  for (uint8_t i = 0; i < 7; i++) {
    readRegister(0x0013 + i, diFunc[i]);
  }

  // Leggi funzioni DOx (0x001A – 0x001C)
  for (uint8_t i = 0; i < 3; i++) {
    readRegister(0x001A + i, doFunc[i]);
  }

   Serial.println("\n🔧 STATO I/O DIGITALI [" + _name + "]");
  Serial.println("🔌 Ingressi (DI0–DI6):");

  for (uint8_t i = 0; i < 7; i++) {
    bool state = (inputFlags >> i) & 0x01;
    Serial.print("   DI"); Serial.print(i);
    Serial.print(": ");
    Serial.print(state ? "Closed 🔒" : "Open 🔓");
    Serial.print(" — Funzione: ");
    Serial.println(getDIFunctionName(diFunc[i]));
  }

  Serial.println("⚡ Uscite (DO0–DO2):");

  for (uint8_t i = 0; i < 3; i++) {
    bool state = (outputFlags >> i) & 0x01;
    Serial.print("   DO"); Serial.print(i);
    Serial.print(": ");
    Serial.print(state ? "Active 🟢" : "Inactive ⚪");
    Serial.print(" — Funzione: ");
    Serial.println(getDOFunctionName(doFunc[i]));
  }
}

String CL57RStepper::decodeErrorCode(uint16_t code) {
  if (!guardInit("decodeErrorCode")) return "Driver non inizializzato";
  switch (code) {
    case 0x0000: return "✅ Nessun errore";
    case 0x0001: return "🔌 Sovracorrente";
    case 0x0002: return "⚡ Sovratensione";
    case 0x0003: return "📉 Perdita di passo (closed loop)";
    case 0x0004: return "📡 Errore feedback / encoder";
    case 0x0005: return "🛑 Stallo motore";
    default:     return "❓ Errore sconosciuto: " + String(code);
  }
}

String CL57RStepper::getDIFunctionName(uint16_t code) {
  if (!guardInit("getDIFunctionName")) return "Driver non inizializzato";
  switch (code) {
    case 1: return "Origine (ORG)";
    case 2: return "Finecorsa +";
    case 3: return "Finecorsa −";
    case 4: return "Rilascio motore";
    case 5: return "Cancella allarmi";
    case 6: return "Stop";
    case 7: return "Emergenza";
    case 8: return "Jog avanti";
    case 9: return "Jog indietro";
    case 10: return "Trigger zero";
    case 11: return "Trigger posizione";
    case 12: return "Trigger velocità";
    case 13: return "Path address 0";
    case 14: return "Path address 1";
    case 15: return "Path address 2";
    case 16: return "Path address 3";
    default: return "Non assegnata";
  }
}

String CL57RStepper::getDOFunctionName(uint16_t code) {
  if (!guardInit("getDOFunctionName")) return "Driver non inizializzato";
  switch (code) {
    case 1: return "Segnale allarme";
    case 2: return "Motore in movimento";
    case 3: return "Ritorno a zero OK";
    case 4: return "Motore in posizione";
    case 5: return "Freno motore";
    default: return "Non assegnata";
  }
}
