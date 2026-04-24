#include <Arduino.h>
#include <BeeLineSensorPro.h>
#include <EEPROM.h>

void saveCalibration();
void loadCalibration();
void performTurn();
void stopMotors();
void smoothStop();
void mdrive(int m1, int m2);

// Motor control pins
int M1Rpwm = 7, M1Lpwm = 8, M1L_EN = 5, M1R_EN = 6;
int M2Rpwm = 11, M2Lpwm = 12, M2L_EN = 9, M2R_EN = 10;

// Control buttons
int pushButton1 = 22;
int pushButton2 = 23;

// Buzzer pin
int buzzerPin = 25;

// State tracking
bool isLineFollowing = false;
bool waitingForFirstButton = true;
bool waitingForSecondButton = true;
bool ignoringBlackLine = false;
bool inIgnorePhase = false;
bool motorsRunning = false;
bool buzzerState = false;

unsigned long ignoreStartTime = 0;
unsigned long lastBuzzerToggle = 0;
const unsigned long buzzerInterval = 300;

int M1_OFFSET = 0;
int M2_OFFSET = 0;

float kp = 0.045;
float kD = 0.05;
int last_value;

unsigned long runStartTime = 0;
unsigned long runDuration = 0;
unsigned long nextRunWaitDuration = 3600000; // 1 hour default

BeeLineSensorPro sensor = BeeLineSensorPro(
  (unsigned char[]){A0, A1, A2, A3, A4, A5, A6, A7},
  LINE_WHITE
);

// EEPROM support
void saveCalibration() {
  int addr = 0;
  for (int i = 0; i < 8; i++) {
    EEPROM.put(addr, sensor.values_min[i]);
    addr += sizeof(int);
    EEPROM.put(addr, sensor.values_max[i]);
    addr += sizeof(int);
  }
}

void loadCalibration() {
  int addr = 0;
  for (int i = 0; i < 8; i++) {
    EEPROM.get(addr, sensor.values_min[i]);
    addr += sizeof(int);
    EEPROM.get(addr, sensor.values_max[i]);
    addr += sizeof(int);
  }
}

void setup() {
  pinMode(M1Lpwm, OUTPUT);
  pinMode(M1Rpwm, OUTPUT);
  pinMode(M1L_EN, OUTPUT);
  pinMode(M1R_EN, OUTPUT);
  pinMode(M2Lpwm, OUTPUT);
  pinMode(M2Rpwm, OUTPUT);
  pinMode(M2L_EN, OUTPUT);
  pinMode(M2R_EN, OUTPUT);
  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  loadCalibration();
}

