This is a code for controlling a 8 or 16 channels relay board through MQTT messages.

At first you have to put your SSID and PASSWORD that fit to your network setting

In your serial monitor, ajust to 115200 baud, at begining, you will receive the topic in and out compose with the values of the clientId + "In" and clientId + "Out"

The code connects to a Wi-Fi network and an MQTT broker using the EspMQTTClient library. 

It sets up the pin configuration of the 74hc595 shift register and initializes the relayState array to store the state of each relay.

The onConnectionEstablished() function is called once the Wi-Fi and MQTT connections are established. This function publishes a message to the MQTT broker to confirm that the relay board is connected and listening to incoming messages. It also subscribes to the MQTT topic.

The setRelayState() function updates the state of a specific relay and publishes the new state to the MQTT broker. The function also updates the shift registers to set the new state of the relay.

The main function of the code receive incoming messages from the subscribed MQTT topic and processes them to change the state of the relays.

The function first prints the received message to the serial console for debugging purposes. Then, it checks the content of the message to identify which relay(s) to switch ON or OFF. If the message contains "ALL ON" or "ALL OFF", all relays are switched on or off, respectively. If the message contains "RELAY 1-8 ON", "RELAY 1-8 OFF", "RELAY 9-16 ON", or "RELAY 9-16 OFF", the specified relays are switched on or off accordingly using the setRelayState() function.

Overall, this code provides a simple and efficient way to control a 8 and 16-channel relay board through MQTT messages.

All the valids command to send to the clientId+"In" topic

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

