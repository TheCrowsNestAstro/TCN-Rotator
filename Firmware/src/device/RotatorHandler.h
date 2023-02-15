#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include "device/RotatorDevice.h"
#include "configuration.hpp"

class RotatorHandler
{
    ESP8266WebServer* _server;

    public:
        RotatorHandler(ESP8266WebServer* server, RotatorDevice* rotatorDevice);
        
        void handlerMgmtVersions();
        void handlerMgmtDescription();
        void handlerMgmtConfiguredDevices();

        void handlerConnected();
        void handlerDescriptionGet();
        void handlerDriverInfoGet();
        void handlerDriverVersionGet();
        void handlerInterfaceVersionGet();
        void handlerNameGet();
        void handlerSupportedActionsGet();

        void handleAction();
        void handleCommandBlind();
        void handleCommandBool();
        void handleCommandString();

        // Device Specific Actions
        // GET
        void handlerDriver0CanReverse();
        void handlerDriver0IsMoving();
        void handlerDriver0MechanicalPosition();
        void handlerDriver0Position();
        void handlerDriver0GetReverse();
        void handlerDriver0StepSize();
        void handlerDriver0TargetPosition();

        // PUT
        void handlerDriver0Reverse();
        void handlerDriver0Halt();
        void handlerDriver0Move();
        void handlerDriver0MoveAbsolute();
        void handlerDriver0MoveMechanical();
        void handlerDriver0Sync();
   
    private:
        uint32_t transID;
        uint32_t clientID;
        u_int32_t deviceNumber;
        uint32_t serverTransactionID = 0;
        String uniqueID = "4431281c-8560-4ad7-982f-5a6e507dda19";

        RotatorDevice* _rotatorDevice;

        void incrementServerTransID();

        void returnEmpty(String errMsg, int errNr);

        void returnStringValue(String val, String errMsg, int errNr);
        void returnBoolValue(bool val, String errMsg, int errNr);
        void returnIntValue(int val, String errMsg, int errNr);
        void returnFloatValue(float val, String errMsg, int errNr);
        void returnJsonArrayValue(JsonArray val, String errMsg, int errNr);
        void returnDoubleValue(double val, String errMsg, int errNr);

        void debugServerQuery();
        void transactionDetails();
        
};
