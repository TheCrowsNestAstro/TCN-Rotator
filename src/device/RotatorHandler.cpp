#include "device/RotatorHandler.h"

/*
ASCOM Return Messages

Successful transaction 0x0 (0) N/A
Property or method not implemented 0x400 (1024) 0x80040400
Invalid value 0x401 (1025) 0x80040401
Value not set 0x402 (1026) 0x80040402
Not connected 0x407 (1031) 0x80040407
Invalid while parked 0x408 (1032) 0x80040408
Invalid while slaved 0x409 (1033) 0x80040409
Invalid operation 0x40B (1035) 0x8004040B
Action not implemented 0x40C (1036) 0x8004040C
*/

RotatorHandler::RotatorHandler(ESP8266WebServer *server, RotatorDevice* rotatorDevice)
{
    _server = server;
    _rotatorDevice = rotatorDevice;
    serverTransactionID = 0;
}

void RotatorHandler::debugServerQuery()
{
    for (int i = 0; i < _server->args(); i++) {
        Log.traceln("%d" CR, i);
        Log.traceln("%s" CR, _server->argName(i));
        Log.traceln("%s" CR, _server->arg(i));
        Log.traceln("--------------------------");
    }
}

bool RotatorHandler::checkStringIsNumerical(String myString)
{
  uint16_t Numbers = 0;

  for(uint16_t i = 0; i < myString.length(); i++)
  {
     if (myString[i] == '0' || myString[i] == '1' || myString[i] == '2' || myString[i] == '3' || myString[i] == '4' || myString[i] == '5' || 
         myString[i] == '6' || myString[i] == '7' || myString[i] == '8' || myString[i] == '9')
    {
        Numbers ++;
    }
  }

  if(Numbers == myString.length())
  {
    return true;
  }
  else {
    return false;
  }
}

bool RotatorHandler::isFloat(String tString) {
    boolean decPt = false;
    int idx = 0;
  
    if (tString.charAt(0) == '+' || tString.charAt(0) == '-' ) {
      idx = 1;
      if (tString.charAt(1) == '.' ) {
        // check for +.???? or -.????
        idx = 2;
        decPt = true;
      };
    }
  
    if (tString.charAt(0) == '.' ) {
      // check for .????
      idx = 1;
      decPt = true;
    };
  
    // check for "+", "=", ".", "+.", "-.", empty string
    if ( tString.length() <= idx ) return false;
  
    for (int x = idx; x < tString.length(); x++) {
      if (tString.charAt(x) == '.') {
        if (decPt) return false;
        else decPt = true;
      }
      else if (tString.charAt(x) < '0' || tString.charAt(x) > '9') return false;
    }
    return true;
  }

bool RotatorHandler::verifyTransactionID()
{
    clientID = 0;(uint32_t)_server->arg("ClientID").toInt();
    transID = 0;

    String _clientTransactionId = "";

    if (_server->hasArg("ClientID"))
    {
        clientID = _server->arg("ClientID").toInt();
    }
    else if(_server->hasArg("clientid"))
    {
        clientID = _server->arg("clientid").toInt();
    }

    if (_server->hasArg("ClientTransactionId"))
    {
        //transID = (uint32_t)_server->arg("ClientTransactionId").toInt();
        _clientTransactionId = _server->arg("ClientTransactionId");
    }
    else if(_server->hasArg("clienttransactionid"))
    {
        //transID = (uint32_t)_server->arg("ClientTransactionId").toInt();
        _clientTransactionId = _server->arg("clienttransactionid");
    }
    else
    {
        _clientTransactionId = _server->arg("ClientTransactionID");
    }

    Log.traceln(F("CLIENTRANSACTIONID: %s" CR), _clientTransactionId.c_str());

    
    if(_clientTransactionId == " " || _clientTransactionId == "" || _clientTransactionId == NULL)
    {
        _server->send(400, "text/plain", "Bad number");
        return false;
    }

    if(checkStringIsNumerical(_clientTransactionId))
    {
        transID = _clientTransactionId.toInt();
        if(transID < 0)
        {
            //_server->send(400, "text/plain", "Bad number");
            transID = 0;
            return true;
        }
        else
        {
            return true;
        }
    }
    else
    {
        _server->send(400, "text/plain", "Bad number");
        return false;
    }
    
}

