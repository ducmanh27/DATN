/**
 * @file sensor.h
 * @author HAIDT191811
 * @brief This library include function to read value from sensors and rtc DS3231
 * @date 2023-06-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "utils.h"
#include "SHT2x.h"
#include "MHZ19.h"
#include <BH1750.h>
#include "Adafruit_TCS34725.h"

#define GP2Y1010AU0F_LED_PIN PIN_PA1
#define GP2Y1010AU0F_OUT_PIN PIN_PF1
#define HC_SR501_OUT_PIN PIN_PE7
#define MAX9814_OUT_PIN PIN_PF0
#define CO2_UART UART3
#define ERROR_RETURN -1

void sensorInit();
unsigned int readMHZ16();
void MHZ16Calibrate();
unsigned int readBH1750();
float readMAX9814();
float readGP2Y1010AU0F();
void readMotion(void *context);
int MOTION();
void sendDataEnv(void *context);
void handleSensorData(StaticJsonDocument<SIZE_OF_JSON> *doc);
#endif