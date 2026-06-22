#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>

// WiFi dan MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

// Pins
#define LED_PIN 13
#define BUZZER_PIN 25
#define SERVO_PIN 26

bool ledState = false;
int servoAngle = 0;
String lcdMessage = "Ready";

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == "home/led") {
    ledState = (msg == "1");
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    Serial.println("LED: " + String(ledState));
  }
  else if (String(topic) == "home/servo") {
    servoAngle = msg.toInt();
    servo.write(servoAngle);
    Serial.println("Servo: " + String(servoAngle) + "°");
  }
  else if (String(topic) == "home/buzzer") {
    if (msg == "1") {
      tone(BUZZER_PIN, 1000, 2000); // bunyi selama 2 detik
    }
  }
  else if (String(topic) == "home/lcd") {
    lcdMessage = msg;
    lcd.clear();
    lcd.print(lcdMessage.substring(0, 16));
    if (lcdMessage.length() > 16) {
      lcd.setCursor(0, 1);
      lcd.print(lcdMessage.substring(16, 32));
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32-SmartHome2.0")) {
      client.subscribe("home/led");
      client.subscribe("home/servo");
      client.subscribe("home/buzzer");
      client.subscribe("home/lcd");
    } else delay(2000);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  servo.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();
  lcd.print("Smart Home 2.0");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Publish current state
  client.publish("home/led/state", ledState ? "1" : "0");
  client.publish("home/servo/state", String(servoAngle).c_str());
  client.publish("home/lcd/state", lcdMessage.c_str());
}