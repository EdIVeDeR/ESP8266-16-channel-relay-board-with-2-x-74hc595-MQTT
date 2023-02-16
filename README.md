# ESP8266-16-channel-relay-board-with-2-x-74hc595-MQTT
ESP8266 16 channel relay board with 2 x 74hc595 and MQTT 

This is a code for controlling a 16 channel relay board using an ESP8266 microcontroller and MQTT protocol.

First you will need to change SSID and Password at the begining of the code to match your own Network 

The code includes the necessary libraries for the ESP8266 WiFi and PubSubClient, as well as definitions for the WiFi credentials, MQTT server address, MQTT authentication details, and pin assignments for the 74hc595 shift registers used to control the relays.

The mqttCallback function handles incoming messages from the MQTT server and parses them to determine which relay(s) to turn on or off based on the received message payload. The setRelayState function updates the shift registers and sets the state of the relay(s) accordingly.

The setup function initializes the serial port and pin modes and attempts to connect to the WiFi network. If the connection is unsuccessful after 25 attempts, the ESP8266 will restart. Once connected to the WiFi network, the code connects to the MQTT server to post a connection confirmation on topic "16ChRelayOut" and subscribes to the "16ChRelayIn" topic to receive incoming messages.

The loop function continuously checks for incoming MQTT messages and sends a heartbeat message to the server every 10 seconds to indicate that the ESP8266 is still connected.

Here's the list of the differents caps sensitives commands you can send throught MQTT topic "16ChRelayIn" to control the relays:

-  ALL ON
-  ALL OFF
-  RELAY 1-8 ON
-  RELAY 1-8 OFF
-  RELAY 9-16 ON
-  RELAY 9-16 OFF
-  Relay #1 ON
-  Relay #1 OFF
-  Relay #2 ON
-  Relay #2 OFF
-  Relay #3 ON
-  Relay #3 OFF
-  Relay #4 ON
-  Relay #4 OFF
-  Relay #5 ON
-  Relay #5 OFF
-  Relay #6 ON
-  Relay #6 OFF
-  Relay #7 ON
-  Relay #7 OFF
-  Relay #8 ON
-  Relay #8 OFF
-  Relay #9 ON
-  Relay #9 OFF
-  Relay #10 ON
-  Relay #10 OFF
-  Relay #11 ON
-  Relay #11 OFF
-  Relay #12 ON
-  Relay #12 OFF
-  Relay #13 ON
-  Relay #13 OFF
-  Relay #14 ON
-  Relay #14 OFF
-  Relay #15 ON
-  Relay #15 OFF
-  Relay #16 ON
-  Relay #16 OFF
