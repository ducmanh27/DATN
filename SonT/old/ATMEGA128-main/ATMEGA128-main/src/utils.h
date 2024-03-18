/**
 * @file actuator.h
 * @author HAIDT191811
 * @brief This library has global variables in program
 * @date 2023-06-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
#include <SPI.h>
#include <Timer.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "RTClib.h"

#include <math.h>

// #define ACTUATOR_NODE
#define SENSOR_NODE
#define SHT21_EN
#define MHZ19_EN

//#define BH1750_EN
//#define MAX9814_EN

//#define GP2Y1010AU0F_EN


// #define PIR_HC_SR501_EN
// #define OLED_EN
// #define MHZ16_EN
// #define TCS34725_EN

#define T_READ_MOTION 999
#define T_ACTUATOR_DATA (unsigned long)(30 * 1000 + 1)
#define T_SENSOR_DATA (unsigned long)(30 * 1000)
#define LEDPIN PIN_PB4
#define SIZE_OF_JSON 512
#define NUM_OF_OPERATOR 8


const char operatorList[NUM_OF_OPERATOR][30] = {"register_ack",
                                                "register",
                                                "keep_alive",
                                                "keep_alive_ack",
                                                "sensor_data",
                                                "actuator_data",
                                                "setpoint",
                                                "setpoint_ack"};
enum OPERATER
{
    REGISTER_ACK,
    REGISTER,
    KEEP_ALIVE,
    KEEP_ALIVE_ACK,
    SENSOR_DATA,
    ACTUATOR_DATA,
    SETPOINT,
    SETPOINT_ACK
};
extern RTC_DS3231 rtc;
extern SoftwareSerial UART1; // RX, TX. UART1 for MAX485
extern SoftwareSerial UART2; // RX, TX. UART2 for ESP07
extern SoftwareSerial UART3; // RX, TX. UART3 for MHZ16

void ledDebug();
int getOperator(String myOperator);

#endif