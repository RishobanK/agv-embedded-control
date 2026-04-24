#include <Arduino.h>

float readUltrasonic(int trigPin, int echoPin);

const int relayPin = 6;
const int beepPin = 7;

const int trigPins[3] = {8, 10, 12};
const int echoPins[3] = {9, 11, 13};

unsigned long previousMillis = 0;

bool obstacleDetected = false;
unsigned long obstacleClearedTime = 0;
const unsigned long gracePeriod = 500;

unsigned long previousBeepTime = 0;
const unsigned long beepInterval = 1000;
const unsigned long beepHighDuration = 500;
bool beepState = false;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(beepPin, OUTPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  digitalWrite(relayPin, LOW);
  digitalWrite(beepPin, LOW);
}

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034f / 2.0f;
}

void loop() {
  bool validReading = false;
  bool obstacleClose = false;

  for (int i = 0; i < 3; i++) {
    float distance = readUltrasonic(trigPins[i], echoPins[i]);
    if (distance > 0) {
      validReading = true;
      if (distance < 30) {
        obstacleClose = true;
      }
    }
    delay(20);
  }

  unsigned long currentMillis = millis();

  if (validReading && obstacleClose) {
    if (!obstacleDetected) {
      obstacleDetected = true;
      digitalWrite(relayPin, HIGH);
      Serial.println("Obstacle Detected - Stopping AGV");
    }
    obstacleClearedTime = 0;
  }

  if (obstacleDetected && !obstacleClose) {
    if (obstacleClearedTime == 0) {
      obstacleClearedTime = currentMillis;
    } else if (currentMillis - obstacleClearedTime >= gracePeriod) {
      obstacleDetected = false;
      digitalWrite(relayPin, LOW);
      Serial.println("Obstacle Cleared - Resuming AGV");
      obstacleClearedTime = 0;
    }
  }

  if (beepState) {
    if (currentMillis - previousBeepTime >= beepHighDuration) {
      digitalWrite(beepPin, LOW);
      beepState = false;
      previousBeepTime = currentMillis;
    }
  } else {
    if (currentMillis - previousBeepTime >= (beepInterval - beepHighDuration)) {
      digitalWrite(beepPin, HIGH);
      beepState = true;
      previousBeepTime = currentMillis;
    }
  }
}