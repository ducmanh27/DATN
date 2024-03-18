#include "sensor.h"

SHT2x sht;
MHZ19 myMHZ19;
BH1750 bh1750;
Adafruit_TCS34725 tcs34725;
int motion = 0, total = 0;

/**
 * @brief Round up 2 numbers
 *
 * @param f input
 * @return float
 */
float myRound(float f)
{
    f = ceil(f * 100) / 100;
    return f;
}

/**
 * @brief setup or begin sensor
 *
 */
void sensorInit()
{
#ifdef SHT21_EN
    sht.begin();
#endif
#ifdef MHZ19_EN
    myMHZ19.begin(CO2_UART);
#endif
#ifdef MHZ16_EN

#endif
#ifdef BH1750_EN
    while (!bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
       ledDebug();
#endif
#ifdef MAX9814_EN
    pinMode(MAX9814_OUT_PIN, INPUT);
#endif
#ifdef TCS34725_EN
    while (!tcs34725.begin())
        ;
    tcs34725.setIntegrationTime(TCS34725_INTEGRATIONTIME_614MS);
    tcs34725.setGain(TCS34725_GAIN_1X);
#endif
#ifdef GP2Y1010AU0F_EN
    pinMode(GP2Y1010AU0F_LED_PIN, OUTPUT);
    pinMode(GP2Y1010AU0F_OUT_PIN, INPUT);
#endif
#ifdef PIR_HC_SR501_EN
    pinMode(HC_SR501_OUT_PIN, INPUT);
#endif
#ifdef OLED_EN

#endif
ledDebug();
};

/**
 * @brief read MHZ16
 *
 * @return unsigned int
 */
unsigned int readMHZ16()
{
    // frame follow datasheet
    unsigned char frame[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    unsigned int hi = 0, lo = 0, ret = ERROR_RETURN;
    CO2_UART.write(frame, 9);
    for (int i = 0; i < 9; i++)
    {
        if (CO2_UART.available() > 0)
        {
            int ch = CO2_UART.read();
            if (i == 2)
                hi = (unsigned int)ch;
            if (i == 3)
                lo = (unsigned int)ch;
            if (i == 8)
                ret = (unsigned int)(hi * 256 + lo);
        }
    }
    return ret;
};
/**
 * @brief calibration MHZ16
 *
 */
void MHZ16Calibrate()
{
    // frame follow datasheet
    uint8_t frame[9] = {0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
    if (CO2_UART.available() > 0)
    {
        CO2_UART.write(frame, 9);
    }
}

/**
 * @brief read BH1750
 *
 * @return unsigned int
 */
unsigned int readBH1750()
{
    unsigned int light = 3*bh1750.readLightLevel();
    if (light < 0)
    {
        light = 0;
    }
    // else
    // {
    //     if (light > 40000.0)
    //     {
    //         // reduce measurement time - needed in direct sun light
    //         bh1750.setMTreg(32);
    //     }
    //     if (light > 10.0)
    //     {
    //         // typical light environment
    //         bh1750.setMTreg(69);
    //     }
    //     if (light <= 10.0)
    //     {
    //         // very low light environment
    //         bh1750.setMTreg(138);
    //     }
    // }
    light = 3*bh1750.readLightLevel();
    return light;
}

/**
 * @brief read MAX9814
 *
 * @return float
 */
float readMAX9814()
{
    unsigned long StartMillis = millis();

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // read sensor in 50ms, take (signalMax - signMin) to caculate sound
    while (millis() - StartMillis < 50)
    {
        unsigned int sample = analogRead(MAX9814_OUT_PIN);

        if (sample < 1024)
        {
            if (sample > signalMax)
            {
                signalMax = sample;
            }
            else if (sample < signalMin)
            {
                signalMin = sample;
            }
        }
    }
    double volts = (signalMax - signalMin) * 3.3 / 1024;

    // with the microphone sensivity is -44db, so VRms/PA is 0.006309
    return 20.0 * log10(volts / 0.006309);
    // return volts;
}

/**
 * @brief read dust
 *
 * @return float
 */
float readGP2Y1010AU0F()
{
    float voltage = 0;
    digitalWrite(GP2Y1010AU0F_LED_PIN, LOW); // turn on led in 280us
    delayMicroseconds(280);
    voltage = analogRead(GP2Y1010AU0F_OUT_PIN) * 5.0 / 1023; // dustval*5/1024
    delayMicroseconds(40);
    digitalWrite(GP2Y1010AU0F_LED_PIN, HIGH); // turn off led

    // according to dataSheet, linear value is from 1V to 3.55V
    if (voltage < 100.0 / 170)
    {
        voltage = 100.0 / 170;
    }
    if (voltage > 3.55)
    {
        voltage = 3.55;
    }

    return (170.0 * voltage - 100) / 5.0; // ug/m3
    // return voltage;
}

/*read motion immediately, take motion now*/
void readMotion(void *context)
{
    unsigned long StartMillis = millis();
    int count = 0, haveMotion = 0;

    // same read sound
    while (millis() - StartMillis < 25)
    {
        haveMotion += digitalRead(HC_SR501_OUT_PIN);
        count++;
    }
    if (haveMotion * 2 > count)
    {
        motion++;
    }
    else
        total++;
}

/*make decision of motion*/
int MOTION()
{
    // in total of time reading if number of motion > 5, this is a Motion
    if ((float)motion / total >= 0.5)
    {
        motion = 0;
        total = 0;
        return 1;
    }
    else
    {
        motion = 0;
        total = 0;
        return 0;
    }
}

/**
 * @brief make msg that included measured values and send to esp07
 */
void sendDataEnv(void *context)
{
    if (!Serial.available())
    {
        
        StaticJsonDocument<SIZE_OF_JSON> doc;
        doc["operator"] = operatorList[SENSOR_DATA];
        doc["status"] = 1;
        doc["info"]["node_id"] = *((int *)context);
        doc["info"]["time"] = rtc.now().unixtime(); // unix timestamp
/* temperature & humidity */
#ifdef SHT21_EN
        if (sht.isConnected())
        {
            sht.read();
            doc["info"]["temp"] = myRound(sht.getTemperature()); // celsius
            doc["info"]["hum"] = myRound(sht.getHumidity());     // %
        }
        else
        {
            sht.reset();
            doc["info"]["temp"] = ERROR_RETURN; // celsius
            doc["info"]["hum"] = ERROR_RETURN;  // %
        }
#else
        doc["info"]["temp"] = ERROR_RETURN; // celsius
        doc["info"]["hum"] = ERROR_RETURN;  // %
#endif
/* CO2 */
#ifdef MHZ19_EN
        doc["info"]["co2"] = myMHZ19.getCO2(); // ppm
#else
#ifdef MHZ16_EN
        doc["info"]["co2"] = readMHZ16(); // ppm
#else
        doc["info"]["co2"] = ERROR_RETURN; // ppm
#endif
#endif
        /* Light */
#ifdef BH1750_EN
        doc["info"]["light"] = readBH1750(); // lux
#else
        doc["info"]["light"] = ERROR_RETURN;
#endif
        /* Sound */
#ifdef MAX9814_EN
        doc["info"]["sound"] = myRound(readMAX9814());
#else
        doc["info"]["sound"] = ERROR_RETURN;
#endif
/* RGB */
#ifdef TCS34725_EN
        unsigned int r, b, g, c;
        tcs34725.enable();            // ENs the device
        tcs34725.setInterrupt(false); // Turn on LED
        delay(60);                    // Take 60ms to read
        tcs34725.getRawData(&r, &g, &b, &c);
        tcs34725.setInterrupt(true); // Turn off LED
        tcs34725.disable();          // Disables the device (lower power sleep mode)
        doc["info"]["red"] = r;
        doc["info"]["green"] = g;
        doc["info"]["blue"] = b;
#else
        doc["info"]["red"] = ERROR_RETURN;
        doc["info"]["green"] = ERROR_RETURN;
        doc["info"]["blue"] = ERROR_RETURN;
#endif
/* Dust */
#ifdef GP2Y1010AU0F_EN
        doc["info"]["dust"] = myRound(readGP2Y1010AU0F());
#else
        doc["info"]["dust"] = ERROR_RETURN;
#endif
/* Motion */
#ifdef PIR_HC_SR501_EN
        doc["info"]["motion"] = MOTION();
#else
        doc["info"]["motion"] = ERROR_RETURN;
#endif
        serializeJson(doc, Serial);
        ledDebug();
    }
    else
    {
      while (Serial.available() > 0)
        Serial.read(); // Flush all bytes in the "link" serial port buffer
    }
}
