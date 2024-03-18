#include "myWifi.h"

int autoConnectWifi()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        log("wifi disconnection!");
        WiFi.mode(WIFI_STA); // Station mode
        WiFi.scanNetworks(); // Scan wifi
        int numberOfNetworksFound = WiFi.scanComplete();
        if (numberOfNetworksFound != 0)
        {
            log("connecting wifi!");
            for (int networkFound = 0; networkFound < numberOfNetworksFound; ++networkFound)
            {
                for (int NetworkKnown = 0; NetworkKnown < NUMBER_OF_KNOWN_NETWORKS; NetworkKnown++)
                {
                    if (WiFi.SSID(networkFound) == ssid[NetworkKnown])
                    {
                        /* find a network */
                        WiFi.begin(ssid[NetworkKnown], password[NetworkKnown]);
                        // WiFi.waitForConnectResult();
                        while (WiFi.status() != WL_CONNECTED)
                        {
                            static int waitTime = 0;
                            if (waitTime > MAXIMUM_CONNECTING_TIME)
                            {
                                break;
                            }
                            else
                            {
                                ledDebug();
                                delay(1000);
                                waitTime++;
                            }
                        };
                        log("wifi connected to: ");
                        log((WiFi.SSID()).c_str());
                        break;
                    }
                }
                if (WiFi.status() == WL_CONNECTED)
                {
                    log("cannot connect: ");
                    log((WiFi.SSID()).c_str());
                    break;
                }
            }
        }
        else
        {
            log("No wifi to connect!");
            return FOUND_NO_NETWORK;
        }
    }
    return CONNECTED;
};

