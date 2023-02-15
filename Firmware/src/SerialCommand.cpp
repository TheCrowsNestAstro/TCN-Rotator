#include "SerialCommand.h"

SerialCommand::SerialCommand(RotatorDevice *device)
{
    _device = device;
}

void SerialCommand::serialRead()
{
    while (Serial.available() > 0)
    {
        char inChar = (char)Serial.read();
        if (inChar == '#')
        {
            Serial.println(_inputString);
            command(_inputString);
            _inputString = "";
        }
        else
        {
            _inputString += inChar;
        }
    }
}

void SerialCommand::command(String command)
{
    String param = command.substring(2);

    switch (command.charAt(0))
    {
    case '#':
        Serial.print(DEVICE_RESPONSE);
        break;

    case 'G': // Return current position  (was 'P' on orig Jolo)
        currentPosition();
        break;
    case 'P':
    {   // Save current position
        // the driver needs to multiply by 100 to avoid dealing with decimal points in the arduino code
        _device->putSync(stringToLong(param));
        break;
    }
    case 'H': // find home
        _device->findHome();
        break;
    case 'M': // Move focuser to new position
    {
        _device->putMoveMechanical(stringToLong(param));
        break;
    }
    case 'S': // Stop movement  (Halt)
        _device->putHalt();
        break;
    case 'R':
        int value;
        value = (stringToNumber(param));
        if (value == 0)
            _device->putReverseState(false);
        else
            _device->putReverseState(true);
        break;

    case 'C': // continuos hold

        int value2;
        value2 = (stringToNumber(param));
        if (value2 == 0)
        {
            //Hold(false);
        }
        else
        {
            //Hold(true);
        }
        break;
    case 'V':
        Serial.print("V ");
        Serial.print(SCOPEFOCUS_VERSION); //*** was println
        Serial.println("#");

        break;
    default:
        Serial.print("ERR:");
        Serial.println(byte(command.charAt(1)), DEC);
    }
}

void SerialCommand::currentPosition()
{
    Serial.print("P ");

    Serial.print(_device->getPosition()); // was println
    //**** try adding is moving
    Serial.print(";"); // delimiter
    Serial.print("M ");
    if (_device->stepper->distanceToGo() == 0)
        Serial.print("false"); // false
    else
        Serial.print("true"); // true

    Serial.println("#"); //  add stop bit
}

int SerialCommand::stringToNumber(String inString)
{
    int i, value = 0, length;
    length = inString.length();
    for (i = 0; i < length; i++)
    {
        value = (10 * value) + inString.charAt(i) - (int)'0';
    }
    return value;
}

long SerialCommand::stringToLong(String inString)
{

    long value = 0;
    int i, length;
    length = inString.length();
    for (i = 0; i < length; i++)
    {
        value = (10 * value) + inString.charAt(i) - (int)'0';
    }

    return value;
}