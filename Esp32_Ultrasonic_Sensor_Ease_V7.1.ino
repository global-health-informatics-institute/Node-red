
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "jessieka";
const char* password = "12345678";
const char* mqtt_server = "192.168.43.79";


// defines pins
const int trigPin = 5;
const int echoPin = 2;

// defines variables
long lastMsg = 0;
char msg[20];
float duration, distance;

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

  Serial.println(WiFi.localIP());
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

float readings() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming
    subscribed topic-process-invoke receivedCallback */
  client.loop();

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Convert the time into a distance
  distance = (duration / 2) / 29.1;   // Divide by 29.1 or multiply by 0.0343
  //  Serial.print(distance);
  //  Serial.println("cm");
  return distance;
}

void loop() {
  Serial.println(WiFi.localIP());
  int has_10_readings = 0;
  float sum_of_valid_reading = 0;
  float average;

  while (true) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      /* configure the MQTT server with IPaddress and port */
    }
    float current_reading = readings();
    if (current_reading > 12 & current_reading < 176) {
      has_10_readings++;
      if (has_10_readings > 10) break;
      if (has_10_readings > 1)
        sum_of_valid_reading += current_reading;
      Serial.println(has_10_readings);
      Serial.println(current_reading);
      Serial.println();
    }
    delay(500);
  }


  snprintf(msg, 10, "%6.2f", (sum_of_valid_reading / 10));
  client.publish("ultra/readings", msg);
  average = sum_of_valid_reading / 10;
  Serial.print("Average: ");
  Serial.println(average);
  delay(50000);

}