void loop() {
  unsigned long currentTime = millis();

  if (!isLineFollowing && waitingForFirstButton && digitalRead(pushButton1) == HIGH) {
    delay(200);
    isLineFollowing = true;
    waitingForFirstButton = false;
    motorsRunning = false;
    runStartTime = millis(); // Start of the first run
  }

  if (waitingForSecondButton && digitalRead(pushButton2) == HIGH) {
    delay(200);
    stopMotors();
    performTurn();
    waitingForSecondButton = false;
  }

  if (isLineFollowing) {
    int err = sensor.readSensor();
    int baseSpeed = 220;
    int diff = map(err * kp + (err - last_value) * kD, -400, 400, -240, 240);
    last_value = err;
    int m1Speed = baseSpeed + diff + M1_OFFSET;
    int m2Speed = baseSpeed - diff + M2_OFFSET;
    mdrive(m1Speed, m2Speed);

    // Sub Stores Marker
    if (!ignoringBlackLine && !inIgnorePhase &&
        analogRead(A1) < 800 && analogRead(A2) < 800 &&
        analogRead(A5) < 800 && analogRead(A6) < 800 &&
        analogRead(A0) > 800 && analogRead(A7) > 800 &&
        ((analogRead(A3) > 800 || analogRead(A4) > 800) || (analogRead(A3) > 800 && analogRead(A4) > 800))
) {

      motorsRunning = true;

      mdrive(200, -200); delay(4500);
      mdrive(150, -150);
      while (true) {
        if (analogRead(A2) > 300 || analogRead(A3) > 300) break;
        delay(10);
      }
      stopMotors(); delay(200);
      mdrive(150, 150); delay(400);
      stopMotors(); delay(200);
      motorsRunning = false;
      digitalWrite(buzzerPin, LOW);
      buzzerState = false;

      // Measure run duration and compute wait
      runDuration = millis() - runStartTime;
      nextRunWaitDuration = (runDuration < 3600000) ? (3600000 - runDuration) : 0;

      // Wait for either button or automatic start
      unsigned long waitStartTime = millis();
      while (millis() - waitStartTime < nextRunWaitDuration) {
        if (digitalRead(pushButton1) == HIGH || digitalRead(pushButton2) == HIGH) break;
        delay(100);
      }

      ignoringBlackLine = true;
      inIgnorePhase = true;
      ignoreStartTime = millis();
      isLineFollowing = true;
      waitingForFirstButton = true;
      waitingForSecondButton = true;

      runStartTime = millis(); // Start time of the new run
    }

    // Category A Marker
    if (!ignoringBlackLine && !inIgnorePhase &&
        (analogRead(A0) > 800 || analogRead(A1) > 800) &&
        analogRead(A2) < 800 && analogRead(A3) < 800 &&
        analogRead(A4) < 800 && analogRead(A5) < 800 &&
        (analogRead(A6) > 800 || analogRead(A7) > 800)) {

      motorsRunning = true;

      mdrive(200, -200); delay(4500);
      mdrive(150, -150);
      while (true) {
        if (analogRead(A2) > 300 || analogRead(A3) > 300) break;
        delay(10);
      }
      stopMotors(); delay(200);
      mdrive(150, 150); delay(400);
      stopMotors(); delay(200);
      motorsRunning = false;

      unsigned long waitStartTime = millis();
      const unsigned long waitDuration = 90000;
      while (millis() - waitStartTime < waitDuration) {
        if (digitalRead(pushButton1) == HIGH || digitalRead(pushButton2) == HIGH) break;
        delay(100);
      }

      ignoringBlackLine = true;
      inIgnorePhase = true;
      ignoreStartTime = millis();
      isLineFollowing = true;
      waitingForFirstButton = true;
      waitingForSecondButton = true;
    }

    waitingForSecondButton = true;

    // All black detected
    if (!ignoringBlackLine && !inIgnorePhase &&
        analogRead(A0) > 800 && analogRead(A1) > 800 &&
        analogRead(A2) > 800 && analogRead(A3) > 800 &&
        analogRead(A4) > 800 && analogRead(A5) > 800 &&
        analogRead(A6) > 800 && analogRead(A7) > 800) {

      smoothStop();
      unsigned long waitStartTime = millis();
      const unsigned long waitDuration = 90000;
      while (millis() - waitStartTime < waitDuration) {
        if (digitalRead(pushButton1) == HIGH || digitalRead(pushButton2) == HIGH) break;
        delay(100);
      }

      ignoringBlackLine = true;
      inIgnorePhase = true;
      ignoreStartTime = millis();
    } else if (inIgnorePhase && (currentTime - ignoreStartTime >= 5000)) {
      inIgnorePhase = false;
      ignoringBlackLine = false;
    }
  }

  // Buzzer pulse control
  if (motorsRunning) {
    if (millis() - lastBuzzerToggle > buzzerInterval) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      digitalWrite(buzzerPin, buzzerState);
    }
  } else {
    digitalWrite(buzzerPin, LOW);
    buzzerState = false;
  }
}

void performTurn() {
  motorsRunning = true;

  mdrive(200, -200); delay(4500);
  mdrive(150, -150);
  while (true) {
    if (analogRead(A2) > 300 || analogRead(A3) > 300) break;
    delay(10);
  }
  stopMotors(); delay(200);
  mdrive(150, 150); delay(400);
  stopMotors(); delay(200);

  isLineFollowing = false;
  waitingForFirstButton = true;
  waitingForSecondButton = true;

  motorsRunning = false;
}

void stopMotors() {
  digitalWrite(M1L_EN, HIGH);
  digitalWrite(M1R_EN, HIGH);
  digitalWrite(M2L_EN, HIGH);
  digitalWrite(M2R_EN, HIGH);
  analogWrite(M1Lpwm, 0);
  analogWrite(M1Rpwm, 0);
  analogWrite(M2Lpwm, 0);
  analogWrite(M2Rpwm, 0);
  motorsRunning = false;
}

void smoothStop() {
  int currentSpeed = 190;
  while (currentSpeed > 0) {
    digitalWrite(M1L_EN, HIGH);
    digitalWrite(M1R_EN, HIGH);
    digitalWrite(M2L_EN, HIGH);
    digitalWrite(M2R_EN, HIGH);
    analogWrite(M1Lpwm, 0);
    analogWrite(M1Rpwm, currentSpeed);
    analogWrite(M2Lpwm, 0);
    analogWrite(M2Rpwm, currentSpeed);
    delay(50);
    currentSpeed -= 10;
  }
  stopMotors();
}

void mdrive(int m1, int m2) {
  m1 = constrain(m1, -255, 255);
  m2 = constrain(m2, -255, 255);
  motorsRunning = (m1 != 0 || m2 != 0);

  if (m1 > 0) {
    analogWrite(M1Lpwm, 255 - m1);
    analogWrite(M1Rpwm, m1);
  } else {
    analogWrite(M1Lpwm, 255);
    analogWrite(M1Rpwm, 225 - m1);
  }

  if (m2 > 0) {
    analogWrite(M2Lpwm, 255 - m2);
    analogWrite(M2Rpwm, m2);
  } else {
    analogWrite(M2Lpwm, 255);
    analogWrite(M2Rpwm, 225 - m2);
  }

  digitalWrite(M1L_EN, HIGH);
  digitalWrite(M1R_EN, HIGH);
  digitalWrite(M2L_EN, HIGH);
  digitalWrite(M2R_EN, HIGH);
}
