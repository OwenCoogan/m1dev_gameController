/*
  Simple wemos D1 mini  MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to the provided access point using dhcp, using ssid and pswd
  It connects to an MQTT server ( using mqtt_server ) then:
  - publishes "connected"+uniqueID to the [root topic] ( using topic ) 
  - subscribes to the topic "[root topic]/composeClientID()/in"  with a callback to handle
  - If the first character of the topic "[root topic]/composeClientID()/in" is an 1, 
    switch ON the ESP Led, else switch it off
  - after a delay of "[root topic]/composeClientID()/in" minimum, it will publish 
    a composed payload to 
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. 
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define buttonPin1 D3
#define buttonPin2 D4
#define buttonPin3 D5
#define buttonPin4 D6

#define BUTTON_A "button/A" 
#define BUTTON_B "button/B"
#define BUTTON_C "button/C"
#define BUTTON_D "button/D"

int red_light_pin = D1; // PIN A METTRE A JOUR
int green_light_pin = D2; // PIN A METTRE A JOUR
int blue_light_pin =D7 ; // PIN A METTRE A JOUR

int buttonStateA = 0;
int buttonStateB = 0;
int buttonStateC = 0;
int buttonStateD = 0;

int buttonStateAPrevious= 1;
int buttonStateBPrevious= 1;
int buttonStateCPrevious= 1;
int buttonStateDPrevious= 1;

int keyUp = HIGH; // Permet d'identifier le changement d'état du bouton
int user=1;

// Update these with values suitable for your network.

const char* ssid = "HUAWEI P40 lite";
const char* pswd = "Valentin18!";
const char* mqtt_server = "192.168.43.54";


const char* topic = "manette";    // rhis is the [root topic]

long timeBetweenMessages = 1000 * 20 * 1;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

int status = WL_IDLE_STATUS;     // the starting Wifi radio's status

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {
              RGB_color(255, 0, 0); // Red
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

String composeClientID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String clientId;
  clientId += "esp-";
  clientId += macToStr(mac);
  return clientId;
}

void reconnect() {
  // Loop until were reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    RGB_color(0, 255, 0); // Green
    //Etat de la led rouge//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String clientId = composeClientID() ;
    clientId += "-";
    clientId += String(micros() & 0xff, 16); // to randomise. sort of

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
          RGB_color(0, 255, 0); // Green
      // Once connected, publish an announcement...// Mettre l'état de la LED EN VERT constant /////////////////////////////////////////////////////////////////////////////////////////////
      client.publish(topic, ("connected " + composeClientID()).c_str() , true );
      // ... and resubscribe
      // topic + clientID + in
      String subscription;
      subscription += topic;
      //subscription += "/";
      //subscription += composeClientID() ;
      //subscription += "/in";
      client.subscribe(subscription.c_str() );
      Serial.print("subscribed to : ");
      Serial.println(subscription);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" wifi=");
      Serial.print(WiFi.status());
      Serial.println(" try again in 5 seconds");
          RGB_color(255, 0, 0); // Red
      // Etat de la LED rouge constant /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void setup() {
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
    pinMode(buttonPin3, INPUT);
        pinMode(buttonPin4, INPUT);

        pinMode(red_light_pin, OUTPUT); // PAS SUR//////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Read button state
  
      buttonStateA = digitalRead(buttonPin1);
  buttonStateB = digitalRead(buttonPin2);
    buttonStateC = digitalRead(buttonPin3);
        buttonStateD = digitalRead(buttonPin4);

    
  
  // confirm still connected to mqtt server
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // check button state
  if(checkState(buttonStateA, buttonStateAPrevious)){
        buttonStateA == 1 ? Serial.println("A button is HIGH") : Serial.println("A button is Low") ;
        String payload = "{";
        payload += "\"button\":\"A\"";
        payload += ", ";
        buttonStateA == 1 ? payload += "\"value\":\"HIGH\"" : payload += "\"value\":\"LOW\"" ;
        payload += ", ";
        payload += "\"User\": 1 ";
        payload += "}"; 
        String pubTopic; 
        pubTopic += topic;
        client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
        Serial.print("Publish topic: ");
        Serial.println(pubTopic);
        Serial.print("Publish message: ");
        Serial.println(payload);
        buttonStateAPrevious = buttonStateA;
  }
  
    if(checkState(buttonStateB, buttonStateBPrevious)){
        buttonStateB == 1 ? Serial.println("B button is HIGH") : Serial.println("B button is Low") ;
        String payload = "{";
        payload += "\"button\":\"B\"";
        payload += ", ";
        buttonStateB == 1 ? payload += "\"value\":\"HIGH\"" : payload += "\"value\":\"LOW\"" ;
        payload += ", ";
        payload += "\"User\": 1 ";
        payload += "}"; 
        String pubTopic; 
        pubTopic += topic;
        client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
        Serial.print("Publish topic: ");
        Serial.println(pubTopic);
        Serial.print("Publish message: ");
        Serial.println(payload);
        buttonStateBPrevious = buttonStateB;

    }
    if(checkState(buttonStateC, buttonStateCPrevious)){
        buttonStateC == 1 ? Serial.println("C button is HIGH") : Serial.println("C button is Low") ;
        String payload = "{";
        payload += "\"button\":\"C\"";
        payload += ", ";
        buttonStateC == 1 ? payload += "\"value\":\"HIGH\"" : payload += "\"value\":\"LOW\"" ;
        payload += ", ";
        payload += "\"User\": 1 ";
        payload += "}"; 
        String pubTopic; 
        pubTopic += topic; 
        client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
        Serial.print("Publish topic: ");
        Serial.println(pubTopic);
        Serial.print("Publish message: ");
        Serial.println(payload);
        buttonStateCPrevious = buttonStateC;

    }
     if(checkState(buttonStateD, buttonStateDPrevious)){
        buttonStateD == 1 ? Serial.println("D button is HIGH") : Serial.println("D button is Low") ;
         String payload = "{";
         payload += "\"button\":\"D\"";
        payload += ", ";
         buttonStateD == 1 ? payload += "\"value\":\"HIGH\"" : payload += "\"value\":\"LOW\"" ;
         payload += ", ";
         payload += "\"User\": 1 ";
         payload += "}"; 
         String pubTopic; 
        pubTopic += topic; 
         client.publish( (char*) pubTopic.c_str() , (char*) payload.c_str(), true );
         Serial.print("Publish topic: ");
         Serial.println(pubTopic);
         Serial.print("Publish message: ");
         Serial.println(payload);
     }
   
    delay(100);
}
bool checkState(int state, int previousState) {
    if(state == previousState) {
        return false;
    } else {
        previousState = state;
        return true;
    }
}
