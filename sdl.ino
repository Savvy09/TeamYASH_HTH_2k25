#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char auth[] = "1sG6_9F1qGrMxU9sFNzW6wSGcPgGAMF5";
char ssid[] = "KH";
char pass[] = "19112004";

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
 {'1', '2', '3'},
 {'4', '5', '6'},
 {'7', '8', '9'},
 {'*', '0', '#'}
};
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int pirPin = 35;
const int solenoidPin = 23;
const int servoPin = 18;
const int ledPin = 19;
const int fanPin = 22;

int motionDetected = 0;
bool lockStatus = true;
String enteredPassword = "";
const String correctPassword = "2004";

// ---------- Blynk Virtual Pins ----------
#define BLYNK_LOCK V2
#define BLYNK_LED  V1
#define BLYNK_TEMP V3
#define BLYNK_FAN  V0

#define ONE_WIRE_BUS 4

Servo doorServo;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

float currentTemp = 0;
bool fanStatus = false;
const float TEMP_THRESHOLD = 40.0;

// ------------ WiFi and Blynk ------------
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n WiFi Connection Failed!");
  }
}

void connectBlynk() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Trying to connect to Blynk...");
    Blynk.config(auth, "blynk.cloud", 80);

    if (Blynk.connect(5000)) {
      Serial.println(" Blynk Connected!");
    } else {
      Serial.println(" Blynk Connection Failed!");
    }
  }
}

// ------------ Blynk button control ------------
BLYNK_WRITE(BLYNK_LOCK) {
  if (param.asInt() == 1) {
    unlockDoor();
  } else if (param.asInt() == 0) {
    lockDoor();
  }
}

BLYNK_WRITE(BLYNK_LED) {
  if (param.asInt() == 1) {
    ledOn();
  } else if (param.asInt() == 0) {
    ledOff();
  }
}

BLYNK_WRITE(BLYNK_FAN) {
  if (param.asInt() == 1) {
    fanOn();
  } else {
    fanOff();
  }
}

// ------------ Door Functions ------------
void lockDoor() {
  if (!lockStatus) {
    doorServo.write(0);     
    delay(1000);            

    digitalWrite(solenoidPin, LOW);
    Serial.println(" Door Locked.");
    lockStatus = true;
    Blynk.virtualWrite(BLYNK_LOCK, 0);

    fanOff();
    ledOff();
  }
}

void unlockDoor() {
  if (lockStatus) {
    digitalWrite(solenoidPin, HIGH);
    Serial.println(" Door Unlocked.");
    lockStatus = false;
    Blynk.virtualWrite(BLYNK_LOCK, 1);

    delay(1000);
    doorServo.write(90);    
  }
}

// ------------ LED Functions ------------
void ledOn() {
  digitalWrite(ledPin, HIGH);
  Serial.println(" LED ON.");
  Blynk.virtualWrite(BLYNK_LED, 1);
}

void ledOff() {
  digitalWrite(ledPin, LOW);
  Serial.println(" LED OFF.");
  Blynk.virtualWrite(BLYNK_LED, 0);
}

// ------------ Fan Functions ------------
void fanOn() {
  digitalWrite(fanPin, HIGH);
  fanStatus = true;
  Serial.println(" Fan ON.");
  Blynk.virtualWrite(BLYNK_FAN, 1);
}

void fanOff() {
  digitalWrite(fanPin, LOW);
  fanStatus = false;
  Serial.println(" Fan OFF.");
  Blynk.virtualWrite(BLYNK_FAN, 0);
}

// ------------ Sensors ------------
void checkPIR() {
  int val = digitalRead(pirPin);

  if (val == HIGH) {
    if (motionDetected == LOW) {
      Serial.println("Motion detected!");
      unlockDoor();
      ledOn();
      motionDetected = HIGH;
    }
  } else {
    if (motionDetected == HIGH) {
      Serial.println("Motion ended!");
      lockDoor();
      ledOff();
      fanOff();
      motionDetected = LOW;
    }
  }
}

void checkKeypad() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      if (enteredPassword == correctPassword) {
        Serial.println(" Correct password!");
        if (lockStatus) {
          unlockDoor();
        } else {
          lockDoor();
        }
      } else {
        Serial.println(" Incorrect password!");
      }
      enteredPassword = "";
    } else if (key == '*') {
      enteredPassword = "";
      Serial.println("🗑 Password cleared.");
    } else {
      enteredPassword += key;
    }
  }
}

void checkTemperature() {
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);

  if (currentTemp != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature: ");
    Serial.println(currentTemp);
    Blynk.virtualWrite(BLYNK_TEMP, currentTemp);
    if (motionDetected == HIGH && currentTemp > TEMP_THRESHOLD) {
      fanOn();
    } else {
      fanOff();
    }
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

// ------------ Setup ------------
void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(solenoidPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  digitalWrite(solenoidPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(fanPin, LOW);

  doorServo.attach(servoPin);
  doorServo.write(0);

  sensors.begin();

  connectWiFi();
  connectBlynk();
  Blynk.virtualWrite(BLYNK_LOCK, 0);
  Blynk.virtualWrite(BLYNK_LED, 0);
  Blynk.virtualWrite(BLYNK_FAN, 0);

  timer.setInterval(2000L, checkTemperature);
}

// ------------ Loop ------------
void loop() {
  Blynk.run();
  timer.run();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" WiFi Disconnected! Reconnecting...");
    connectWiFi();
  }
  if (!Blynk.connected()) {
    Serial.println(" Blynk Disconnected! Reconnecting...");
    connectBlynk();
  }

  checkPIR();
  checkKeypad();
}
