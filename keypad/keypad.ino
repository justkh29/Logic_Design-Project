#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Khoi tao LCD I2C
char password[4]; 
char init_pass[4], new_pass[4]; // Mat khau luu trong EEPROM va mat khau moi
char key_pressed = 0; 
const byte row = 4, col = 4; 

unsigned long previousMillis = 0; 
const long interval = 20; // Chu ky kiem tra cam bien
int buzzer = 13; 
int pinA0 = A0, pinA1 = A1, pinA2 = A2; 
int flameA0; // Gia tri doc tu cam bien lua

char MatrixKey[row][col] = { 
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPin[row] = {2, 3, 4, 5}, colPin[col] = {6, 7, 8, 9}; 
Keypad keypad_key = Keypad(makeKeymap(MatrixKey), rowPin, colPin, row, col); // Doi tuong ban phim

unsigned long offTime = 0; // Thoi gian tat LCD
const unsigned long setTime = 15000; // Thoi gian cho tat LCD
int wrong_attempts = 0; // Dem so lan nhap sai

void setup() {
  Serial.begin(9600); // Ket noi Serial
  lcd.begin(16, 2); // Khoi tao LCD
  lcd.backlight(); // Bat den nen LCD
  pinMode(buzzer, OUTPUT); // Chan buzzer la OUTPUT
  digitalWrite(buzzer, LOW); 
  pinMode(pinA0, INPUT); 
  pinMode(pinA1, INPUT);
  pinMode(pinA2, INPUT);
  displayLCD("  LAI 10 DO AN ", " Nha Thong Minh ", 3000);
  displayLCD("Nhap mat khau:", "", 0); 
  setInitialPassword(); // Cai dat mat khau mac dinh
}

void loop() {
  checkfire(pinA0, buzzer); // Kiem tra lua cam bien pinA0
  key_pressed = keypad_key.getKey(); // Lay phim vua nhan
  if (key_pressed) {
    offTime = millis(); // Cap nhat thoi gian nhan
    handleKeyPress(); // Xu ly phim nhan
  }
  if (millis() - offTime > setTime) lcd.noBacklight(); // Tat LCD neu khong nhan phim
  else lcd.backlight(); // Bat LCD

  if (key_pressed == '#') changePassword(); // Doi mat khau khi nhan '#'

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkfire(pinA0, buzzer); // Kiem tra lua cam bien 
    checkfire(pinA1, buzzer); // Kiem tra lua cam bien 
    checkfire(pinA2, buzzer); // Kiem tra lua cam bien 
  }
}

void checkfire(int pin, int buzzerPin) {
  flameA0 = analogRead(pin); // Doc gia tri cam bien lua
  if (flameA0 < 100) { // Neu phat hien lua
    digitalWrite(buzzer, HIGH); // Bat buzzer
    Serial.write('2'); // Gui tin hieu lua toi ESP
  } else {
    digitalWrite(buzzer, LOW); // Tat buzzer
    Serial.write('1'); 
  }
}

void handleKeyPress() {
  static int i = 0;
  if (key_pressed != '#') {
    password[i++] = key_pressed; // Luu ky tu nhap
    lcd.print('*'); // Hien thi '*'
  }
  if (i == 4) { // Neu nhap du 4 ky tu
    i = 0;
    if (checkPassword()) { // Kiem tra mat khau
      displayLCD("Mat khau dung", "", 2000);
      Serial.write('0'); // Bat relay
      delay(3000);
      Serial.write('1'); // Tat relay
      wrong_attempts = 0; // Reset so lan sai
    } else {
      wrong_attempts++; // Tang so lan sai
      if (wrong_attempts >= 4) { // Sai qua 4 lan
displayLCD("    Trom nha", "    bo a con", 2000); // Bao dong
        digitalWrite(buzzer, HIGH); 
        Serial.write('3'); // Gui tin hieu bao dong toi ESP
      } else {
        displayLCD("Sai", "Thu lai", 2000); // Thong bao sai mat khau
      }
    }
    displayLCD("Nhap mat khau:", "", 0); // Hien thong bao nhap mat khau
  }
}

bool checkPassword() {
  for (int j = 0; j < 4; j++) init_pass[j] = EEPROM.read(j); // Doc mat khau tu EEPROM
  return strncmp(password, init_pass, 4) == 0; // So sanh mat khau
}

void changePassword() {
  if (getPasswordInput("Nhap mat khau cu:", new_pass, true)) { // Nhap mat khau cu
    if (strncmp(new_pass, init_pass, 4) == 0) { // Neu dung mat khau cu
      if (getPasswordInput("Mat khau moi:", init_pass, false)) { // Nhap mat khau moi
        for (int j = 0; j < 4; j++) EEPROM.write(j, init_pass[j]); // Luu vao EEPROM
        displayLCD("Da doi mat khau", "", 1000); // Thong bao thanh cong
      }
    } else {
      displayLCD("Sai mat khau", "Thu lai", 1000); // Thong bao sai mat khau cu
    }
  }
  displayLCD("Nhap mat khau:", "", 0); // Quay lai man hinh nhap mat khau
}

bool getPasswordInput(const char* msg, char* pass, bool showAsterisk) {
  int j = 0;
  lcd.clear();
  lcd.print(msg); // Hien thong bao
  lcd.setCursor(0, 1);
  while (j < 4) { // Nhap du 4 ky tu
    char key = keypad_key.getKey();
    if (key) {
      if (key != '#') {
        pass[j++] = key; 
        lcd.print(showAsterisk ? '*' : key);
      }
    }
  }
  delay(500);
  return true;
}

void setInitialPassword() {
  for (int j = 0; j < 4; j++) {
    EEPROM.write(j, j + 49); // Luu mat khau mac dinh (1, 2, 3, 4)
    init_pass[j] = EEPROM.read(j); // Doc lai mat khau
  }
}

void displayLCD(const char* line1, const char* line2, int delay_ms) {
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  if (delay_ms > 0) delay(delay_ms);
}