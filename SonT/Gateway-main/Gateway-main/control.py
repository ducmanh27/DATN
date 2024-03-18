from Libraly.mqtt import Client
from Libraly.dao import SqliteDAO
import json
import time as clock
import utils

dbName = utils.dbName
broker = utils.broker
port = utils.port
keepalive = utils.keepalive
thingTopics = utils.thingTopics
serverTopics = utils.serverTopics


def sendSetPointProcess() -> None:
    db = SqliteDAO(dbName)
    topic = []
    topic.append(serverTopics["send_setpoint"])
    topic.append(serverTopics["send_setpoint_ack"])
    client = Client(topic)
    client.connect(broker, port, keepalive)
    client.loop_start()
    while True:
        msg = client.msg_arrive()
        if (msg):
            msg = json.loads(msg)
            # print(msg)
            if msg["operator"] == "send_setpoint":
                colValuesTuple = []
                colValuesTuple.append(1)
                option = msg["option"]
                aim = None
                value = None
                time = None
                setPointMsg = None
                if option == "manual":
                    aim = "speed"
                else:
                    if "temp" in msg["info"]:
                        aim = "temp"
                    else:
                        aim = "co2"
                value = msg["info"]["aim"]
                time = msg["info"]["time"]

                colValuesTuple.append(option)
                colValuesTuple.append(aim)
                colValuesTuple.append(value)
                colValuesTuple.append(time)
                colValuesTuple = tuple(colValuesTuple)
                print(f"Store set point to DB {colValuesTuple}")
                db.insertOneRecord("SetPointControl",
                                   ["node_id", "option", "aim", "value", "time"], colValuesTuple)

                setPointMsg = {
                    "operator": "setPoint",
                    "id": 1,
                    "option": option,
                    "info": {
                                "speed": value,
                                "time": time,
                    },
                }
                client.publish(thingTopics["setPoint"], json.dumps(
                    setPointMsg))
                print(f'Send set point to things: {setPointMsg}')

                sendSetPointAckMsg = {
                    "operator": "send_setpoint_ack",
                    "info": {
                                "status": 1,
                    },
                }
                client.publish(topic[1], json.dumps(
                    sendSetPointAckMsg))
    client.loop_stop()
