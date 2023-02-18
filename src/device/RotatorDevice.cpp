#include "device/RotatorDevice.h"

RotatorDevice::RotatorDevice()
{
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);

    // pinMode(EN_PIN, OUTPUT);

    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(EN_PIN, LOW);

    stepper = new AccelStepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
    /*
    setMaxSpeed Info

    5000 works good
    500000 for confrom test
    1000 geared rotator
    10000 for geared stepper
    200 for non-geared large stepper(500 max) also may depend on what else is on loop()
    */
    stepper->setMaxSpeed(500.0f);

    /*
    1000 for geared rotator
    10000 for geared stepper
    1000 for non-geared large stepper
    */
    stepper->setAcceleration(1000.0f);

    stepper->setCurrentPosition(0);
    // stepper->setCurrentPosition(readFocuserPos());
    // stepper->setSpeed(45);
    // stepper->moveTo(400*9);
}

/*
Indicates whether the Rotator supports the Reverse method.
True if the Rotator supports the Reverse method.
*/
bool RotatorDevice::getCanReverse()
{
    return _canReverse;
}

/*
Indicates whether the rotator is currently moving.
True if the rotator is currently moving to a new position. False if the focuser is stationary.
*/
bool RotatorDevice::getIsMoving()
{
    Log.traceln("RotatorDevice::getIsMoving()");
    return _isMoving;
}

/*
Returns the rotator's mechanical current position.
Returns the raw mechanical position of the rotator in degrees.
*/
double RotatorDevice::getMechanicalPosition()
{
    Log.traceln("RotatorDevice::getMechanicalPosition()");
    // In Degrees
    return _mechanicalPosition;
}

/*
Returns the rotator's current position.
Current instantaneous Rotator position, in degrees.
*/
double RotatorDevice::getPosition()
{
    Log.traceln("RotatorDevice::getPosition()");
    // In Degrees

    return (double)(stepper->currentPosition() / STEPS_PER_DEGREE); //((stepper->currentPosition()*0.9)/9);
}

/**
 * @brief Returns the rotator’s Reverse state.
 *
 * @return true
 * @return false
 */
int RotatorDevice::getReverseState()
{
    Log.traceln("RotatorDevice::getReverseState()");
    if (_reverseState == true)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
Returns the minimum StepSize
The minimum StepSize, in degrees.
*/
double RotatorDevice::getStepSize()
{
    _stepSize = 0.9;
    return _stepSize;
}

/*
Returns the destination position angle.
The destination position angle for Move() and MoveAbsolute().
*/
double RotatorDevice::getTargetPosition()
{
    Log.traceln("RotatorDevice::getTargetPosition()");
    return (double)(_targetPosition / STEPS_PER_DEGREE);
}

// PUT
/*
Sets the rotator’s Reverse state.

*/
void RotatorDevice::putReverseState(bool reverse)
{
    Log.traceln("RotatorDevice::putReverseState(bool reverse)");
    _reverseState = reverse;
}

/*
Immediatley stops rotator motion.
Immediately stop any Rotator motion due to a previous Move or MoveAbsolute method call.
*/
void RotatorDevice::putHalt()
{
    Log.traceln("RotatorDevice::putHalt()");
    stepper->stop();
    _halt = true;
}

/*
Moves the rotator to a new relative position.
Causes the rotator to move Position degrees relative to the current Position value.
*/
void RotatorDevice::putMove(long position)
{
    Log.traceln("RotatorDevice::putMove(long position)");
    Log.traceln("RotatorDevice::putMove(double position)");
    _targetPosition = stepper->currentPosition() + (STEPS_PER_DEGREE * position);
    stepper->moveTo(_targetPosition);
}

/*
Moves the rotator to a new absolute position.
Causes the rotator to move the absolute position of Position degrees.
*/
void RotatorDevice::putMoveAbsolute(double position)
{
    Log.traceln("RotatorDevice::putMoveAbsolute(double position)");
    _targetPosition = position;
}

/*
Moves the rotator to a new raw mechanical position.
Causes the rotator to move the mechanical position of Position degrees.
*/
void RotatorDevice::putMoveMechanical(double position)
{
    Log.traceln("RotatorDevice::putMoveMechanical(double position)");
    long steps = STEPS_PER_DEGREE * position;
    _targetMechanicalPosition = steps;
}

/*
Syncs the rotator to the specified position angle without moving it.
Causes the rotator to sync to the position of Position degrees.
*/
void RotatorDevice::putSync(double position)
{
    _targetMechanicalPosition = position;
}

void RotatorDevice::findHome()
{
    /*
    HEState = digitalRead(HOME_PIN);
    // Serial.println("Finding Home");  // leave for testing
    homeFound = false;
    findingHome = true;
    if (HEState == HIGH)
    {                                                                     // sensor goes low by magnet
        moveStepper(stepper.currentPosition() - findHomeStepSize, false); // ***  adjust speed by changing the -x number 100 seems to work well

        findingHome = true;
    }
    if (HEState == LOW)
    {
        if (digitalRead(HOME_PIN) == LOW)
        { // double check sensor state
            homeFound = true;
            findingHome = false;
            saveCurrentPos(maxSteps / 2); // allow for 2 complete revolustions.  centered at 360 degrees
            Serial.print("H ");
            Serial.print("true"); // true
            Serial.println("#");  //  add stop bit
            Serial.println(HEState);
        }
    }
    */
}
// ------------------------------------------------------------------


void RotatorDevice::update()
{
    if (stepper == nullptr)
    {
        return;
    }

    if (stepper->distanceToGo() == 0)
    {
        _isMoving = false;
        // stepper->moveTo(_targetPosition);
    }
    else
    {
        _isMoving = true;
    }

    stepper->run();
    /*
    if (_isFindingHome == true)
    {
        stepper->moveTo(_targetPosition);
        return;
    }
    */
    /*
     if (_targetPosition != stepper->currentPosition())
     {
         if (_targetPosition < 0 || _targetPosition > maxSteps)
         {
             Serial.println("out of range");
             return;
         }
         else
         {
             stepper->moveTo(_targetPosition);
         }
     }

     stepper->run();
     */
}
