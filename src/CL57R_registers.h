#ifndef CL57R_REGISTERS_H
#define CL57R_REGISTERS_H

// ░░░ Informazioni generali
#define REG_MODEL_ID                  0x0000  // Drive model code (RO)
#define REG_SOFTWARE_VERSION          0x0001  // Firmware version (RO)
#define REG_WORKING_MODE              0x0002  // 0=idle, 1=pos, 2=speed, 3=zero, 4=jog, 5=path (RO)

// ░░░ Stato e diagnostica
#define REG_STATUS_WORD               0x0003  // Bit0=in pos, Bit3=alarm, Bit4=enabled (RO)
#define REG_ERROR_CODE                0x0004  // 1=overcurrent, 2=overvoltage, 4=tracking (RO)
#define REG_DI_STATE                  0x0005  // Ingressi X0–X6 digitali (RO)
#define REG_DO_STATE                  0x0006  // Uscite Y0–Y2 digitali (RO)

// ░░░ Posizione e velocità
#define REG_POS_ACTUAL_H              0x0007  // Actual position MSB (RO)
#define REG_POS_ACTUAL_L              0x0008  // Actual position LSB (RO)
#define REG_SPEED_ACTUAL              0x0009  // RPM attuale (RO)

// ░░░ Logica ingressi/uscite
#define REG_DI_INVERT_LEVEL           0x0010  // Inversione ingressi (RW/S)
#define REG_DI_FILTER_TIME            0x0011  // Filtro ingressi in ms (RW/S)
#define REG_DO_INVERT_LEVEL           0x0012  // Inversione uscite (RW/S)

#define REG_X0_FUNCTION               0x0013
#define REG_X1_FUNCTION               0x0014
#define REG_X2_FUNCTION               0x0015
#define REG_X3_FUNCTION               0x0016
#define REG_X4_FUNCTION               0x0017
#define REG_X5_FUNCTION               0x0018
#define REG_X6_FUNCTION               0x0019

#define REG_Y0_FUNCTION               0x001A
#define REG_Y1_FUNCTION               0x001B
#define REG_Y2_FUNCTION               0x001C

// ░░░ Comunicazione
#define REG_MODBUS_ID                 0x0020  // Slave address (RW/S)
#define REG_SERIAL_FORMAT             0x0021  // Format: 0=8N1, 1=8N2, 2=8E1 (RW/S)

// ░░░ Orientamento e impostazioni generali
#define REG_ORIENTATION               0x0022  // 0=CCW, 1=CW (RW/S)
#define REG_MICROSTEP                 0x0023  // μstep/giro (RW/S)
#define REG_EEPROM_SYNC               0x0024  // 0=no, 1=sì (RW/S)
#define REG_OVERTRAVEL_METHOD         0x0025  // 0=slow stop, 1=emergency (RW/S)

// ░░░ Movimento singolo
#define REG_VSTART                    0x0030  // Velocità iniziale (RPM) (RW)
#define REG_ACCEL_TIME                0x0031  // Accelerazione (ms) (RW)
#define REG_DECEL_TIME                0x0032  // Decelerazione (ms) (RW)
#define REG_VMAX                      0x0033  // Velocità max (RPM) (RW)
#define REG_POS_TARGET_H              0x0034  // Target position MSB (RW)
#define REG_POS_TARGET_L              0x0035  // Target position LSB (RW)
#define REG_MOTION_CTRL_WORD          0x0036  // Bit0=enable, Bit1=rel/abs (RW)

// ░░░ Controlli comandi diretti
#define REG_AUX_CONTROL               0x0037  // 0x0001=reset, 0x0004=clear alarm, 0x0008=clear pos (RW)
#define REG_ENABLE_MOTOR              0x0038  // 1=ON, 0=OFF (RW)
#define REG_TRIGGER_MODE              0x0039  // Trigger multi-path (RW)
#define REG_POS_MODE                  0x003A  // 0=relativo, 1=assoluto (RW)

// ░░░ Ritorno all’origine
#define REG_ZERO_MODE                 0x0040
#define REG_ZERO_SPEED                0x0041
#define REG_ZERO_CRAWL                0x0042
#define REG_ZERO_ACCEL_DECEL          0x0043
#define REG_ZERO_COMP_H               0x0044
#define REG_ZERO_COMP_L               0x0045

// ░░░ Jog manuale
#define REG_JOG_SPEED                 0x0046
#define REG_JOG_ACCEL_TIME            0x0047
#define REG_JOG_DECEL_TIME            0x0048

// ░░░ Servo / encoder
#define REG_LOOP_MODE                 0x0050  // 0=open, 1=closed (RW/S)
#define REG_ENCODER_RESOLUTION        0x0051  // 0=1000, 1=2500 (RW/S)
#define REG_TRACKING_ERROR_THRESHOLD  0x0052  // in impulsi (RW/S)

// ░░░ Correnti
#define REG_CURRENT_BASE_RATIO        0x0053  // Corrente base (RW/S)
#define REG_CURRENT_CLOSED_RATIO      0x0054  // Corrente closed loop (RW/S)
#define REG_CURRENT_OPEN_RATIO        0x0055  // Corrente open loop (RW/S)
#define REG_INSTRUCTION_FILTER_TIME   0x0056  // Filtro comandi in step (RW/S)

// ░░░ Tuning controlli (solo avanzati)
#define REG_GAIN_CURRENT_P            0x0057
#define REG_GAIN_CURRENT_I            0x0058
#define REG_GAIN_POSITION_P           0x0059
#define REG_GAIN_SPEED_P              0x005A
#define REG_GAIN_FEED_FORWARD         0x005B

// ░░░ Profili multipli (Path 0–15)
#define REG_PATH_ACCEL_START          0x00A0  // 0x00A0 ~ 0x00AF (accel time)
#define REG_PATH_SPEED_START          0x00B0  // 0x00B0 ~ 0x00BF (running speed)
#define REG_PATH_ACCEL_TIME           0x00C0  // 0x00C0 ~ 0x00CF (accel time)
#define REG_PATH_DECEL_TIME           0x00D0  // 0x00D0 ~ 0x00DF (decel time)

#endif // CL57R_REGISTERS_H
