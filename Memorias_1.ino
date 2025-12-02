// Pines para LEDs (D8 a D13 = 6 LEDs)
int leds[] = {8, 9, 10, 11, 12, 13};
int numLeds = 6;

// Pines para Botones (D2 a D7 = 6 botones)
int botones[] = {2, 3, 4, 5, 6, 7};
int numBotones = 6;

int ultimoEstado[6];

int minaPosicion;   // posición de la mina (0–5)
bool gameOver = false;

void setup() {
  // LEDs
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  // Botones con INPUT_PULLUP
  for (int i = 0; i < numBotones; i++) {
    pinMode(botones[i], INPUT_PULLUP);
    ultimoEstado[i] = HIGH;
  }

  iniciarJuego();
}

void iniciarJuego() {
  // Apagar LEDs
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], LOW);
  }

  // Mina aleatoria
  randomSeed(analogRead(0));
  minaPosicion = random(0, numLeds);

  gameOver = false;
}

void loop() {
  for (int i = 0; i < numBotones; i++) {

    int lectura = digitalRead(botones[i]);

    // Detectar flanco HIGH → LOW
    if (lectura == LOW && ultimoEstado[i] == HIGH) {
      delay(50);

      // Si aún no se pierde
      if (!gameOver) {
        if (i == minaPosicion) {
          gameOver = true;
          explosion();
          iniciarJuego();     // 🔥 Se reinicia automático
        } else {
          digitalWrite(leds[i], HIGH);
        }
      }
    }

    ultimoEstado[i] = lectura;
  }
}

void explosion() {
  for (int x = 0; x < 6; x++) {
    for (int i = 0; i < numLeds; i++) digitalWrite(leds[i], HIGH);
    delay(150);
    for (int i = 0; i < numLeds; i++) digitalWrite(leds[i], LOW);
    delay(150);
  }
}
