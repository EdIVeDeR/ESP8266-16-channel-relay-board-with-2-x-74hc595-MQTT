#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid =                "SSID";               // Change this value with your SSID name
const char* password =            "PASSWORD";           // Change this value with your Password
const char* mqttServer =          "192.168.1.100";      // MQTT server adress
const int   mqttPort =            1883;                 // MQTT server Port
const char* mqttUser =            "your_MQTT_username"; // MQTT server username
const char* mqttPassword =        "your_MQTT_password"; // MQTT server password
const char* topicIn =             "16ChRelay1In";       // MQTT Topic in
const char* topicOut =            "16ChRelay1Out";      // MQTT Topic in
const int   latchPin =            12;                   // Latch Pin of 74hc595
const int   clockPin =            13;                   // Clock  Pin of 74hc595
const int   dataPin =             14;                   // Data  Pin of 74hc595
int         oePin =               5;                    // Oe Pin of 74hc595  
const int   numRegisters =        2;                    // Number of 74hc595 on the board
const int   numRelays =           numRegisters * 8;     // Number of relay on the board obtain by multiplication of the number of relay by the number of 74hc595 on the board
bool        relayState            [numRelays];          // Store the state of each relay
int         connectionAttempts =  0;                    // Number of connection try for WiFi
int         failedAttempts =      0;                    // Number of connection try for MQTT server
IPAddress   subnet                (255, 255, 0, 0);     // Subnet Mask
IPAddress   gateway               (192, 168, 1, 1);     // Default Gateway
IPAddress   local_IP              (192, 168, 1, 184);   // Static IP Address for ESP8266


WiFiClient espClient;
PubSubClient client(espClient);

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print ("Message arrived on Topic:");
  Serial.print (topicOut);
  char message[20]={0x00};

  for(int i=0;i<length;i++)
    message[i]=(char)payload[i];
  message[length]=0x00;
  Serial.print (message);
  String str_msg = String(message);
  Serial.println(str_msg);
  String command = str_msg;
  command.trim();

  // Parse the MQTT command
  int relayNum = -1;
  bool newState = false;

  if(command.equals("ALL ON")) {
    for(int i=0;i<numRelays;i++) {
      setRelayState(i,true);
    }
  }
  else if(command.equals("ALL OFF")) {
    for(int i=0;i<numRelays;i++) {
      setRelayState(i,false);
    }
  }
  else if (command.startsWith("RELAY 1-8 ")) {
    String stateStr = command.substring(10);
    if (stateStr.equals("ON")) {
      for(int i=0;i<8;i++) {
        setRelayState(i,true);
      }
    } else if (stateStr.equals("OFF")) {
      for(int i=0;i<8;i++) {
        setRelayState(i,false);
      }
    }
  }
  else if (command.startsWith("RELAY 9-16 ")) {
    String stateStr = command.substring(11);
    if (stateStr.equals("ON")) {
      for(int i=8;i<numRelays;i++) {
        setRelayState(i,true);
      }
    } else if (stateStr.equals("OFF")) {
      for(int i=8;i<numRelays;i++) {
        setRelayState(i,false);
      }
    }
  }
  else if (command.startsWith("Relay #")) {
    int spaceIndex = command.indexOf(' ', 7);
    if (spaceIndex > 0) {
      String relayStr = command.substring(7, spaceIndex);
      relayNum = relayStr.toInt();
      if (relayNum >= 1 && relayNum <= numRelays) {
        String stateStr = command.substring(spaceIndex + 1);
        if (stateStr.equals("ON")) {
          newState = true;
        } else if (stateStr.equals("OFF")) {
          newState = false;
        }
      }
    }
  }
  else if (command.equals("RESET BOARD")) {
    // Reset the microcontroller
    ESP.reset();
  }

  // Set the state of the relay if a valid command was received
  if (relayNum >= 1 && relayNum <= numRelays) {
    setRelayState(relayNum - 1, newState);
  }  
}

void setRelayState(int relayNum, bool state) {
  // Set the state of a relay
  relayState[relayNum] = state;

  // Update the shift registers with the new relay state
  byte outputData[numRegisters];
  for (int i = 0; i < numRegisters; i++) {
    outputData[i] = 0;
  }
  for (int i = 0; i < numRelays; i++) {
    int regNum = i / 8;
    int bitNum = i % 8;
    if (relayState[i]) {
      bitSet(outputData[regNum], bitNum);
    }
  }
  digitalWrite(latchPin, LOW);
  for (int i = numRegisters - 1; i >= 0; i--) {
    shiftOut(dataPin, clockPin, MSBFIRST, outputData[i]);
  }
  digitalWrite(latchPin, HIGH);

  // Publish the confirmation message
  String message = String(relayNum) + " " + (state ? "ON" : "OFF");
  client.publish("16ChRelay1Out", message.c_str());

// Log the confirmation message to Serial for debugging
  Serial.println("Relay state changed: " + message);
}


void setup() {

  Serial.begin(115200);
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, LOW);  // enable outputs

  if (WiFi.config(local_IP, gateway, subnet)) {
     Serial.println("Static IP Configured");
  }
  else {
    Serial.println("Static IP Configuration Failed");
  }

WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.println("Connecting to WiFi...");
connectionAttempts++;

if (connectionAttempts >= 25) { // check if reached maximum attempts
Serial.println("Failed to connect after 50 attempts. Restarting...");
ESP.restart(); // reset board
}
}

Serial.println("Connected to WiFi"); // if connected before reaching 50 attempts, print success message.
client.setServer(mqttServer, mqttPort);
client.setCallback(mqttCallback);

while (!client.connected()) {
  if (failedAttempts >= 5) {
    ESP.reset(); // reset the ESP8266 board
  }
  
  if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
    client.subscribe(topicIn);
    failedAttempts = 0; // reset the failed attempts counter
  } else {
    delay(500);
    Serial.println("Connecting to MQTT server...");
    failedAttempts++;
  }
}

Serial.println("Connected to MQTT server");
client.publish(topicOut,"16 Channels Relay Board #1 is connect and listening on Topic 16ChRelay1In");

// Set all relays to off initially
  for (int i = 0; i < numRelays; i++) {
    setRelayState(i, false);
  }
}

void loop() {
client.loop();
}
