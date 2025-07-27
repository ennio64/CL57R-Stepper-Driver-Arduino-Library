#ifndef CL57R_MACROS_H
#define CL57R_MACROS_H

#include "CL57RStepper.h"

// 📐 Macro per creare istanza con nome associato
#define CL57RStepper_(varName, busPtr, slaveID) \
  CL57RStepper varName(busPtr, slaveID, #varName)

#endif

