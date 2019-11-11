// defines pins numbers

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TiyaB";
const char* password = "12345678";
const char* mqtt_server = "192.168.43.63";

long lastMsg = 0;
char msg[20];
const int trigPin = 2;
const int echoPin = 5;

// defines variables
long duration;
int distance;
long hp,h ;

WiFiClient espClient;
PubSubClient client(espClient);



void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe("ultra/readings");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void setup() {

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
 
  
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication
}

void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
h = duration / 58;
h = h - 6;
h = 1572 - h;
hp = 2 * h;

Serial.print(hp);
/*
 * 
// Calculating the distance
distance=( (duration/2)/29.1);
inches= ( (duration /2)/74);
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
Serial.print("Distance: ");
Serial.println("inches");
Serial.println(inches);*/
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }

    /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  client.loop();

      /* publish the message */
      snprintf(msg, 10,"%d", 10);
      client.publish("ultra/readings",msg);
    
delay(1000);
}
