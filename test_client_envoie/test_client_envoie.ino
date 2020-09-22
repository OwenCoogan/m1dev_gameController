/*
MQTT Client steps
[x] Connect Wemos to the WIFI
[x] Connect Wemos to the MQTT Broker
[ ] Subscribe to a topic on the MQTT Broker
    - The led is on when topic payload is "ON"
    - The led is off when topic payload is "OFF"
[ ] Publish to a topic on the MQTT Broker
    - Each le led status change, publish the status of the led
*/

// Imports
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// For debugging
bool debug = true;

// Objects
WiFiClient espClient;
PubSubClient client(espClient);

// WIFI
#define WIFI_SSID "iotdrome"
#define WIFI_PASSWORD "glumdalklitch"

// MQTT
#define MQTT_CLIENT_ID "wemos"

// Topics
#define LIGHT_STATUS_TOPIC "light/status"
#define LIGHT_SWITCH_TOPIC "light/switch"

// Buffer for MQTT messages
char message_buff[100];

// Message timing
long lastMsg = 0;
long lastRecu = 0;

// Material
#define light_on LOW
#define light_off HIGH

// Status
bool light_status = "OFF";
bool initial = true;
//


// Setup function
void setup() {
  // For debugging
  Serial.begin(9600);

  // Set material
  pinMode(LED_BUILTIN, OUTPUT);

  // Turn the LED off
  digitalWrite(LED_BUILTIN, light_off);

  // Start WIFI connection
  setup_wifi();

  if (debug) {
    Serial.println("");
    Serial.print("Connection to MQTT broker");
    Serial.println("...");
  }

  // Define MQTT server connection
  client.setServer(WiFi.gatewayIP(), 1883);

  // Set callback function for MQTT sended message
  client.setCallback(callback);
}



// Loop function
void loop() {
  // Check connection
  if (!client.connected()) reconnect();

  // Start client loop
  client.loop();

  client.subscribe(LIGHT_SWITCH_TOPIC);

  // Check initial load
  if (initial) {
    // Publish status
    publish_status(false);

    // Tiggle initial
    initial = false;
  }
}


// Function to connect WIFI
void setup_wifi() {
  // Set interval
  delay(10);

  // Display Debug
  if (debug) {
    Serial.println("");
    Serial.print("Connection to WIFI");
  }

  // Connect WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Check WIFI connection status
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Display Debug
    if (debug) {
      Serial.print(".");
    }
  }

  // Success: display debug
  if (debug) {
    Serial.println("");
    Serial.println("");
    Serial.println("WIFI connection established!");
    Serial.print("=> SSID: ");
    Serial.println(WIFI_SSID);
  }
}

// Function to reconnect MQTT Broker
void reconnect() {

  // Loop to wait connection
  while (!client.connected()) {

    // MQTT Broker is connected
    if (client.connect(MQTT_CLIENT_ID)) {
      if (debug) {
        Serial.println("MQTT Broker connection established!");
        Serial.print("=> Local IP: ");
        Serial.println(WiFi.localIP());
      };
    }

    // Board is not connected
    else {
      // Display debug
      if (debug) {
        Serial.println("Connection fail with status: " + String(client.state()));
        Serial.println("New test in 5 seconds");
        Serial.println("...");
      }

      // Set interval
      delay(5000);
    }
  }
}

// Function to publish status from the board
void publish_status (bool lightStatus) {
  Serial.println(lightStatus);
  String topic = String(LIGHT_STATUS_TOPIC);
  // Check
  if (lightStatus) {
    // Publish light statuts "ON"
    client.publish(topic.c_str(), String("ON").c_str(), true);

    // Display debug
    if ( debug ) {
      Serial.println(String("[TOPIC] Sub: ") + topic + String(" ON"));
    };
  } else {
    // Publish light statuts "OFF"
    client.publish(topic.c_str(), String("OFF").c_str(), true);

    // Display debug
    if ( debug ) {
      Serial.println(String("[TOPIC] Sub: " + topic + " OFF"));
    };
  };
}
//

// Function to switch light from topic message
void switchLight ( char* level,  bool lightStatus){
  if(level == "HIGH"){
    digitalWrite(LED_BUILTIN, light_on);
    // Publish status
    publish_status(lightStatus);
  } else{
    digitalWrite(LED_BUILTIN, light_off);
    // Publish status
    publish_status(lightStatus);
  };
}



// Function for MQTT Broker callback (from https://bit.ly/2pjkgzI)
void callback(char* topic, byte* payload, unsigned int length) {

  // Display debug
  if ( debug ) {
    Serial.println("...");
    Serial.println("");
    Serial.println("[TOPIC] Pub: " + String(topic));
  }

  // Character buffer with ending null terminator (string)
    int i = 0;
    for(i=0; i<length; i++) message_buff[i] = payload[i];
    message_buff[i] = '\0';
    String msgString = String(message_buff);

    // Display debug
    if ( debug ) {
      Serial.println("[TOPIC] Payload: " + msgString);
    }
  //

  // switch ligth status from topic message
  msgString == "ON" ? switchLight("HIGH", true) : switchLight("LOW", false);
}
