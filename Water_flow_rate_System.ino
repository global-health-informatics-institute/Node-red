#include <LiquidCrystal.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TiyaB";
const char* password = "12345678";
const char* mqtt_server = "192.168.43.3";

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
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;
const int input = A0;
void setup()
{
Serial.begin(9600);
lcd.begin(16, 2);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Water Flow Meter");
lcd.setCursor(0,1);
lcd.print("****************");
delay(2000);
pinMode(input,INPUT);
}
void loop()
{
X = pulseIn(input, HIGH);
Y = pulseIn(input, LOW);
TIME = X + Y;
FREQUENCY = 1000000/TIME;
WATER = FREQUENCY/7.5;
LS = WATER/60;
if(FREQUENCY >= 0)
{
if(isinf(FREQUENCY))
{
lcd.clear();
lcd.setCursor(0,0);
lcd.print("VOL. :0.00");
lcd.setCursor(0,1);
lcd.print("TOTAL:");
lcd.print( TOTAL);
lcd.print(" L");
}
else
{
TOTAL = TOTAL + LS;
Serial.println(FREQUENCY);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("VOL.: ");
lcd.print(WATER);
lcd.print(" L/M");
lcd.setCursor(0,1);
lcd.print("TOTAL:");
lcd.print( TOTAL);
lcd.print(" L");
}
}
delay(1000);
}
