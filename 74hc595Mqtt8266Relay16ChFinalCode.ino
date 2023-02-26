#include "EspMQTTClient.h"

const char* clientId =            "16ChRelay2";         // Change to make it the only one with this name on your network
const char* ssid =                "SSID";          // Change this value with your SSID name
const char* password =            "PASSORD";       // Change this value with your Password
const char* mqttServer =          "192.168.1.100";      // MQTT server adress
const int   mqttPort =            1883;                 // MQTT server Port
const char* mqttUser =            "your_MQTT_username"; // MQTT server username
const char* mqttPassword =        "your_MQTT_password"; // MQTT server password
const char* topicIn =             "In";                 // MQTT Topic in
const char* topicOut =            "Out";                // MQTT Topic in
const int   latchPin =            12;                   // Latch Pin of 74hc595
const int   clockPin =            13;                   // Clock  Pin of 74hc595
const int   dataPin =             14;                   // Data  Pin of 74hc595
int         oePin =               5;                    // Oe Pin of 74hc595  
const int   numRegisters =        2;                    // Number of 74hc595 on the board
const int   numRelays =           numRegisters * 8;     // Number of relay on the board obtain by multiplication of the number of relay by the number of 74hc595 on the board
bool        relayState            [numRelays];          // Store the state of each relay

// Setup WiFi and Mqtt client 

EspMQTTClient client(
  ssid,         
  password,     
  mqttServer,   
  mqttUser,     
  mqttPassword, 
  clientId,     
  mqttPort      
);

// Function that change the relay state after a valid Mqtt command by  
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
  client.publish(String(clientId) + topicIn,message.c_str());

// Log the confirmation message to Serial for debugging
  Serial.println("Relay state changed: " + message);
}
// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  client.publish(String(clientId) + String(topicIn), "16 Channels Relay Board is connect and listening on Topic "+String(clientId)+"In"); // Publish a message to Topic put in variable topicOut. You can activate the retain flag by setting the third parameter to true
  client.subscribe(String(clientId) + topicIn, [](const String & payload) {                                                               // Subscribe to topic insert in variable topicOut and display received message to Serial
  Serial.println(payload);
  client.publish(String(clientId) + topicOut, payload);                                                                                   // Publish a message to Topic put in variable topicOut
  String str_msg = String(payload);
  Serial.println(str_msg);

// Extract command from string
if (str_msg.equals("ALL ON")) {
  // Switch all relays on
  for (int i = 0; i < 16; i++) {
    setRelayState(i, true);
  }
  Serial.println("All relays switched ON");
} else if (str_msg.equals("ALL OFF")) {
  // Switch all relays off
  for (int i = 0; i < 16; i++) {
    setRelayState(i, false);
  }
  Serial.println("All relays switched OFF");
} else if (str_msg.startsWith("RELAY 1-8 ")) {
  // Switch relays 1-8 on or off
  bool status = str_msg.endsWith("ON");
  for (int i = 0; i < 8; i++) {
    setRelayState(i, status);
  }
  if (status) {
    Serial.println("Relays 1-8 switched ON");
  } else {
    Serial.println("Relays 1-8 switched OFF");
  }
} else if (str_msg.startsWith("RELAY 9-16 ")) {
  // Switch relays 9-16 on or off
  bool status = str_msg.endsWith("ON");
  for (int i = 8; i < 16; i++) {
    setRelayState(i, status);
  }
  if (status) {
    Serial.println("Relays 9-16 switched ON");
  } else {
    Serial.println("Relays 9-16 switched OFF");
  }
} else if (str_msg.startsWith("Relay #")) {

  // Switch individual relays on or off
  int relayNumber = str_msg.substring(7, 9).toInt(); // Extract relay number from string
  relayNumber = relayNumber - 1;
  bool status = str_msg.endsWith("ON");
  setRelayState(relayNumber, status);
  if (status) {
    Serial.println("Relay " + String(relayNumber) + " switched ON");
  } else {
    Serial.println("Relay " + String(relayNumber) + " switched OFF");
  }
  } 
});
}

void setup()
{
  Serial.begin(115200);       // Start serial monitor debug
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, LOW);   // enable outputs
  client.enableDebuggingMessages();                                                                  // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();                                                                     // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA();                                                                                // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage((String(clientId) + String(topicOut)).c_str(), "I am going offline"); // You can activate the retain flag by setting the third parameter to true
  
}

void loop()
{
  client.loop();
}
