#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Botones
#define BTN_NEXT 2
#define BTN_SELECT 3

char board[9];
int cursor = 0;

// ------------------ FUNCIONES BOTONES ------------------

bool waitForPress(int pin) {
  // Espera a LOW
  while (digitalRead(pin) == HIGH);
  delay(60); // debounce
  // Espera a que suelte
  while (digitalRead(pin) == LOW);
  delay(60);
  return true;
}

// --------------------------------------------------------
void setup() {
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    for(;;);

  resetBoard();
  drawBoard();
}

// --------------------------------------------------------
void loop() {

  // BOTÓN NEXT
  if (digitalRead(BTN_NEXT) == LOW) {
    waitForPress(BTN_NEXT);
    cursor = (cursor + 1) % 9;
    drawBoard();
  }

  // BOTÓN SELECT
  if (digitalRead(BTN_SELECT) == LOW) {
    waitForPress(BTN_SELECT);
    
    if (board[cursor] == ' ') {

      // JUGADOR
      board[cursor] = 'X';
      drawBoard();
      delay(150);

      if (checkWin('X')) {
        showMessage("GANASTE!");
        delay(2000);
        resetBoard();
        drawBoard();
        return;
      }

      if (isFull()) {
        showMessage("EMPATE");
        delay(2000);
        resetBoard();
        drawBoard();
        return;
      }

      // CPU
      cpuMove();
      drawBoard();
      delay(150);

      if (checkWin('O')) {
        showMessage("PERDISTE");
        delay(2000);
        resetBoard();
        drawBoard();
        return;
      }

      if (isFull()) {
        showMessage("EMPATE");
        delay(2000);
        resetBoard();
        drawBoard();
        return;
      }
    }
  }

}

// --------------------------
// LÓGICA DEL JUEGO
// --------------------------

void resetBoard() {
  for (int i = 0; i < 9; i++) board[i] = ' ';
  cursor = 0;
}

bool isFull() {
  for (int i = 0; i < 9; i++)
    if (board[i] == ' ')
      return false;
  return true;
}

bool checkWin(char p) {
  int w[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
  };
  for (int i = 0; i < 8; i++)
    if (board[w[i][0]] == p && board[w[i][1]] == p && board[w[i][2]] == p)
      return true;
  return false;
}

// -------- IA MINIMAX --------

int minimax(char b[], bool isMax) {
  if (checkWinBoard(b,'O')) return 1;
  if (checkWinBoard(b,'X')) return -1;
  if (isFullBoard(b)) return 0;

  int best = isMax ? -1000 : 1000;

  for (int i = 0; i < 9; i++) {
    if (b[i] == ' ') {
      b[i] = isMax ? 'O' : 'X';
      int score = minimax(b, !isMax);
      b[i] = ' ';
      best = isMax ? max(best, score) : min(best, score);
    }
  }
  return best;
}

bool isFullBoard(char b[]) {
  for (int i=0;i<9;i++) if (b[i]==' ') return false;
  return true;
}

bool checkWinBoard(char b[], char p) {
  int w[8][3]={
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
  };
  for(int i=0;i<8;i++)
    if(b[w[i][0]]==p && b[w[i][1]]==p && b[w[i][2]]==p)
      return true;
  return false;
}

void cpuMove() {
  int bestScore = -1000;
  int bestMove = 0;

  for (int i = 0; i < 9; i++) {
    if (board[i] == ' ') {
      board[i] = 'O';
      int score = minimax(board, false);
      board[i] = ' ';
      if (score > bestScore) {
        bestScore = score;
        bestMove = i;
      }
    }
  }
  board[bestMove] = 'O';
}

// ------------------------
// DIBUJO DEL TABLERO
// ------------------------

void drawBoard() {
  display.clearDisplay();
  display.drawLine(42,0,42,64,WHITE);
  display.drawLine(84,0,84,64,WHITE);
  display.drawLine(0,21,128,21,WHITE);
  display.drawLine(0,43,128,43,WHITE);

  display.setTextSize(2);

  for (int i = 0; i < 9; i++) {
    int x = (i % 3) * 42 + 12;
    int y = (i / 3) * 21 + 3;

    if (board[i] != ' ') {
      display.setCursor(x, y);
      display.print(board[i]);
    }
  }

  int cx = (cursor % 3) * 42;
  int cy = (cursor / 3) * 21;
  display.drawRect(cx, cy, 42, 21, WHITE);

  display.display();
}

void showMessage(String msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.print(msg);
  display.display();
}
