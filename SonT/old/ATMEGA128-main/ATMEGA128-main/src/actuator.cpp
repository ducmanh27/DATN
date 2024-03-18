#include "actuator.h"

void optoPinInit()
{
    pinMode(PIN_PA6, OUTPUT);
    pinMode(PIN_PA5, OUTPUT);
    pinMode(PIN_PA4, OUTPUT);
    pinMode(PIN_PA3, OUTPUT);
}

unsigned int crc_chk(unsigned char *data, unsigned char length)
{
    unsigned int reg_crc = 0xFFFF;
    while (length--)
    {
        reg_crc ^= *data++;
        for (int j = 0; j < 8; j++)
        {
            if (reg_crc & 0x01)
            { /* LSB(b0)=1 */
                reg_crc = (reg_crc >> 1) ^ 0xA001;
            }
            else
            {
                reg_crc = reg_crc >> 1;
            }
        }
    }
    return reg_crc;
}
void send485(unsigned char device, unsigned char adress_low, unsigned int value)
{
    unsigned int crc = 0;
    unsigned char byteSend[30];
    for (int i = 0; i < 8; i++)
    {
        switch (i) //[0x01 0x06 0x20 0x00 0x00 0x01 crc] 01 06 3 20 05669
        {
        case 0:
            byteSend[i] = device; // default of INVT: 0x01
            break;
        case 1:
            byteSend[i] = 0x06; // 06H (correspond to binary 0000 0110), write one word(Word) => Master write data to the inverter and one command can write one data other than multiple dates
            break;
        case 2:
            byteSend[i] = 0x20; // The address instruction of other function in MODBUS 20,21,30 or 50 0x14
            break;
        case 3:
            byteSend[i] = adress_low;
            break; //
        case 4:
            byteSend[i] = 0xff & (value >> 8);
            break;
        case 5:
            byteSend[i] = 0xff & value;
            break;
        case 6:
            crc = crc_chk(byteSend, 6);
            byteSend[i] = crc & 0xff;
            break;
        case 7:
            byteSend[i] = 0xff & (crc >> 8);
            break;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        RS485_UART.write(byteSend[i]);
    }
}
void FORWARD_RUNNING()
{
    send485(DEVICE_ADDRESS, 0x00, 0x0001);
}
void REVERSE_RUNNING()
{
    send485(DEVICE_ADDRESS, 0x00, 0x0002);
}
void STOP()
{
    send485(DEVICE_ADDRESS, 0x00, 0x0005); // 01 06 20 00 00 05
}
void SET_FREQUENCY()
{
    send485(DEVICE_ADDRESS, 0x01, 0x0032); // 32 H =50 D
}
void MAXFREQUENCY_FORWARD_RUNNING()
{
    send485(DEVICE_ADDRESS, 0x05, 0x0032);
}
void MAXFREQUENCY_REVERSE_RUNNING()
{
    send485(DEVICE_ADDRESS, 0x06, 0x0032);
}

void run_multispeed(int speed) // n = [0,15]
{
    // PIN_PA6 38
    // PIN_PA5 39
    // PIN_PA4 40
    // PIN_PA3 41
    int opto = 0;
    while (speed > 0)
    {
        digitalWrite(PIN_PA3 - opto, speed % 2);
        speed = speed / 2;
        opto++;
    }
}


/**
 * @brief make msg that included actuator values and send to esp07
 *{
  "operator": "actuatorData",
  "status": 0,
  "info": {
    "node_id": 3,
    "state": 1,
    "speed": "15%",
    "time": 1655396252
  }
}
 */
void sendDataAct(void *context)
{
  if (!Serial.available())
  {
    StaticJsonDocument<SIZE_OF_JSON> doc;
    doc["operator"] = operatorList[ACTUATOR_DATA];
    doc["status"] = 1;
    doc["info"]["node_id"] = *((int*)context);
    doc["info"]["time"] = rtc.now().unixtime();
    doc["info"]["state"] = 0;
    doc["info"]["speed"] = 0;
    serializeJson(doc, Serial);
  }
};
void handleActuatorData(StaticJsonDocument<SIZE_OF_JSON> *doc){};

void handleSetPoint(StaticJsonDocument<SIZE_OF_JSON> *doc)
{
    run_multispeed((int)(*doc)["info"]["speed"]);
    if (!Serial.available())
    {
        StaticJsonDocument<SIZE_OF_JSON> docAck;
        docAck["operator"] = operatorList[SETPOINT_ACK];
        docAck["status"] = 1;
        docAck["info"]["node_id"] = (*doc)["info"]["node_id"];
        docAck["info"]["time"] = rtc.now().unixtime();
        serializeJson(docAck, Serial);
    }
};

void handleSetPointAck(StaticJsonDocument<SIZE_OF_JSON> *doc){};