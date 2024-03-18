#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
#include <SoftwareSerial.h>

#define DEBUG_ENABLE 0                  // 0 is not debug by UART
#define LEDPIN 2                        // led pin to bink for debugging
#define RE_REGISTER_WAITING_TIME (int)2 // after 2 minus if device doesn't register successfully, it re-registers

void utilsInit();
void ledDebug();
void log(const char *msg);
#endif