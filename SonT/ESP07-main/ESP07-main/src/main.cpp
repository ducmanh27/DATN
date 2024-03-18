/*************************************** INCLUDE ***************************************/
#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "myWifi.h"
#include "myMQTT.h"

/*************************************** GLOBLA VARIABLES ****************************************/

int nodeID = UNSUCCESSFUL_ID;
int roomID = UNSUCCESSFUL_ID;
unsigned long mainStart = millis();
unsigned long currenttime = millis();
unsigned long regtime = 20000;

/***************************************** SETUP ***************************************/

void setup()
{
  utilsInit();
  Serial.begin(115200);
  myMQTTInit();
  log("initialize successfully!");
}

/**************************************** LOOP ****************************************/

void loop()
{
  if (autoConnectWifi() == CONNECTED)
  {
    autoConnectMQTT();
  }
  else
  {
    ledDebug();
  }

  if (nodeID == UNSUCCESSFUL_ID || roomID == UNSUCCESSFUL_ID) // Check if the device has been registered or not.
  {
    // If the device is not registered, it will send a login message after RE_REGISTER_WAITING_TIME minutes.
    if (millis() - mainStart > RE_REGISTER_WAITING_TIME * 60 * 1000)
    {
      log("No registration!");
      mainStart = millis();
      registration();
    }
  }
  else
  {
    // If registered, the ESP07 will process the messages received from ATMEGA128.
    if (Serial.available())
    {
      StaticJsonDocument<SIZE_OF_JSON_MSG> doc;
      DeserializationError err = deserializeJson(doc, Serial.readString());
      // if right json format
      if (err == DeserializationError::Ok)
      {
        log("receive a msg from ATMEGA!");
        ledDebug(); // means there is a valid received message in json format
        // In case the operator is "register", it will send a registration message to the gateway
        if (doc["operator"] == operatorList[REGISTER])
        {
          log("ATMEGA Registers!");
          registration();
        }
        // In other cases, the message will be forwarded to the gateway with the corresponding topic.
        else
        {
          //ledDebug();
          if (doc["info"]["node_id"]==0)
          {
            doc["info"]["node_id"] = nodeID;
            doc["info"]["time"] = currenttime+(millis()-mainStart+regtime)/1000;
            for (int i = KEEP_ALIVE; i < NUM_OF_OPERATOR; i++)
            {
              if (doc["operator"] == operatorList[i])
              {
                mqttpublishJsonDoc(Topic[i], doc);
              }
            }
          }
          else
          {
            log("ATMEGA send error msg!");
          }
        }
      }
    }
    else
    {
      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.read();
    }
  }
}
