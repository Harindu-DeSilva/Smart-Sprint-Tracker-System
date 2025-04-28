#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#define START_SENSOR_PIN 2
#define FINISH_SENSOR_PIN 3
#define MODE_INDICATOR_LED 4
#define FINISH_BUZZER 5

const int MAX_LAPS = 50;
float recordedLapTimes[MAX_LAPS];
float recordedLapSpeeds[MAX_LAPS];

unsigned long raceStartTime = 0;
unsigned long singleLapStartTime = 0;
int currentLapIndex = 0;
float userTrackDistance = 0;
int userTotalLaps = 0;
bool raceInProgress = false;
bool runningReverseMode = false;
bool waitingAtStartLine = true;
bool raceSessionCompleted = false;
int lapDetailDisplayIndex = 0;
bool alternateDirectionMode = false;
bool insideSettingsMenu = false;
bool insideMainMenu = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte rowPins[4] = {6, 7, 8, 9};
byte colPins[4] = {10, 11, 12, 13};

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
String userInput = "";
bool awaitingDistanceInput = true;

void setup() {
  Serial.begin(9600);
  pinMode(FINISH_BUZZER, OUTPUT);
  pinMode(START_SENSOR_PIN, INPUT);
  pinMode(FINISH_SENSOR_PIN, INPUT);
  pinMode(MODE_INDICATOR_LED, OUTPUT);
  lcd.begin();
  lcd.backlight();
  displayMainMenuScreen();
}

void loop() {
  char key = keypad.getKey();

  if (key == 'A') {
    resetRaceSession();
    return;
  }

  if (key) {
    if (insideMainMenu) {
      processMenuInput(key);
      return;
    }
    if (insideSettingsMenu) {
      processSettingsInput(key);
      return;
    }

    if (key == '*') {
      userInput = "";
      lcd.clear();
      lcd.print(awaitingDistanceInput ? "Enter Distance:" : "Enter Laps:");
    } else if (key == '#') {
      if (raceSessionCompleted) {
        displayLapDetails();
      } else if (awaitingDistanceInput) {
        userTrackDistance = userInput.toFloat();
        awaitingDistanceInput = false;
        userInput = "";
        lcd.clear();
        lcd.print("Enter Laps:");
      } else {
        userTotalLaps = userInput.toInt();
        userInput = "";
        lcd.clear();
        lcd.print("Ready to Start!");
      }
    } else {
      userInput += key;
      lcd.clear();
      lcd.print(userInput);
    }
  }

  if (Serial.available()) {
    char btData = Serial.read();
    if (btData == '1') {
      runningReverseMode = true;
      digitalWrite(MODE_INDICATOR_LED, HIGH);
    } else if (btData == '0') {
      runningReverseMode = false;
      digitalWrite(MODE_INDICATOR_LED, LOW);
    }
  }

  if (alternateDirectionMode) {
    runningReverseMode = (currentLapIndex % 2 != 0);
  }

  int startSensorState = digitalRead(START_SENSOR_PIN);
  int finishSensorState = digitalRead(FINISH_SENSOR_PIN);

  if (runningReverseMode) {
    int temp = startSensorState;
    startSensorState = finishSensorState;
    finishSensorState = temp;
  }

  if (startSensorState == LOW && !raceInProgress) {
    if (userTrackDistance <= 0 || userTotalLaps <= 0) {
      lcd.clear();
      lcd.print("Enter Distance & Laps");
      resetRaceSession();
      return;
    }

    raceStartTime = millis();
    singleLapStartTime = millis();
    raceInProgress = true;
    waitingAtStartLine = false;

    lcd.clear();
    lcd.print("Race Started!");
  }

  if (finishSensorState == LOW && raceInProgress && !waitingAtStartLine) {
    unsigned long singleLapEndTime = millis();
    float lapDuration = (singleLapEndTime - singleLapStartTime) / 1000.0;
    float lapSpeed = userTrackDistance / lapDuration;

    recordedLapTimes[currentLapIndex] = lapDuration;
    recordedLapSpeeds[currentLapIndex] = lapSpeed;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lap ");
    lcd.print(currentLapIndex + 1);
    lcd.print(": ");
    lcd.print(lapDuration, 2);
    lcd.print("s");

    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(lapSpeed, 2);
    lcd.print(" m/s");

    currentLapIndex++;
    waitingAtStartLine = true;

    if (currentLapIndex == userTotalLaps) {
      raceInProgress = false;
      raceSessionCompleted = true;
      lapDetailDisplayIndex = 0;

      digitalWrite(FINISH_BUZZER, HIGH);
      delay(1000);
      digitalWrite(FINISH_BUZZER, LOW);

      lcd.clear();
      lcd.print("Race Completed!");
    }
  }

  if (startSensorState == LOW && waitingAtStartLine && raceInProgress) {
    singleLapStartTime = millis();
    waitingAtStartLine = false;
  }
}

void displayLapDetails() {
  if (lapDetailDisplayIndex < currentLapIndex) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lap ");
    lcd.print(lapDetailDisplayIndex + 1);
    lcd.print(": ");
    lcd.print(recordedLapTimes[lapDetailDisplayIndex], 2);
    lcd.print("s");

    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(recordedLapSpeeds[lapDetailDisplayIndex], 2);
    lcd.print("m/s");

    lapDetailDisplayIndex++;
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("No more laps");
    delay(2000);
    displayMainMenuScreen();
  }
}

void resetRaceSession() {
  raceInProgress = false;
  raceSessionCompleted = false;
  userInput = "";
  awaitingDistanceInput = true;
  currentLapIndex = 0;
  userTrackDistance = 0;
  userTotalLaps = 0;
  waitingAtStartLine = true;
  insideSettingsMenu = false;
  insideMainMenu = true;
  displayMainMenuScreen();
}

void displayMainMenuScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Practice 2.Settings");
}

void processMenuInput(char key) {
  if (key == '1') {
    insideMainMenu = false;
    lcd.clear();
    lcd.print("Enter Distance:");
    awaitingDistanceInput = true;
  } else if (key == '2') {
    insideMainMenu = false;
    insideSettingsMenu = true;
    displaySettingsScreen();
  }
}

void displaySettingsScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Switch Run Mode:");
  lcd.setCursor(0, 1);
  lcd.print(alternateDirectionMode ? "Enabled" : "Disabled");
}

void processSettingsInput(char key) {
  if (key == '1') {
    alternateDirectionMode = !alternateDirectionMode;
    displaySettingsScreen();
  } else if (key == 'B') {
    insideSettingsMenu = false;
    insideMainMenu = true;
    displayMainMenuScreen();
  }
}