bool RotatorHandler::checkBoolValue(String valueName)
{
    
    if (_server->hasArg(valueName))
    {
        String valueString = _server->arg(valueName);
        if(valueString == " " || valueString == "" || valueString == NULL)
        {
            // _server->send(400, "text/plain", "Bad value");
            returnEmpty("Bad value", 1025);
            return false;
        }
        else if(valueString == "true" || valueString == "True" || valueString == "false" || valueString == "False")
        {
            return true;
        }
        else
        {
            returnEmpty("Bad value", 1025);
            return false;
        }  
    }
    else
    {
        _server->send(400, "text/plain", "Invalid parameter");
        return false;
    }
}

double RotatorHandler::checkDoubleValue(String valueName)
{
    if (_server->hasArg(valueName))
    {
        String valueString = _server->arg(valueName);
        if(valueString == " " || valueString == "" || valueString == NULL)
        {
            returnEmpty("Bad value", 1025);
            return false;
        }
        else if(isFloat(valueString))
        {
            return true;
        }

        returnEmpty("Bad value", 1025);
        return false;
        
    }
    else
    {
        _server->send(400, "text/plain", "Invalid parameter");
        return false;
    }
}

int RotatorHandler::checkIntValue(String valueName)
{
    String valueString;
    if (_server->hasArg(valueName))
    {
        valueString = _server->arg(valueName);
    }
    
    if(valueString)
    {
        if(valueString == " " || valueString == "" || valueString == NULL)
        {
            //returnEmpty("Bad value", 1025);
            _server->send(400, "text/plain", "Invalid parameter");
            return false;
        }
        else if(checkStringIsNumerical(valueString))
        {
            return true;
        }

        //returnEmpty("Bad value", 1025);
        _server->send(400, "text/plain", "Invalid parameter");
        return false;
        
    }
    else
    {
        _server->send(400, "text/plain", "Invalid parameter");
        return false;
    }
}

void RotatorHandler::transactionDetails()
{
    clientID = (uint32_t)_server->arg("ClientID").toInt();
    transID = (uint32_t)_server->arg("ClientTransactionID").toInt();
    deviceNumber = (uint32_t)_server->arg("device_number").toInt();
}



void RotatorHandler::incrementServerTransID()
{
    serverTransactionID++;
}

