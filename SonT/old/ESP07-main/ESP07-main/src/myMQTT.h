#ifndef MY_MQTT_H
#define MY_MQTT_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "utils.h"

#define SIZE_OF_JSON_MSG 512
#define UNSUCCESSFUL_ID 0
#define NUM_OF_OPERATOR 12
enum OPERATER
{
  REGISTER_ACK,
  REGISTER,
  KEEP_ALIVE,
  KEEP_ALIVE_ACK,
  GATEWAY_DELETE,
  GATEWAY_DELETE_ACK,
  GATEWAY_ADD,
  GATEWAY_ADD_ACK,
  SENSOR_DATA,
  ACTUATOR_DATA,
  SETPOINT,
  SETPOINT_ACK
};
const char operatorList[NUM_OF_OPERATOR][30] = {
    "register_ack",
    "register",
    "keep_alive",
    "keep_alive_ack",
    "gateway_delete",
    "gateway_delete_ack",
    "gateway_add",
    "gateway_add_ack",
    "sensor_data",
    "actuator_data",
    "setpoint",
    "setpoint_ack"};
extern char Topic[NUM_OF_OPERATOR][30];

// info you mqtt broker
const int mqtt_port = 1883;
//const char mqtt_server[] = "27.71.227.1";
//const char mqtt_server[] = "broker.hivemq.com";
const char mqtt_server[] = "192.168.1.193";
extern int nodeID;
extern int roomID;
extern unsigned long realtime;

void myMQTTInit();
void autoConnectMQTT();
void registration();
void callback(char *topic, byte *payload, unsigned int length);
void mqttpublishJsonDoc(const char * topic, StaticJsonDocument<SIZE_OF_JSON_MSG> doc);
void resetNode();
#endif