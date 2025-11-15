// ====== PINES ======
const int led1 = 8;   // LED para SOS
const int button1 = 4; // Botón para activar SOS

// ====== VARIABLES ======
int lastB1 = HIGH;
unsigned long lastButtonChange1 = 0;
const unsigned long buttonDebounce = 50;

int activeEffect = 0; // 0 = apagado, 1 = SOS

// SOS
const int MAX_SOS = 40;
bool sosOn[MAX_SOS];
unsigned int sosDur[MAX_SOS];
int sosLen = 0;
int sosIdx = 0;
unsigned long sosLastMillis = 0;

void buildSOSPattern();
void updateSOS();

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(button1, INPUT_PULLUP);

  buildSOSPattern();

  digitalWrite(led1, LOW);
}

void loop() {
  unsigned long now = millis();

  // === BOTÓN PARA ACTIVAR SOS ===
  int b1 = digitalRead(button1);
  if (b1 != lastB1 && now - lastButtonChange1 > buttonDebounce) {
    lastButtonChange1 = now;
    if (b1 == LOW) activeEffect = 1;  // activar SOS
  }
  lastB1 = b1;

  // === EJECUTAR SOS ===
  if (activeEffect == 1) updateSOS();
  else digitalWrite(led1, LOW);
}

// ====== Construye el patrón SOS ======
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

// ====== Actualizar SOS ======
void updateSOS() {
  unsigned long now = millis();

  if (now - sosLastMillis >= sosDur[sosIdx]) {
    sosIdx++;
    if (sosIdx >= sosLen) sosIdx = 0;
    sosLastMillis = now;
  }

  digitalWrite(led1, sosOn[sosIdx] ? HIGH : LOW);
}