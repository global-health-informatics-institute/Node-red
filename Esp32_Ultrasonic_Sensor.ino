// defines pins numbers

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TiyaB";
const char* password = "12345678";
const char* mqtt_server = "192.168.43.63";

long lastMsg = 0;
char msg[20];
const int trigPin = 5;
const int echoPin = 2;

// defines variables
float initialdistance = 100; // insert initial distance or distance between sensor and empty base tank
float surfacearea = 216.7948; // enter value of surface area of the rectangular tank base by multiplying width and length

float duration, distance, heightofwater, volume, lvolume;

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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  }

  Serial.println("");
  Serial.println("WiFi connected");
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
  distance = (duration*.0343)/2;
  heightofwater = initialdistance-distance;
  volume = heightofwater*surfacearea;
  lvolume = volume/1000;
Serial.println(lvolume);

  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }

    /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  client.loop();

      /* publish the message */
      snprintf(msg, 10,"%6.2f", lvolume);
      client.publish("ultra/readings",msg);
    
delay(60000);
}
