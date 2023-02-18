#pragma once
#include <Arduino.h>
#include "device/RotatorDevice.h"

#define DEVICE_RESPONSE "scopfocus Rotator ver 1008"
#define SCOPEFOCUS_VERSION "1008"

class SerialCommand
{
    public:
        SerialCommand(RotatorDevice* device);
        void serialRead();
        void command(String command);

    private:
        RotatorDevice* _device;
        String _inputString;

        void currentPosition();
        int stringToNumber(String inString);
        long stringToLong(String inString);
};