void RotatorHandler::returnEmpty(String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnBoolValue(bool val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    
    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnStringValue(String val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnIntValue(int val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnFloatValue(float val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnJsonArrayValue(JsonArray val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

void RotatorHandler::returnDoubleValue(double val, String errMsg, int errNr)
{
    incrementServerTransID();
    DynamicJsonDocument doc(1024);

    doc["Value"] = val;
    doc["ErrorMessage"] = errMsg;
    doc["ErrorNumber"] = errNr;
    doc["ClientTransactionID"] = transID;
    doc["ServerTransactionID"] = serverTransactionID;

    String output;
    serializeJson(doc, output);
    Log.traceln(F("Returning: %s" CR), output.c_str());

    _server->send(200, "application/json", output);
}

/*
 * ALPACA DEVICE MANAGEMENT
 */

 void RotatorHandler::handlerMgmtVersions()
 {
     Log.traceln("handlerMgmtVersions called...");
     incrementServerTransID();
 
     clientID = (uint32_t)_server->arg("ClientID").toInt();
     transID = (uint32_t)_server->arg("ClientTransactionID").toInt();
 
     if (_server->method() == HTTP_GET)
     {
         DynamicJsonDocument doc(1024);
 
         const size_t CAPACITY = JSON_ARRAY_SIZE(1);
         StaticJsonDocument<CAPACITY> devices;
         JsonArray array = devices.to<JsonArray>();
         array.add(1);
         // returnJsonArrayValue(array);
         
         doc["Value"] = array;
         doc["ErrorMessage"] = "";
         doc["ErrorNumber"] = 0;
         doc["ClientTransactionID"] = transID;
         doc["ServerTransactionID"] = serverTransactionID;
 
         String output;
         serializeJson(doc, output);
         Log.traceln(F("Returning: %s" CR), output.c_str());
         // return output;
         _server->send(200, "application/json", output);
         // sendDeviceSetup( returnCode, message, err );
         
     }
 }
 
 void RotatorHandler::handlerMgmtDescription()
 {
     Log.traceln("handlerMgmtDescription called");
 
     clientID = (uint32_t)_server->arg("ClientID").toInt();
     transID = (uint32_t)_server->arg("ClientTransactionID").toInt();
 
     if (_server->method() == HTTP_GET)
     {
         incrementServerTransID();
         DynamicJsonDocument val(1024);
 
         val["ServerName"] = SERVER_NAME; //"TCN Mount Switch";
         val["Manufacturer"] = MANUFACTURER; // "Christian Kardach";
         val["ManufacturerVersion"] = MANUFACTURER_VERSION; //"1.0";
         val["Location"] = LOCATION; //"SE";
 
         const size_t CAPACITY = JSON_ARRAY_SIZE(5);
         StaticJsonDocument<CAPACITY> devices;
         JsonArray array = devices.to<JsonArray>();
         array.add(val);
 
         //returnJsonArrayValue(array);
         
         DynamicJsonDocument doc(1024);
         doc["Value"] = val;
         doc["ErrorMessage"] = "";
         doc["ErrorNumber"] = 0;
         doc["ClientTransactionID"] = transID;
         doc["ServerTransactionID"] = serverTransactionID;
 
         String output;
         serializeJson(doc, output);
         Log.traceln(F("Returning: %s" CR), output.c_str());
         _server->send(200, "application/json", output);
         
     }
 }
 
 void RotatorHandler::handlerMgmtConfiguredDevices()
 {
     Log.traceln("handlerMgmtConfiguredDevices called");
 
     clientID = (uint32_t)_server->arg("ClientID").toInt();
     transID = (uint32_t)_server->arg("ClientTransactionID").toInt();
 
     if (_server->method() == HTTP_GET)
     {
         incrementServerTransID();
         DynamicJsonDocument val(1024);
         DynamicJsonDocument doc(1024);
 
         const size_t CAPACITY = JSON_ARRAY_SIZE(5);
         StaticJsonDocument<CAPACITY> devices;
 
         val["DeviceName"] = DEVICE_NAME; //"TCN Mount Switch";
         val["DeviceType"] = DEVICE_TYPE; //"Switch";
         val["DeviceNumber"] = DEVICE_NR; //0;
         val["UniqueID"] = UNIQUE_CLIENT_ID;
         JsonArray array = devices.to<JsonArray>();
         array.add(val);
 
         // returnJsonArrayValue(array);
         
         doc["Value"] = array;
         doc["ErrorMessage"] = "";
         doc["ErrorNumber"] = 0;
         doc["ClientTransactionID"] = transID;
         doc["ServerTransactionID"] = serverTransactionID;
 
         String output;
         serializeJson(doc, output);
         Log.traceln(F("Returning: %s" CR), output.c_str());
 
         _server->send(200, "application/json", output);
         
     }
 }
 
 void RotatorHandler::handlerConnected()
 {
     Log.traceln("handlerConnected called");
     if (_server->method() == HTTP_GET)
     {
         if(verifyTransactionID())
         {
             Log.traceln("handlerConnected GET called");
 
             String deviceType = _server->arg("device_type");
             //uint32_t deviceNumber = (uint32_t)_server->arg("device_number").toInt();
     
             returnBoolValue(_rotatorDevice->connected, "", 0);
         }
 
     }
 
     if (_server->method() == HTTP_PUT)
     {
         //debugServerQuery();
         if(verifyTransactionID())
         {
             Log.traceln("handlerConnected PUT called");
             if(checkBoolValue("Connected"))
             {
                 if(_server->arg("Connected") == "True" || _server->arg("Connected") == "true")
                 {
                    _rotatorDevice->connected = true;
                 }
                 else
                 {
                    _rotatorDevice->connected = false;
                 }
                 // switchDevice->connected = (bool)_server->arg("Connected");
                 returnEmpty("", 0);
             }
         }
         
     }
 }

/***********************************
 * ASCOM STANDARD
 **********************************/
void RotatorHandler::handlerDescriptionGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDescriptionGet called");
        returnStringValue(DESCRIPTION, "", 0);
    }
}

void RotatorHandler::handlerDriverInfoGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriverInfoGet called");
        returnStringValue(DRIVER_INFO, "", 0);
    }
}

void RotatorHandler::handlerDriverVersionGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriverVersionGet called");
        returnStringValue(DRIVER_VERSION, "", 0);
    }
}

