/*************************************** INCLUDE ***************************************/
#include "utils.h"
// #include "sync.h"
#ifdef SENSOR_NODE
#include "sensor.h"
#endif
#ifdef ACTUATOR_NODE
#include "actuator.h"
#endif

/*********************************** GLOBAL VARIABLES **********************************/
SoftwareSerial UART1(PIN_PD2, PIN_PD3); // RX, TX UART1 for MAX485
SoftwareSerial UART2(PIN_PE2, PIN_PE3); // RX, TX UART2 for ESP07
SoftwareSerial UART3(PIN_PE4, PIN_PE5); // RX, TX UART3 for MHZ16/MHZ19
int nodeID = 0;
unsigned long mainStart = millis();
/***********************************STATIC VARIABLES ***********************************/
Timer t;
RTC_DS3231 rtc;
/***************************************** SETUP ***************************************/
void setup()
{
  
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  UART1.begin(9600);
  UART2.begin(9600);
  UART3.begin(9600);
  rtc.begin();
  ledDebug();
  // registration();
#ifdef ACTUATOR_NODE
  optoPinInit();
  t.every(T_ACTUATOR_DATA, sendDataAct, (void *)(&nodeID));
#endif
#ifdef SENSOR_NODE
  sensorInit();
#ifdef PIR_HC_SR501_EN
  t.every(T_READ_MOTION, readMotion, (void *)0);
#endif
  t.every(T_SENSOR_DATA, sendDataEnv, (void *)(&nodeID));
  
#endif

  // ready

}

/**************************************** LOOP ****************************************/
void loop()
{

  t.update();
//   if (nodeID != 0)
//   {
    
//     t.update(); // update timer
//   }
//   else
//   {
//     if (millis() - mainStart > 30 * 1000)
//     {
//       mainStart = millis();
//       StaticJsonDocument<SIZE_OF_JSON> doc;
//       doc["operator"] = operatorList[REGISTER];
//       doc["info"]["device_type"] = "sensor";
//       doc["status"] = 0;
//       doc["info"]["macAddress"] = "";
//       serializeJson(doc, Serial);
//     }
//   }

//   // process message
//   if (Serial.available())
//   {
//     StaticJsonDocument<SIZE_OF_JSON> doc;
//     DeserializationError err = deserializeJson(doc, Serial.readString());

//     if (err == DeserializationError::Ok)
//     {
//       ledDebug();                           // signal for received data
//       switch (getOperator(doc["operator"])) // swich operator
//       {
//       case REGISTER_ACK:
//         if (doc["status"] == 1)
//         {
//           nodeID = doc["info"]["node_id"];
//         }
//         break;
//       case REGISTER:
//         // have no thing to do in this case
//         break;
//       case KEEP_ALIVE:
//         // have no thing to do in this case
//         break;
//       case KEEP_ALIVE_ACK:
//         // have no thing to do in this case
//         break;
// #ifdef SENSOR_NODE
//       case SENSOR_DATA:
//         break;
// #endif
// #ifdef ACTUATOR_NODE
//       case ACTUATOR_DATA:
//         handleActuatorData(&doc);
//         break;
//       case SETPOINT:
//         handleSetPoint(&doc);
//         break;
//       case SETPOINT_ACK:
//         handleSetPointAck(&doc);
//         break;
// #endif
//       default:
//         break;
//       }
//     }
//     else
//     {
//       while (Serial.available() > 0)
//         Serial.read(); // Flush all bytes in the "link" serial port buffer
//     }
//   }
}
