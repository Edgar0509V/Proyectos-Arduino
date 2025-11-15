// ====== CONFIGURACIÓN DE PINES ======
// LEDs
const int led1 = 8; // LED del SOS
const int led2 = 7; // LED del efecto de brillo (PWM)
const int led3 = 6; // LED del parpadeo

// Botones
const int button1 = 4;
const int button2 = 3;
const int button3 = 5;   // <-- CAMBIADO A PIN 5
const int buttonStop = 2; // Interrupción (IRQ) en pin 2

// ====== VARIABLES ======
int lastB1 = HIGH;
int lastB2 = HIGH;
int lastB3 = HIGH;
volatile bool stopFlag = false; // Bandera de paro (interrupción)

// ====== PROTOTIPOS ======
void sosSignal(int led);
void fadeEffect(int led);
void blinkLed(int led);
void stopISR();

// ====== SETUP ======
void setup() {

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);   // <-- PIN 5
  pinMode(buttonStop, INPUT_PULLUP); // PIN 2 IRQ

  // Interrupción externa en pin 2
  attachInterrupt(digitalPinToInterrupt(buttonStop), stopISR, FALLING);
}

// ====== LOOP PRINCIPAL ======
void loop() {
  int b1 = digitalRead(button1);
  int b2 = digitalRead(button2);
  int b3 = digitalRead(button3);  // <-- AHORA LEE EL PIN 5

  if (b1 == LOW && lastB1 == HIGH) sosSignal(led1);
  if (b2 == LOW && lastB2 == HIGH) blinkLed(led2);
  if (b3 == LOW && lastB3 == HIGH) fadeEffect(led3);  // <-- Sigue llamando a LED 3

  lastB1 = b1;
  lastB2 = b2;
  lastB3 = b3;
}

// ====== FUNCIONES ======
void stopISR() {
  stopFlag = true;
}

// SOS ... --- ...
void sosSignal(int led) {
  stopFlag = false;
  
  for (int i = 0; i < 3 && !stopFlag; i++) {
    digitalWrite(led, HIGH); delay(200);
    digitalWrite(led, LOW); delay(200);
  }
  for (int i = 0; i < 3 && !stopFlag; i++) {
    digitalWrite(led, HIGH); delay(600);
    digitalWrite(led, LOW); delay(200);
  }
  for (int i = 0; i < 3 && !stopFlag; i++) {
    digitalWrite(led, HIGH); delay(200);
    digitalWrite(led, LOW); delay(200);
  }
  digitalWrite(led, LOW);
}

void fadeEffect(int led) {
  stopFlag = false;
  for (int brillo = 0; brillo <= 255 && !stopFlag; brillo++) {
    analogWrite(led, brillo);
    delay(10);
  }
  for (int brillo = 255; brillo >= 0 && !stopFlag; brillo--) {
    analogWrite(led, brillo);
    delay(10);
  }
  analogWrite(led, LOW);
}

void blinkLed(int led) {
  stopFlag = false;
  for (int i = 0; i < 10 && !stopFlag; i++) {
    digitalWrite(led, HIGH); delay(300);
    digitalWrite(led, LOW); delay(300);
  }
  digitalWrite(led, LOW);
}