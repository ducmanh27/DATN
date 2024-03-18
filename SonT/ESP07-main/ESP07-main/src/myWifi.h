#ifndef MY_WIFI
#define MY_WIFI
#include <ESP8266WiFi.h>
#include "utils.h"

// ssid and password wifi
#define NUMBER_OF_KNOWN_NETWORKS 10
const char ssid[NUMBER_OF_KNOWN_NETWORKS][50] = {
    // "do tien hai",
    // "678 Lang-2G",
    // "678 Lang-5G",
    // "EVSELab",
    "IPAC LAB 2.4G",
    "TRANSON1"
    };
const char password[NUMBER_OF_KNOWN_NETWORKS][50] = {
    // "dotienhai",
    // "868686868686",
    // "868686868686",
    // "EVSELab0111200",
    "12345687",
    "transon999"
    };
#define MAXIMUM_CONNECTING_TIME 5 // 1s per time
#define CONNECTED 1
#define FOUND_NO_NETWORK 2

int autoConnectWifi();

#endif