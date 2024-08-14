#include <DHT.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>

#define DHT22_PIN 2
#define TRIG_PIN  9
#define ECHO_PIN  8
#define PIR_PIN   3
#define LDR_PIN   A0
#define LED_PIN   A3
#define BUZZER    6

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST     0

int tempo = 180;

// Nokia ringtone
int melody[] = {
  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4,
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4,
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2,
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS3231 rtc;
DHT dht22(DHT22_PIN, DHT22);

long previousMillisSensor = 0;
long previousMillisDisplay = 0;

File myFile;
File root;

bool writeFile = false;
bool headersPrinted = false;

const int PIN_RED   = 7;
const int PIN_GREEN = 5;
const int PIN_BLUE  = 4;

void setup() {
  // setup lcd
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.clear();

  // setup serial monitor
  Serial.begin(9600);

  // initialize the DHT22 sensor
  dht22.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);

  // setup RTC module
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    Serial.flush();
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, atur ulang waktu!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // setup SD card
  if (!SD.begin(10)) {
    Serial.println(F("Card initialization failed!"));
    while (1);
  }

  showMenu();
}

void loop() {
  DateTime now = rtc.now();

  unsigned long currentMillis = millis();

  // Update display date and time
  if (currentMillis - previousMillisDisplay >= 1000) {
    previousMillisDisplay = currentMillis;
    DisplayTerangGelap();
    DisplayDateTime(now);
  }

  // Read sensor data
  if (currentMillis - previousMillisSensor >= 2000) {
    previousMillisSensor = currentMillis;

    if (writeFile) {
      readAndPrintSensorData(now);
    }
  }

  // Check for Serial input
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    handleMenuInput(input);
  }
}

void DisplayTerangGelap() {
  float kecerahan = getIlluminance();

  lcd.setCursor(0, 3);
  if (kecerahan < 1000) {
    lcd.print(F("Malam"));
  } else {
    lcd.print(F("Pagi "));
  }
}

