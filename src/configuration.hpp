#pragma once

// ASCOM Alpaca Definitions
#define SERVER_NAME "TCN"
#define MANUFACTURER "Christian Kardach"
#define MANUFACTURER_VERSION "1.0"
#define LOCATION "SE"

#define DEVICE_NAME "TCN Rotator"
#define DEVICE_TYPE "Rotator"
#define DEVICE_NR 0
#define UNIQUE_CLIENT_ID "" // https://www.guidgenerator.com/online-guid-generator.aspx

#define DESCRIPTION ""
#define DRIVER_INFO "TCN ASCOM Alpaca Driver"
#define DRIVER_VERSION "1.0"

// Stepper Setup
#define EN_PIN 14
#define DIR_PIN  12
#define STEP_PIN  13
#define HOME_PIN 4

// Storage
#define ROTATOR_EEPROM_START 0

#define WHEEL_CIRCUMFERENCE 502.655f
#define PULLEY_TEETH 16
#define GT2_BELT_PITCH 2.0f
#define STEPPER_SPR (400*9)
#define STEPS_PER_DEGREE 78.125 //((DEC_WHEEL_CIRCUMFERENCE / (DEC_PULLEY_TEETH * GT2_BELT_PITCH) * DEC_STEPPER_SPR / 360.0f))
// (502.655/32.0*3600/360) = 156.875
// Gear ratio 0.128:1
//46.08:360