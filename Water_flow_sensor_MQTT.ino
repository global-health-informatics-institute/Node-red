#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

const char* ssid = "TiyaB";
const char* password = "12345678";


// MQTT Cibfiguration
const char* mqtt_server = "192.168.43.63";
const int mqtt_port = 1883;
const char* mqtt_id = "Irrigation_sensor";
const char* mqtt_pub_topic_waterflow = "/home/irrigation_sensor/water";
const char* mqtt_sub_topic_healthcheck = "/home/irrigation_sensor";
const char* mqtt_sub_topic_operation = "/home/irrigation_sensor/operation";

// Other params
const int update_time_sensors = 60000;
const int FlowSensorPin = 4;
const char LED_BUILTIN = 15;

/* Configuración cliente WiFi */
WiFiClient espClient;

/* Configuración MQTT */
PubSubClient clientMqtt(espClient);
char msg[50];
String mqttcommand = String(14);

/* Configuración sensor de flujo */
int FlowSensorState = 0;
float CountFlow = 0;
float CountLitre = 0;
long CountStart = 0;


void setup() {
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, HIGH);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  /* Iiniciar wifi */
  setup_wifi();
  clientMqtt.setServer(mqtt_server, mqtt_port);
  clientMqtt.setCallback(callback);

  /* Iiniciar sensor */
  pinMode(FlowSensorPin, INPUT);
}

void setup_wifi() {
  delay(10);

  // Comienza el proceso de conexión a la red WiFi
  Serial.println();
  Serial.print("[WIFI]Conectando a ");
  Serial.println(ssid);

  // Modo estación
  WiFi.mode(WIFI_STA);
  // Inicio WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("[WIFI]WiFi conectada");
  Serial.print("[WIFI]IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT]Mensaje recibido (");
  Serial.print(topic);
  Serial.print(") ");
  mqttcommand = "";
  for (int i = 0; i < length; i++) {
    mqttcommand += (char)payload[i];
  }
  Serial.print(mqttcommand);
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if (mqttcommand == "command") {
    Serial.println("don");
  } 
}

void reconnect() {
  Serial.print("[MQTT]Intentando conectar a servidor MQTT... ");
  // Bucle hasta conseguir conexión
  while (!clientMqtt.connected()) {
    Serial.print(".");
    // Intento de conexión
    if (clientMqtt.connect(mqtt_id)) { // Ojo, para más de un dispositivo cambiar el nombre para evitar conflicto
      Serial.println("");
      Serial.println("[MQTT]Conectado al servidor MQTT");
      // Once connected, publish an announcement...
      clientMqtt.publish(mqtt_sub_topic_healthcheck, "starting");
      // ... and subscribe
      clientMqtt.subscribe(mqtt_sub_topic_operation);
    } else {
      Serial.print("[MQTT]Error, rc=");
      Serial.print(clientMqtt.state());
      Serial.println("[MQTT]Intentando conexión en 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!clientMqtt.connected()) {
    reconnect();
  }
  clientMqtt.loop();

  FlowSensorState = digitalRead(FlowSensorPin);
  if (FlowSensorState == HIGH) {
    CountFlow += 1;
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)  
  } else {
    digitalWrite(LED_BUILTIN, HIGH) ;    // turn the LED off by making the voltage LOW
  }
  long now_sensors = millis();
  if (now_sensors - CountStart > update_time_sensors) {
    CountStart = now_sensors;
    Serial.println("Resume--------------------------------------");
    if (CountFlow > 1500000 || CountFlow == 0) {
      clientMqtt.publish(mqtt_pub_topic_waterflow, "0");  
    } else {
      //Flow pulse characteristics (6.6*L/Min)
      CountLitre = CountFlow*6.6/2815562;
      snprintf (msg, 10, "%6.2f", CountLitre);
      clientMqtt.publish(mqtt_pub_topic_waterflow, msg);   
      Serial.print(msg);
      Serial.println(" liters");
    }
    Serial.println(CountFlow);
    Serial.println("");
    CountFlow = 0;
  }  
}
