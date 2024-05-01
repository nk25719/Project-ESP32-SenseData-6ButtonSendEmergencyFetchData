// Part 2 of the Firmware project
// this code was uploaded to the arduino uno only 
// this code reads the value on pin A1 and send it through the serial to the esp32 
// this code turn on five lamps, each lamp is automatically turned on by the emergency level received 
// this code receives the emergency level from the esp32 using a string  
// this code program each of the lamps to blink in a specific interval 




#include <SoftwareSerial.h>

SoftwareSerial Serial2(0, 1); // Pinout of Arduino to ESP32 for communication with ESP32

const int analogPin = A1;
const int lamp1Pin = 2;
const int lamp2Pin = 3;
const int lamp3Pin = 4;
const int lamp4Pin = 5;
const int lamp5Pin = 6;

unsigned long previousMillis = 0;
const long interval = 100; // Adjust the interval based on your needs

void setup() {
  Serial.begin(9600); // Arduino serial
  Serial2.begin(115200);
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  pinMode(lamp3Pin, OUTPUT);
  pinMode(lamp4Pin, OUTPUT);
  pinMode(lamp5Pin, OUTPUT);
}

void blinkLamp(int lampPin, int blinkDelay) {
  digitalWrite(lampPin, HIGH);
  delay(blinkDelay / 2); // On for half of the blink delay
  digitalWrite(lampPin, LOW);
  delay(blinkDelay / 2); // Off for the other half of the blink delay
}

void loop() {
  if (Serial2.available() > 0) {
    String receivedData = Serial2.readStringUntil('\n');
    Serial.println("Received from ESP32: " + receivedData);
  }

  unsigned long currentMillis = millis();

  // Read the analog value from pin A1
  int sensorValue = analogRead(analogPin);

  // SENSOR VALUE ON ESP32 SERIAL 115200
  Serial.print("Analog Value on A1: ");
  Serial.println(sensorValue);

  // SENSOR VALUE ON ARDUINO SERIAL 9600
  Serial2.print("SensorValue");
  Serial2.println(sensorValue);

  delay(900); // Add a delay if needed

  if (Serial.available() > 0) {
    String emergencyLevel = Serial.readStringUntil('\n');
    Serial.println(emergencyLevel);

    if (emergencyLevel.startsWith("Received")) {
      int level = emergencyLevel.substring(26).toInt();

      switch (level) {
        case 1:
          // Blink lamp1 continuously
          if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            blinkLamp(lamp1Pin, 750); // Blink every 750 milliseconds
          }
          break;
        case 2:
          // Blink lamp2 continuously with faster speed
          if (currentMillis - previousMillis >= interval / 2) {
            previousMillis = currentMillis;
            blinkLamp(lamp2Pin, 600); // Blink every 600 milliseconds
          }
          break;
        case 3:
          // Blink lamp3 continuously with even faster speed
          if (currentMillis - previousMillis >= interval / 4) {
            previousMillis = currentMillis;
            blinkLamp(lamp3Pin, 450); // Blink every 450 milliseconds
          }
          break;
        case 4:
          // Blink lamp4 continuously with even faster speed
          if (currentMillis - previousMillis >= interval / 8) {
            previousMillis = currentMillis;
            blinkLamp(lamp4Pin, 300); // Blink every 300 milliseconds
          }
          break;
        case 5:
          // Blink lamp5 continuously with even faster speed
          if (currentMillis - previousMillis >= interval / 16) {
            previousMillis = currentMillis;
            blinkLamp(lamp5Pin, 150); // Blink every 150 milliseconds
          }
          break;
      }
    }
  }
}