void RotatorHandler::handlerInterfaceVersionGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerInterfaceVersionGet called");
        returnIntValue(1, "", 0);
    }
}

void RotatorHandler::handlerNameGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerNameGet called");
        returnStringValue(DEVICE_NAME, "", 0);
    }
}

void RotatorHandler::handlerSupportedActionsGet()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerSupportedActionsGet called");

        DynamicJsonDocument val(1024);
        const size_t CAPACITY = JSON_ARRAY_SIZE(13);
        StaticJsonDocument<CAPACITY> devices;
        JsonArray array = devices.to<JsonArray>();
        
        array.add("action");
        array.add("commandblind");
        array.add("commandbool");
        array.add("commandstring");

        returnJsonArrayValue(array, "", 0);
    }
}

void RotatorHandler::handleAction()
{
    if(verifyTransactionID())
    {
        Log.traceln("handleAction called");

        clientID = (uint32_t)_server->arg("ClientID").toInt();
        transID = (uint32_t)_server->arg("ClientTransactionID").toInt();

        String deviceType = _server->arg("device_type");
        //u_int32_t deviceNumber = (uint32_t)_server->arg("device_number").toInt();

        String action = _server->arg("Action");
        String parameters = _server->arg("Parameters");

        returnStringValue("ok", "", 0);
    }
}

void RotatorHandler::handleCommandBlind()
{
    if(verifyTransactionID())
    {
        Log.traceln("handleCommandBlind called");

        clientID = (uint32_t)_server->arg("ClientID").toInt();
        transID = (uint32_t)_server->arg("ClientTransactionID").toInt();

        String deviceType = _server->arg("device_type");
        // u_int32_t deviceNumber = (uint32_t)_server->arg("device_number").toInt();

        String command = _server->arg("Command");
        String raw = _server->arg("Raw");

        Log.traceln("%s" CR, command);
        Log.traceln("%s" CR, raw);

        returnEmpty("", 0);
    }
}

void RotatorHandler::handleCommandBool()
{
    if(verifyTransactionID())
    {
        Log.traceln("handleCommandBool called");

        clientID = (uint32_t)_server->arg("ClientID").toInt();
        transID = (uint32_t)_server->arg("ClientTransactionID").toInt();

        String deviceType = _server->arg("device_type");
        // u_int32_t deviceNumber = (uint32_t)_server->arg("device_number").toInt();

        String command = _server->arg("Command");
        String raw = _server->arg("Raw");

        Log.traceln("%s" CR, command);
        Log.traceln("%s" CR, raw);

        returnBoolValue(true, "", 0);
    }
}

void RotatorHandler::handleCommandString()
{
    if(verifyTransactionID())
    {
        Log.traceln("handleCommandBool called");

        clientID = (uint32_t)_server->arg("ClientID").toInt();
        transID = (uint32_t)_server->arg("ClientTransactionID").toInt();

        String deviceType = _server->arg("device_type");
        // u_int32_t deviceNumber = (uint32_t)_server->arg("device_number").toInt();

        String command = _server->arg("Command");
        String raw = _server->arg("Raw");

        Log.traceln("%s" CR, command.c_str());
        Log.traceln("%s" CR, raw.c_str());

        returnStringValue("ok", "", 0);
    }
}

/***********************************
 * ASCOM Alpaca Rotator
 **********************************/
// GET
/*
True if the Rotator supports the Reverse method.
*/
void RotatorHandler::handlerDriver0CanReverse()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0TargetPosition called");
        
        returnBoolValue(_rotatorDevice->getCanReverse(), "", 0);
    }
}

