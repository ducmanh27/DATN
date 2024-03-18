#include "myMQTT.h"

WiFiClient espClient;
PubSubClient client(espClient);
char Topic[NUM_OF_OPERATOR][30] = {
    "farm/register",
    "farm/register",
    "farm/*/alive",
    "farm/*/alive",
    "farm/*/sync_node",
    "farm/*/sync_node",
    "farm/*/sync_node",
    "farm/*/sync_node",
    "farm/*/sensor",
    "farm/*/actuator",
    "farm/*/actuator",
    "farm/*/actuator"};

void myMQTTInit()
{
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}
/**
 * @brief send message containing mac to ATMEGA
 *
 */
void registration()
{
    StaticJsonDocument<SIZE_OF_JSON_MSG> doc;
    doc["operator"] = operatorList[REGISTER];
    doc["info"]["node_function"] = "sensor";
    doc["status"] = 0;
    doc["info"]["mac_address"] = WiFi.softAPmacAddress();
    mqttpublishJsonDoc(Topic[REGISTER], doc);
    
}
/**
 * @brief reconnect mqtt
 *
 */
void autoConnectMQTT()
{
    // Loop until we're reconnected mqtt
    while (!client.connected() && WiFi.status() == WL_CONNECTED)
    {
        log("mqtt disconnection!");

        // Create a random client ID
        String clientId = "device_";
        clientId += String(random(0xffff), HEX);

        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            log("mqtt connecting!");
            // Need only subscribe to topic Topic[REGISTER] & Topic[REGISTER_ACK] for registration
            client.subscribe((const char *)Topic[REGISTER]);
            client.subscribe((const char *)Topic[REGISTER_ACK]);
            log("mqtt connected!");
            registration();
        }
        else
        {
            // Wait 3 seconds before retrying
            delay(3000);
        }
    }
    client.loop();
}

/**
 * @brief Receive news from mqtt and immediately transfer it to ATMEGA128
 *
 * @param topic
 * @param payload
 * @param length
 */
void callback(char *topic, byte *payload, unsigned int length)
{
    //ledDebug();
    StaticJsonDocument<SIZE_OF_JSON_MSG> doc;
    DeserializationError err = deserializeJson(doc, (const char *)payload);
    if (err == DeserializationError::Ok)
    {
        ledDebug();
        // Topic[REGISTER_ACK]
        if (!((bool)strcmp(topic, Topic[REGISTER_ACK])))
        {
            // operatorList[REGISTER_ACK] = register_ack
            if (doc["operator"] == operatorList[REGISTER_ACK])
            {
                if (doc["status"] == 1 && WiFi.softAPmacAddress() == doc["info"]["mac_address"])
                {
                    log("register successfully!");
                    nodeID = doc["info"]["node_id"];
                    roomID = doc["info"]["room_id"];
                    realtime = doc["info"]["time"];
                    for (unsigned int i = 0; i < length; i++)
                    {
                        Serial.print((char)payload[i]);
                    }
                    sprintf(Topic[KEEP_ALIVE], "farm/%d/alive", roomID);
                    sprintf(Topic[KEEP_ALIVE_ACK], "farm/%d/alive", roomID);
                    sprintf(Topic[GATEWAY_DELETE], "farm/%d/sync_node", roomID);
                    sprintf(Topic[GATEWAY_DELETE_ACK], "farm/%d/sync_node", roomID);
                    sprintf(Topic[GATEWAY_ADD], "farm/%d/sync_node", roomID);
                    sprintf(Topic[GATEWAY_ADD_ACK], "farm/%d/sync_node", roomID);
                    sprintf(Topic[SENSOR_DATA], "farm/%d/sensor", roomID);
                    sprintf(Topic[ACTUATOR_DATA], "farm/%d/actuator", roomID);
                    sprintf(Topic[SETPOINT], "farm/%d/actuator", roomID);
                    sprintf(Topic[SETPOINT_ACK], "farm/%d/actuator", roomID);
                    for (int i = KEEP_ALIVE; i < NUM_OF_OPERATOR; i++)
                    {
                        client.subscribe((const char *)Topic[i]);
                    }
                }
            }
        }
        // Topic[KEEP_ALIVE]
        if (!((bool)strcmp(topic, Topic[KEEP_ALIVE])))
        {
            if (doc["operator"] == operatorList[KEEP_ALIVE])
            {
                if (WiFi.softAPmacAddress() == doc["info"]["mac_address"])
                {
                    log("receive keepAlive msg");
                    if (nodeID == doc["info"]["node_id"])
                    {
                        log("send keepAlive_ack msg");
                        doc["operator"] = operatorList[KEEP_ALIVE_ACK];
                        doc["status"] = 1;
                        mqttpublishJsonDoc(Topic[KEEP_ALIVE_ACK], doc);
                    }
                    else
                    {
                        log("wrong keepAlive msg! re-register!");
                        resetNode();
                    }
                }
            }
        }
        // Topic[DELETE]
        if (!((bool)strcmp(topic, Topic[GATEWAY_DELETE])))
        {
            if (doc["operator"] == operatorList[GATEWAY_DELETE])
            {
                if (WiFi.softAPmacAddress() == doc["info"]["mac_address"])
                {
                    log("This device is deleted!");

                    doc["operator"] = operatorList[GATEWAY_DELETE_ACK];
                    mqttpublishJsonDoc(Topic[GATEWAY_DELETE_ACK], doc);
                    resetNode();
                }
            }
        }
    }
}

void resetNode()
{
    roomID = 0;
    nodeID = 0;
    client.disconnect();
};

void mqttpublishJsonDoc(const char *topic, StaticJsonDocument<SIZE_OF_JSON_MSG> doc)
{
    char msg[SIZE_OF_JSON_MSG];
    serializeJson(doc, msg);
    client.publish(topic, msg);
}
