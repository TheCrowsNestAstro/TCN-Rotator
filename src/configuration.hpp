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

// RATIO 7.8125:1
// 125 Big Gear
#define GEAR_RATIO 7.8125f
#define STEPPER_SPR 400
#define MICROSTEPPING 8
#define STEPS_PER_DEGREE   ((STEPPER_SPR * MICROSTEPPING * GEAR_RATIO)/360.0f)
#define MIN_STEP_SIZE (1.0f/STEPS_PER_DEGREE)

#define STEPPER_SPEED             900
#define STEPPER_ACCELERATION      500