/*
True if the rotator is currently moving to a new position. False if the focuser is stationary.
*/
void RotatorHandler::handlerDriver0IsMoving()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0IsMoving called");

        returnBoolValue(_rotatorDevice->getIsMoving(), "", 0);
    }
}

/*
Returns the raw mechanical position of the rotator in degrees.
*/
void RotatorHandler::handlerDriver0MechanicalPosition()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0MechanicalPosition called");

        returnDoubleValue(_rotatorDevice->getMechanicalPosition(), "", 0);
    }
}

/*
Current instantaneous Rotator position, in degrees.
*/
void RotatorHandler::handlerDriver0Position()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0Position called");

        returnDoubleValue(_rotatorDevice->getPosition(), "", 0);
    }
}

/*
Returns the rotator’s Reverse state.
*/
void RotatorHandler::handlerDriver0GetReverse()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0GetReverse called");

        returnBoolValue(_rotatorDevice->getReverseState(), "", 0);
    }
}

/*
The minimum StepSize, in degrees.
*/
void RotatorHandler::handlerDriver0StepSize()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0StepSize called");
        
        returnDoubleValue(_rotatorDevice->getStepSize(), "", 0);
    }
}

/*
The destination position angle for Move() and MoveAbsolute().
*/
void RotatorHandler::handlerDriver0TargetPosition()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0TargetPosition called");
        
        returnDoubleValue(_rotatorDevice->getTargetPosition(), "", 0);
    }
}

// PUT
/*
Sets the rotator’s Reverse state.
*/
void RotatorHandler::handlerDriver0Reverse()
{
    if(verifyTransactionID())
    {
        
        Log.traceln("handlerDriver0Reverse called");

        if (_server->method() == HTTP_PUT)
        {
            debugServerQuery();
            if(checkBoolValue("Reverse"))
            {
                if(_server->arg("Reverse") == "True" || _server->arg("Reverse") == "true")
                {
                    _rotatorDevice->putReverseState(true);
                }
                else if(_server->arg("Reverse") == "False" || _server->arg("Reverse") == "false")
                {
                    _rotatorDevice->putReverseState(false);
                }
                // int rev = _server->arg("Reverse").toInt();        
                returnEmpty("", 0);
            }
        }
    }
}

/*
Immediately stop any Rotator motion due to a previous Move or MoveAbsolute method call.
*/
void RotatorHandler::handlerDriver0Halt()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0Halt called");

        if (_server->method() == HTTP_PUT)
        {
            _rotatorDevice->putHalt();
            returnEmpty("", 0);
        }
    }
}

/*
Causes the rotator to move Position degrees relative to the current Position value.
*/
void RotatorHandler::handlerDriver0Move()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0Move called");

        if (_server->method() == HTTP_PUT)
        {
            long position = (long)_server->arg("Position").toDouble();
            _rotatorDevice->putMove(position);
            returnEmpty("", 0);
        }
    }
}

/*
Causes the rotator to move the absolute position of Position degrees.
*/
void RotatorHandler::handlerDriver0MoveAbsolute()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0MoveAbsolute called");
        
        if (_server->method() == HTTP_PUT)
        {
            double position = _server->arg("Position").toDouble();
            _rotatorDevice->putMoveAbsolute(position);
            returnEmpty("", 0);
        }
    }
}

/*
Causes the rotator to move the mechanical position of Position degrees.
*/
void RotatorHandler::handlerDriver0MoveMechanical()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0MoveAbsolute called");

        if (_server->method() == HTTP_PUT)
        {
            double position = _server->arg("Position").toDouble();
            _rotatorDevice->putMoveMechanical(position);
            returnEmpty("", 0);
        }
    }
}

/*
Syncs the rotator to the specified position angle without moving it.
*/
void RotatorHandler::handlerDriver0Sync()
{
    if(verifyTransactionID())
    {
        Log.traceln("handlerDriver0MoveAbsolute called");

        if (_server->method() == HTTP_PUT)
        {
            double position = _server->arg("Position").toDouble();
            _rotatorDevice->putSync(position);
            returnEmpty("", 0);
        }
    }
}

// -----------------------------------------------------------------------
