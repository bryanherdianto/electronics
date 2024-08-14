#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6RlXl0uEg"
#define BLYNK_TEMPLATE_NAME "Smart dan Secure Door System"
#define BLYNK_AUTH_TOKEN "zSopb4rGmBYaoJt3F37Oq5y6UoAwKH34"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Keypad.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Menghubungkan masing-masing pin digital pada masing-masing alat
const int pinLED = 5;
const int pinBuzzer = 12;
const int pinServo = 15;

Servo myservo;

const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t colPins[COLS] = { 23, 22, 21, 19 }; // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 14, 27, 26, 25 }; // Pins connected to R1, R2, R3, R4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const String password = "AB94D"; // change your password here
String input_password;

int pos = 90;

typedef enum {
  AMAN, GAGAL
} Status;

Status status = GAGAL;

void setup()
{
  // Debug console
  Serial.begin(115200);

  // menghubungkan pin digital pada buzzer, LED, dan servo
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLED, OUTPUT);
  myservo.attach(pinServo);

  input_password.reserve(32);

  // inisialisasi Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  // menjalankan Blynk
  Blynk.run();

  // mendapatkan input karakter dari 4x4 Keypad
  char key = keypad.getKey();

  if (key) {
    // mencetak input karakter pada serial monitor
    Serial.println(key);
    if (key == '*') {
      input_password = ""; // clear input password
    }
    else if (key == '#') {
      if (input_password == password) {
        Serial.println("password is correct");
        Blynk.logEvent("SAFE", "WEH SELAMAT DATANG KEMBALI!");

        // Apabila password benar, maka status jadi AMAN dan widget bisa digunakan
        status = AMAN;
      }
      else {
        Serial.println("password is incorrect, try again");
        Blynk.logEvent("SAFE", "ADA MALINGGG!");

        // Apabila password salah, maka status jadi GAGAL dan widget jadi tidak berguna
        status = GAGAL;
      }
      input_password = ""; // clear input password
    }
    else {
      input_password += key; // append new character to input password string
    }
  }
}

BLYNK_WRITE(V0) // Executes when the value of virtual pin V0 changes
{
  if (status == AMAN) {
    myservo.write(param.asInt()); // servo akan bergerak sesuai dengan slider input
  }
}

BLYNK_WRITE(V1) // Executes when the value of virtual pin V1 changes
{
  if (status == AMAN) {
    if (param.asInt() == 1) {
      tone(pinBuzzer, 2000, 500);
      delay(1000);
    }
  }
}

BLYNK_WRITE(V2) // Executes when the value of virtual pin V2 changes
{
  if (status == AMAN) {
    if (param.asInt() == 1) {
      // execute this code if the switch widget is now ON
      digitalWrite(pinLED, HIGH);  // Set digital pinLED HIGH
      Serial.println("On");
    }
    else {
      // execute this code if the switch widget is now OFF
      digitalWrite(pinLED, LOW);  // Set digital pinLED LOW
      Serial.println("Off");
    }
  }
}