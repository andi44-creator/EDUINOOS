#include <LiquidCrystal.h>
#include <EEPROM.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int backlightPin = 10;
int backlightState = 1;
int gameSpeed = 1;
int speedDelays[] = {800, 500, 250};


int currentMenu = 0;
int numMenus = 7;
String menuItems[] = {"Calculator", "Settings", "About", "Game", "Search", "Help", "Terminal"};


#define NONE 0
#define SELECT 1
#define LEFT 2
#define UP 3
#define DOWN 4
#define RIGHT 5


byte calculatorIcon[8] = {
  B00100, B01010, B01010, B11111,
  B10001, B10001, B11111, B00000
};
byte settingsIcon[8] = {
  B00100, B10101, B01110, B11111,
  B01110, B10101, B00100, B00000
};
byte aboutIcon[8] = {
  B00100, B01010, B00100, B00000,
  B00100, B00100, B00100, B00000
};
byte gameIcon[8] = {
  B00000, B01110, B11011, B10001,
  B10101, B10001, B01110, B00000
};
byte searchIcon[8] = {
  B01110, B10001, B10001, B10001,
  B01110, B00100, B01000, B10000
};
byte terminalIcon[8] = {
  B11111, B10001, B10101, B10001,
  B10101, B10001, B11111, B00000
};

int readButton() {
  int val = analogRead(0);
  if (val > 1000) return NONE;
  if (val < 50) return RIGHT;
  if (val < 195) return UP;
  if (val < 380) return DOWN;
  if (val < 555) return LEFT;
  if (val < 790) return SELECT;
  return NONE;
}

void loadSettings() {
  backlightState = EEPROM.read(0);
  gameSpeed = EEPROM.read(1);
  analogWrite(backlightPin, backlightState ? 255 : 0);
}

void saveSettings() {
  EEPROM.write(0, backlightState);
  EEPROM.write(1, gameSpeed);
}

void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((byte)currentMenu); // icon
  lcd.print(" ");
  lcd.print(menuItems[currentMenu]);
}

void openApp(int appIndex) {
  lcd.clear();
  switch (appIndex) {
    case 0: calculatorApp(); break;
    case 1: settingsApp(); break;
    case 2:
      lcd.print("EDUINOOS v1.0");
      lcd.setCursor(0, 1);
      lcd.print("By Andi Abazi//Gllogjë");
      delay(2000);
      break;
    case 3: gameApp(); break;
    case 4: searchApp(); break;
    case 5: helpApp(); break;
    case 6: terminalApp(); break;
  }
  showMenu();
}

void calculatorApp() {
  int num1 = 0, num2 = 0;
  char op = '+';
  int state = 0;
  bool inApp = true;

  while (inApp) {
    lcd.clear();
    if (state == 0) {
      lcd.print("1st Num: ");
      lcd.print(num1);
    } else if (state == 1) {
      lcd.print("Op: ");
      lcd.print(op);
    } else if (state == 2) {
      lcd.print("2nd Num: ");
      lcd.print(num2);
    } else if (state == 3) {
      lcd.print("Result:");
      lcd.setCursor(0, 1);
      if (op == '/' && num2 == 0) lcd.print("Error: /0");
      else {
        float result = 0;
        if (op == '+') result = num1 + num2;
        if (op == '-') result = num1 - num2;
        if (op == '*') result = num1 * num2;
        if (op == '/') result = (float)num1 / num2;
        lcd.print(result);
      }
    }

    delay(200);
    int btn = readButton();
    switch (btn) {
      case UP:
        if (state == 0) num1++;
        else if (state == 1) op = (op == '+') ? '-' : (op == '-') ? '*' : (op == '*') ? '/' : '+';
        else if (state == 2) num2++;
        break;
      case DOWN:
        if (state == 0) num1--;
        else if (state == 1) op = (op == '+') ? '/' : (op == '/') ? '*' : (op == '*') ? '-' : '+';
        else if (state == 2) num2--;
        break;
      case LEFT: state = (state - 1 + 4) % 4; break;
      case RIGHT: state = (state + 1) % 4; break;
      case SELECT:
        if (state == 3) {
          delay(3000);
          inApp = false;
        }
        break;
    }
  }
}

void settingsApp() {
  int setting = 0;
  bool inApp = true;

  while (inApp) {
    lcd.clear();
    if (setting == 0) {
      lcd.print("Backlight: ");
      lcd.print(backlightState ? "ON" : "OFF");
    } else if (setting == 1) {
      lcd.print("Game Speed: ");
      lcd.print(gameSpeed == 0 ? "Easy" : gameSpeed == 1 ? "Medium" : "Hard");
    } else {
      lcd.print("Save & Exit");
    }

    delay(200);
    int btn = readButton();
    switch (btn) {
      case UP: setting = (setting - 1 + 3) % 3; break;
      case DOWN: setting = (setting + 1) % 3; break;
      case LEFT:
      case RIGHT:
        if (setting == 0) {
          backlightState = !backlightState;
          analogWrite(backlightPin, backlightState ? 255 : 0);
        } else if (setting == 1) {
          gameSpeed = (gameSpeed + 1) % 3;
        }
        break;
      case SELECT:
        if (setting == 2) {
          saveSettings();
          lcd.clear();
          lcd.print("Settings Saved");
          delay(1000);
          inApp = false;
        }
        break;
    }
  }
}

