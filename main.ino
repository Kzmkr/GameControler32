#include <Arduino.h>
#include <BleCombo.h>

// -----------------------------------------------------
// OBJECTS
// -----------------------------------------------------
BleCombo bleCombo("ESP32 Combo");

// -----------------------------------------------------
// GPIO MAP
// -----------------------------------------------------
#define JOY_A_X        0
#define JOY_A_Y        1
#define JOY_B_X        2
#define JOY_B_Y        3

#define JOY_A_BTN      4
#define JOY_B_BTN      5

#define BTN_1          7
#define BTN_2          8
#define BTN_3          9
#define BTN_4          20

#define ANALOG_KEYS    10

// -----------------------------------------------------
// CONFIG
// -----------------------------------------------------
#define CENTER_X 2048
#define CENTER_Y 2048
#define DEADZONE 120
#define NUM_SAMPLES 8
float mouseSpeed = 10.0;

// -----------------------------------------------------
// ADC KEY THRESHOLDS
// -----------------------------------------------------
#define ADC_K1 300
#define ADC_K2 800
#define ADC_K3 1400
#define ADC_K4 2000
#define ADC_K5 2600
#define ADC_TOL 150

// -----------------------------------------------------
// HELPERS
// -----------------------------------------------------
int readAverage(int pin) {
  long t = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) t += analogRead(pin);
  return t / NUM_SAMPLES;
}

float normalize(int v, int c) {
  int d = v - c;
  if (abs(d) < DEADZONE) return 0;
  return constrain(d / 1800.0, -1.0, 1.0);
}

// -----------------------------------------------------
// SETUP
// -----------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(JOY_A_BTN, INPUT_PULLUP);
  pinMode(JOY_B_BTN, INPUT_PULLUP);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);

  bleCombo.begin();
  Serial.println("BLE Combo Ready");
}

// -----------------------------------------------------
// LOOP
// -----------------------------------------------------
void loop() {
  if (!bleCombo.isConnected()) return;

  // ---------------- MOUSE ----------------
  int ax = readAverage(JOY_A_X);
  int ay = readAverage(JOY_A_Y);
  bleCombo.move(
    normalize(ax, CENTER_X) * mouseSpeed,
    normalize(ay, CENTER_Y) * mouseSpeed
  );

  // ---------------- WASD ----------------
  int bx = readAverage(JOY_B_X);
  int by = readAverage(JOY_B_Y);

  bleCombo.release(KEY_W);
  bleCombo.release(KEY_A);
  bleCombo.release(KEY_S);
  bleCombo.release(KEY_D);

  if (by < CENTER_Y - DEADZONE) bleCombo.press(KEY_W);
  if (by > CENTER_Y + DEADZONE) bleCombo.press(KEY_S);
  if (bx < CENTER_X - DEADZONE) bleCombo.press(KEY_A);
  if (bx > CENTER_X + DEADZONE) bleCombo.press(KEY_D);

  // ---------------- DIGITAL BUTTONS ----------------
  if (digitalRead(BTN_1) == LOW) bleCombo.press(KEY_RETURN);
  else bleCombo.release(KEY_RETURN);

  if (digitalRead(BTN_2) == LOW) bleCombo.press('e');
  else bleCombo.release('e');

  if (digitalRead(BTN_3) == LOW) bleCombo.press(KEY_SPACE);
  else bleCombo.release(KEY_SPACE);

  if (digitalRead(BTN_4) == LOW) bleCombo.press(KEY_ESC);
  else bleCombo.release(KEY_ESC);

  // ---------------- ADC KEYS (5 KEYS) ----------------
  int adc = analogRead(ANALOG_KEYS);

  bleCombo.release(KEY_TAB);
  bleCombo.release('q');
  bleCombo.release('r');
  bleCombo.release('f');
  bleCombo.release(KEY_LEFT_SHIFT);

  if (abs(adc - ADC_K1) < ADC_TOL) bleCombo.press(KEY_TAB);
  else if (abs(adc - ADC_K2) < ADC_TOL) bleCombo.press('q');
  else if (abs(adc - ADC_K3) < ADC_TOL) bleCombo.press('r');
  else if (abs(adc - ADC_K4) < ADC_TOL) bleCombo.press('f');
  else if (abs(adc - ADC_K5) < ADC_TOL) bleCombo.press(KEY_LEFT_SHIFT);

  delay(10);
}
