import paho.mqtt.client as mqtt

username = 'christian'
password = 'mqttpermission'

def on_connect(mqttc, obj, flags, reason_code, properties):
    print("reason_code: " + str(reason_code))

def replace_line(index, values):
    lines = open("RadioMapMatrix1.txt", 'r').readlines()
    lines[(int(index)+1)] = values
    out = open("RadioMapMatrix1.txt", 'w')
    out.writelines(lines)
    out.close()

def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    payload = msg.payload.decode()  # Decode the payload
    elements = payload.strip("[]").split(":")  # Extract elements from payload
    row_index = elements[0]  # Extract the row index
    data = " ".join(elements[0:])  # Join the rest of the elements with a space
    data = data + "\n"
    replace_line(row_index,data)

def on_subscribe(mqttc, obj, mid, reason_code_list, properties):
    print("Subscribed: " + str(mid) + " " + str(reason_code_list))


def on_log(mqttc, obj, level, string):
    print(string)


# If you want to use a specific client id, use
# mqttc = mqtt.Client("client-id")
# but note that the client id must be unique on the broker. Leaving the client
# id parameter empty will generate a random id for you.
mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.username_pw_set(username, password)
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
# Uncomment to enable debug messages
# mqttc.on_log = on_log
mqttc.connect("localhost", 1883, 60)
mqttc.subscribe("test_sensor_data", qos=1)

mqttc.loop_forever()