// ====== PINES ======
const int led1 = 8; // SOS
const int led2 = 7; // Blink
const int led3 = 6; // Fade (PWM)

const int button1 = 4; // SOS
const int button2 = 3; // Blink
const int button3 = 5; // Fade
const int buttonPause = 2; // IRQ (pausa / reanudar)

// ====== VARIABLES ======
int lastB1 = HIGH;
int lastB2 = HIGH;
int lastB3 = HIGH;

unsigned long lastButtonChange1 = 0;
unsigned long lastButtonChange2 = 0;
unsigned long lastButtonChange3 = 0;
const unsigned long buttonDebounce = 50; // ms

volatile bool pauseRequested = false;
bool pauseFlag = false;
unsigned long lastPauseToggle = 0;
const unsigned long pauseDebounce = 200;

int activeEffect = 0;

// SOS
const int MAX_SOS = 40;
bool sosOn[MAX_SOS];
unsigned int sosDur[MAX_SOS];
int sosLen = 0;
int sosIdx = 0;
unsigned long sosLastMillis = 0;

// Blink
int idxBlink = 0;
unsigned long blinkLastMillis = 0;
const unsigned long blinkPeriod = 300;

// Fade
int idxFade = 0;
int fadeDir = 1;
unsigned long fadeLastMillis = 0;
const unsigned long fadeStepPeriod = 10;

void pauseISR();
void buildSOSPattern();
void updateSOS();
void updateBlink();
void updateFade();

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(buttonPause, INPUT_PULLUP);

  buildSOSPattern();

  attachInterrupt(digitalPinToInterrupt(buttonPause), pauseISR, FALLING);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  analogWrite(led3, 0);  // SOLO PWM, NO digitalWrite!
}

void loop() {
  unsigned long now = millis();

  if (pauseRequested) {
    pauseRequested = false;
    if (now - lastPauseToggle > pauseDebounce) {
      pauseFlag = !pauseFlag;
      lastPauseToggle = now;
    }
  }

  // ====== BOTONES ======
  int b1 = digitalRead(button1);
  if (b1 != lastB1 && now - lastButtonChange1 > buttonDebounce) {
    lastButtonChange1 = now;
    if (b1 == LOW) activeEffect = 1;
  }
  lastB1 = b1;

  int b2 = digitalRead(button2);
  if (b2 != lastB2 && now - lastButtonChange2 > buttonDebounce) {
    lastButtonChange2 = now;
    if (b2 == LOW) activeEffect = 2;
  }
  lastB2 = b2;

  int b3 = digitalRead(button3);
  if (b3 != lastB3 && now - lastButtonChange3 > buttonDebounce) {
    lastButtonChange3 = now;
    if (b3 == LOW) activeEffect = 3;
  }
  lastB3 = b3;

  if (pauseFlag) {
    return; // congelar estado
  }

  // ====== EFECTOS ======
  if (activeEffect == 1) updateSOS();
  else if (activeEffect == 2) updateBlink();
  else if (activeEffect == 3) updateFade();
  else {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    analogWrite(led3, 0);
  }
}

void pauseISR() {
  pauseRequested = true;
}

// ====== SOS ======
void buildSOSPattern() {
  sosLen = 0;

  // 3 dots
  for (int i = 0; i < 3; i++) {
    sosOn[sosLen] = true;  sosDur[sosLen++] = 200;
    sosOn[sosLen] = false; sosDur[sosLen++] = 200;
  }
  sosOn[sosLen] = false; sosDur[sosLen++] = 200;

  // 3 dashes
  for (int i = 0; i < 3; i++) {
    sosOn[sosLen] = true;  sosDur[sosLen++] = 600;
    sosOn[sosLen] = false; sosDur[sosLen++] = 200;
  }
  sosOn[sosLen] = false; sosDur[sosLen++] = 200;

  // 3 dots
  for (int i = 0; i < 3; i++) {
    sosOn[sosLen] = true;  sosDur[sosLen++] = 200;
    sosOn[sosLen] = false; sosDur[sosLen++] = 200;
  }

  sosOn[sosLen] = false; sosDur[sosLen++] = 600;

  sosIdx = 0;
  sosLastMillis = millis();
}

void updateSOS() {
  unsigned long now = millis();

  digitalWrite(led2, LOW);
  analogWrite(led3, 0);

  if (now - sosLastMillis >= sosDur[sosIdx]) {
    sosIdx++;
    if (sosIdx >= sosLen) sosIdx = 0;
    sosLastMillis = now;
  }

  digitalWrite(led1, sosOn[sosIdx] ? HIGH : LOW);
}

// ====== BLINK ======
void updateBlink() {
  unsigned long now = millis();

  digitalWrite(led1, LOW);
  analogWrite(led3, 0);

  if (now - blinkLastMillis >= blinkPeriod) {
    blinkLastMillis = now;
    digitalWrite(led2, (idxBlink % 2 == 0) ? HIGH : LOW);
    idxBlink++;
  }
}

// ====== FADE ======
void updateFade() {
  unsigned long now = millis();

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  if (now - fadeLastMillis >= fadeStepPeriod) {
    fadeLastMillis = now;

    analogWrite(led3, idxFade);

    idxFade += fadeDir;
    if (idxFade >= 255) { idxFade = 255; fadeDir = -1; }
    if (idxFade <= 0)   { idxFade = 0;   fadeDir = 1; }
  } else {
    analogWrite(led3, idxFade);
  }
}