void DisplayDateTime(DateTime now) {
  lcd.setCursor(0, 2);
  lcd.print(F("Hour : "));

  if (now.hour() <= 9) {
    lcd.print(F("0"));
  }
  lcd.print(now.hour(), DEC);
  lcd.print(F(":"));
  if (now.minute() <= 9) {
    lcd.print(F("0"));
  }
  lcd.print(now.minute(), DEC);
  lcd.print(F(":"));
  if (now.second() <= 9) {
    lcd.print(F("0"));
  }
  lcd.print(now.second(), DEC);

  lcd.setCursor(0, 1);
  lcd.print(F("Date : "));
  if (now.day() <= 9) {
    lcd.print(F("0"));
  }
  lcd.print(now.day(), DEC);
  lcd.print(F("/"));
  if (now.month() <= 9) {
    lcd.print(F("0"));
  }
  lcd.print(now.month(), DEC);
  lcd.print(F("/"));
  lcd.print(now.year(), DEC);

  char DOW[][10] = {"Sunday   ", "Monday   ", "Tuesday  ", "Wednesday", "Thursday ", "Friday   ", "Saturday "};
  lcd.setCursor(0, 0);
  lcd.print(F("Day  : "));
  lcd.print(DOW[now.dayOfTheWeek()]);
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print(F("\t"));
    }

    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println(F("/"));
      printDirectory(entry, numTabs + 1);

    } else {
      // files have sizes, directories do not
      Serial.print(F("\t\t"));
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void showMenu() {
  Serial.println(F("Menu:"));
  Serial.println(F("1: Write to SD card"));
  Serial.println(F("2: Read from SD card"));
  Serial.println(F("3: Remove a file"));
  Serial.println(F("4: List all files in SD card"));
  Serial.println(F("Enter choice:"));
}

void handleMenuInput(int input) {

  if (input == 0) {
    clearSerialBuffer();
    writeFile = false;
    headersPrinted = false;
    myFile.close();
    Serial.println(F("Stopped."));

    haltSerialMonitor();
    clearSerialMonitor();
    showMenu();
  }
  else if (input == 1) {
    Serial.print(F("Input file name: "));
    clearSerialBuffer();
    Serial.setTimeout(5000);
    String inputString = Serial.readStringUntil('\n');
    String filename = inputString + ".txt";
    Serial.println(filename);

    myFile = SD.open(filename, FILE_WRITE);
    // if the file opened okay, write to it
    if (myFile) {
      Serial.println(F("(Press 0 to stop writing)"));
      writeFile = true;
    } else {
      // if the file didn't open, print an error
      Serial.println(F("Error opening file."));
    }
  }
  else if (input == 2) {
    Serial.println(F("Files in the card:"));
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println(F(""));

    Serial.print(F("What file to read: "));
    clearSerialBuffer();
    Serial.setTimeout(5000);
    String inputString = Serial.readStringUntil('\n');
    String filename = inputString + ".txt";
    Serial.println(filename);

    // open the file for reading
    myFile = SD.open(filename);
    if (myFile) {
      // read from the file until there's nothing else in it
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      // close the file
      myFile.close();
    } else {
      // if the file didn't open, print an error
      Serial.println(F("Error opening file."));
    }

    haltSerialMonitor();
    clearSerialMonitor();
    showMenu();
  }
  else if (input == 3) {
    Serial.println(F("Files in the card:"));
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println(F(""));

    Serial.print(F("What file to be removed: "));
    clearSerialBuffer();
    Serial.setTimeout(5000);
    String inputString = Serial.readStringUntil('\n');
    String filename = inputString + ".txt";
    Serial.println(filename);

    if (SD.exists(filename)) {
      Serial.println(F("File exists."));
    } else {
      Serial.println(F("File doesn't exist."));
    }

    // delete the file
    Serial.println(F("Removing file..."));
    SD.remove(filename);

    if (SD.exists("File")) {
      Serial.println(F("File exists."));
    } else {
      Serial.println(F("File doesn't exist."));
    }

    haltSerialMonitor();
    clearSerialMonitor();
    showMenu();
  }
  else if (input == 4) {
    clearSerialBuffer();
    Serial.println(F("Files in the card:"));
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println(F(""));

    haltSerialMonitor();
    clearSerialMonitor();
    showMenu();
  }
  else {
    clearSerialBuffer();
    Serial.println(F("Invalid selection. Please choose again."));
    showMenu();
  }
}

float getIlluminance(){
  int ldrValue = analogRead(LDR_PIN);
  const float GAMMA = 0.7;
  const float RL10 = 50;
  float voltase = ldrValue / 1024.0 * 5.0;
  float resistansi = 2000 * voltase / (1 - voltase / 5.0);
  return pow(RL10 * 1e3 * pow(10, GAMMA) / resistansi, (1 / GAMMA));
}

void readAndPrintSensorData(DateTime now) {
  float duration_us, distance_cm;
  int redValue, greenValue, blueValue;

  // DHT22
  float humi = dht22.readHumidity();
  float tempC = dht22.readTemperature();

  if (tempC >= 40.0) {
    // warna jadi merah jika di atas 40 derajat
    redValue = 255;
    greenValue = 0;
    blueValue = 0;
  } else if (tempC <= 20.0) {
    // warna jadi hijau jika di bawah 20 derajat
    redValue = 0;
    greenValue = 255;
    blueValue = 0;
  } else {
    // lakukan interpolasi pada suhu di antara 20 dan 40
    float tempRange = 40.0 - 20.0;
    float tempFactor = (tempC - 20.0) / tempRange;

    // hitung nilai RGB berdasarkan interpolasi
    redValue = int(255 * tempFactor);
    greenValue = int(255 * (1.0 - tempFactor));
    blueValue = 0;

    redValue = constrain(redValue, 0, 255);
    greenValue = constrain(greenValue, 0, 255);
  }

  analogWrite(PIN_RED, redValue);
  analogWrite(PIN_GREEN, greenValue);
  analogWrite(PIN_BLUE, blueValue);

  // Ultrasonic sensor
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;

  if (distance_cm < 100.0) {
    playSong();
  }

  // PIR sensor
  int pirValue = digitalRead(PIR_PIN);

  if (pirValue == LOW) {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }

  // LDR sensor
  float kecerahan = getIlluminance();

  if (!headersPrinted) {
    Serial.print(F("Humidity,"));
    Serial.print(F("Temperature,"));
    Serial.print(F("Distance,"));
    Serial.print(F("Motion,"));
    Serial.println(F("Illuminance"));

    myFile.print(F("Date,"));
    myFile.print(F("Time,"));
    myFile.print(F("Humidity,"));
    myFile.print(F("Temperature,"));
    myFile.print(F("Distance,"));
    myFile.print(F("Motion,"));
    myFile.println(F("Illuminance"));
  }
  headersPrinted = true;

  Serial.print(humi);
  Serial.print(F(","));
  Serial.print(tempC);
  Serial.print(F(","));
  Serial.print(distance_cm);
  Serial.print(F(","));
  Serial.print(pirValue);
  Serial.print(F(","));
  Serial.println(kecerahan);

  myFile.print(now.year(), DEC);
  myFile.print(F("/"));
  myFile.print(now.month(), DEC);
  myFile.print(F("/"));
  myFile.print(now.day(), DEC);
  myFile.print(F(","));
  myFile.print(now.hour(), DEC);
  myFile.print(F(":"));
  myFile.print(now.minute(), DEC);
  myFile.print(F(":"));
  myFile.print(now.second(), DEC);
  myFile.print(F(","));
  myFile.print(humi);
  myFile.print(F(","));
  myFile.print(tempC);
  myFile.print(F(","));
  myFile.print(distance_cm);
  myFile.print(F(","));
  myFile.print(pirValue);
  myFile.print(F(","));
  myFile.println(kecerahan);
  
}

void clearSerialBuffer() {
  while (Serial.available()) {
    Serial.read();
  }
}

void clearSerialMonitor() {
  for (int i = 0; i < 50; i++) {
    Serial.println();
  }
}

void haltSerialMonitor() {
  Serial.println("Press any key to continue...");
  while (Serial.read() == -1) {
    // tunggu input
  }
}

void playSong() {
  for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }

    tone(BUZZER, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(BUZZER);
  }
}