void gameApp() {
  int player = 7;
  int block = random(0, 16);
  bool playing = true;
  int score = 0;

  while (playing) {
    lcd.clear();
    lcd.setCursor(block, 0);
    lcd.print("#");
    lcd.setCursor(player, 1);
    lcd.print("P");

    delay(speedDelays[gameSpeed]);

    int btn = readButton();
    if (btn == LEFT && player > 0) player--;
    if (btn == RIGHT && player < 15) player++;

    if (block == player) {
      lcd.clear();
      lcd.print("GAME OVER!");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      delay(3000);
      playing = false;
    } else {
      block = random(0, 16);
      score++;
    }
  }
}

void searchApp() {
  char query[17] = "                ";
  int cursor = 0;
  char currentChar = 'A';
  bool inApp = true;

  while (inApp) {
    lcd.clear();
    lcd.print("Search: ");
    lcd.setCursor(0, 1);

    for (int i = 0; i < 16; i++) {
      if (i == cursor) lcd.print(currentChar);
      else lcd.print(query[i]);
    }

    delay(200);
    int btn = readButton();
    switch (btn) {
      case LEFT:
        if (cursor > 0) cursor--;
        currentChar = query[cursor] ? query[cursor] : 'A';
        break;
      case RIGHT:
        if (cursor < 15) cursor++;
        currentChar = query[cursor] ? query[cursor] : 'A';
        break;
      case UP:
        currentChar = (currentChar == 'Z') ? 'A' : currentChar + 1;
        break;
      case DOWN:
        currentChar = (currentChar == 'A') ? 'Z' : currentChar - 1;
        break;
      case SELECT:
        query[cursor] = currentChar;
        lcd.clear();
        lcd.print("Searching...");
        delay(1500);
        lcd.clear();
        lcd.print("Result:");
        lcd.setCursor(0, 1);
        lcd.print("no internet connection check your internet connection");
        delay(3000);
        inApp = false;
        break;
    }
  }
}

void helpApp() {
  int page = 0;
  int totalPages = 3;
  bool inHelp = true;

  while (inHelp) {
    lcd.clear();

    if (page == 0) {
      lcd.setCursor(0, 0);
      lcd.print("EDUINOOS Help");
      lcd.setCursor(0, 1);
      lcd.print("Use arrows + OK");
    } else if (page == 1) {
      lcd.setCursor(0, 0);
      lcd.print("UP/DOWN = Menu");
      lcd.setCursor(0, 1);
      lcd.print("SELECT = Launch");
    } else if (page == 2) {
      lcd.setCursor(0, 0);
      lcd.print("LEFT/RIGHT =");
      lcd.setCursor(0, 1);
      lcd.print("Scroll/Input");
    }

    delay(200);
    int btn = readButton();
    switch (btn) {
      case LEFT:  page = (page - 1 + totalPages) % totalPages; break;
      case RIGHT: page = (page + 1) % totalPages; break;
      case SELECT: inHelp = false; break;
    }
  }
}

void terminalApp() {
  char buffer[17] = "                ";
  int cursor = 0;
  char currentChar = 'A';
  bool inApp = true;

  while (inApp) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("> ");
    for (int i = 0; i < 14; i++) {
      if (i == cursor) lcd.print(currentChar);
      else lcd.print(buffer[i]);
    }

    lcd.setCursor(0, 1);
    lcd.print("SELECT = Run");

    delay(200);
    int btn = readButton();
    switch (btn) {
      case LEFT:
        if (cursor > 0) cursor--;
        currentChar = buffer[cursor] ? buffer[cursor] : 'A';
        break;
      case RIGHT:
        if (cursor < 13) cursor++;
        currentChar = buffer[cursor] ? buffer[cursor] : 'A';
        break;
      case UP:
        currentChar = (currentChar == 126) ? 32 : currentChar + 1;
        break;
      case DOWN:
        currentChar = (currentChar == 32) ? 126 : currentChar - 1;
        break;
      case SELECT:
        buffer[cursor] = currentChar;
        lcd.clear();
        lcd.print("Running...");
        delay(1000);
        lcd.clear();
        lcd.print("Response://unix//");
        lcd.setCursor(0, 1);
        lcd.print("OK");
        delay(2000);
        inApp = false;
        break;
    }
  }
}

void setup() {
  lcd.begin(16, 2);
  pinMode(backlightPin, OUTPUT);
  loadSettings();
  randomSeed(analogRead(0));

  lcd.createChar(0, calculatorIcon);
  lcd.createChar(1, settingsIcon);
  lcd.createChar(2, aboutIcon);
  lcd.createChar(3, gameIcon);
  lcd.createChar(4, searchIcon);
  lcd.createChar(5, terminalIcon); // Icon for terminal

  // Boot logo
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("EDUINOOS OS");
  lcd.setCursor(1, 1);
  lcd.print("Starting...");
  delay(1500);

  // Loading bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Booting OS...");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.print((char)255);
    delay(100);
  }
  delay(500);

  showMenu();
}

void loop() {
  int btn = readButton();
  switch (btn) {
    case UP:
      currentMenu = (currentMenu - 1 + numMenus) % numMenus;
      showMenu();
      delay(300);
      break;
    case DOWN:
      currentMenu = (currentMenu + 1) % numMenus;
      showMenu();
      delay(300);
      break;
    case SELECT:
      openApp(currentMenu);
      delay(300);
      break;
  }
}
