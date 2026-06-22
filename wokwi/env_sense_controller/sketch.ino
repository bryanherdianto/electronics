#define BLYNK_TEMPLATE_ID "TMPL6vRm_chKe"
#define BLYNK_TEMPLATE_NAME "Env-Sense Controller"
#define BLYNK_AUTH_TOKEN "U-KuM7zkKA6XbtMo3zZ1Awpez_QWsmbu"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define RELAY 23
#define RELAY_VOLT 34
#define DHT_PIN 12
#define DHT_TYPE DHT22
#define PIR_PIN 32
#define POT_PIN 35
#define TRIG_PIN 15
#define ECHO_PIN 2

DHT dht22(DHT_PIN, DHT_TYPE);

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  digitalWrite(RELAY, pinValue);
}

long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  pinMode(RELAY_VOLT, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dht22.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();

  int h = dht22.readHumidity();
  int tC = dht22.readTemperature();
  float v = (analogRead(RELAY_VOLT) / 4095.0) * 3.3;
  int pirState = digitalRead(PIR_PIN);
  int potValue = analogRead(POT_PIN);
  long distance = readUltrasonic();

  if (!isnan(h) && !isnan(tC))
  {
    // send to blynk
    Blynk.virtualWrite(V0, tC);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, v);
    Blynk.virtualWrite(V4, pirState);
    Blynk.virtualWrite(V5, potValue);
    Blynk.virtualWrite(V6, distance);
  }
  else
  {
    Serial.printf("Sensor error!\n");
  }

  delay(1000);
}