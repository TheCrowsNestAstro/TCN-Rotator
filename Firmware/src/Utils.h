#include <Arduino.h>
#include <EEPROM.h>
#include "configuration.hpp"

static void savePosition(long newPos) {
  writeLong(getSaveFocuserPosAddress() + 1, newPos);
}

static long readPosition() {
  return readLong(getReadFocuserPosAddress() + 1);
}

static long readLong(word address) {
  word lowWord = readWord(address);
  word highWord = readWord(address + 2);
  return lowWord + highWord * 65536;
}

static void writeWord(word address, word value) {
  EEPROM.write(address, lowByte(value)); 
  EEPROM.write(address + 1, highByte(value)); 
}

static word readWord(word address) {
  return word(EEPROM.read(address + 1), EEPROM.read(address));   
}

static void writeLong(word address, long value) {
  word lowWord = value % 65536;
  word highWord = value / 65536;
  writeWord(address, lowWord);
  writeWord(address + 2, highWord);
}

// Simple EEPROM wear leveling
static int getSaveFocuserPosAddress()
{
    for (byte x = 0; x < 20; x++)
    {
        int address = ROTATOR_EEPROM_START + 5 * x;
        if (EEPROM.read(address) == 0)
        {
            EEPROM.write(address, 0xFF);
            return address;
        }
    }
    // Array is full, erase it and start from 0 - takes about 100ms
    for (byte x = 0; x < 20; x++)
        EEPROM.write(ROTATOR_EEPROM_START + 5 * x, 0);
    EEPROM.write(ROTATOR_EEPROM_START, 0xFF);
    return ROTATOR_EEPROM_START;
}

static int getReadFocuserPosAddress()
{
    for (byte x = 0; x < 20; x++)
    {
        int address = ROTATOR_EEPROM_START + 5 * x;
        if (EEPROM.read(address) == 0)
            return ROTATOR_EEPROM_START + 5 * (x - 1);
    }
}