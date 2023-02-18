#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>
#include <AccelStepper.h>
#include "configuration.hpp"

class RotatorDevice
{
    public:
        RotatorDevice();
        AccelStepper* stepper;

        bool connected = false;
        String uniqueID = "";

        bool    getCanReverse();
        bool    getIsMoving();
        double  getMechanicalPosition();
        double  getPosition();
        int    getReverseState();
        double  getStepSize();
        double  getTargetPosition();

        void putReverseState(bool reverse);
        void putHalt();
        void putMove(long position);
        void putMoveAbsolute(double position);
        void putMoveMechanical(double position);
        void putSync(double position);

        void findHome();

        void update();

    private:
        bool _isFindingHome = false;

        bool _canReverse = true;
        bool _isMoving = false;
        double _mechanicalPosition = 0.0; // Degrees
        double _position = 0.0; // Degrees
        bool _reverseState = false;
        double _stepSize = 0.0;
        long _targetPosition = 0.0;
        double _targetMechanicalPosition = 0.0;

        bool _halt = false;

        // *************  this value must be changed for each specific setup ***************************************
        long maxSteps = 72000;  // **** steps needed for 2 complete revolutions.  prevent cord wrap.  
        //  Must also enter this value/720 = steps/degree for first time ASCOM driver setup, click "properties 
        // ******************************************************************************************************


        //void writeRelayData(int relay, int boolValue, double doubleValue);
        //byte relayData = B00000000;
};
