#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char password[4];
char init_pass[4], new_pass[4];
char key_pressed = 0;
const byte row = 4, col = 4;

unsigned long previousMillis = 0;
const long interval = 20;
int buzzer = 13;
int pinA0 = A0; 
int pinA1 = A1;  
int pinA2 = A2;
int flameA0; 

char MatrixKey[row][col] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPin[row] = {2, 3, 4, 5}, colPin[col] = {6, 7, 8, 9};
Keypad keypad_key = Keypad(makeKeymap(MatrixKey), rowPin, colPin, row, col);

unsigned long offTime = 0; 
const unsigned long setTime = 15000;
int wrong_attempts = 0;

void setup() {
  Serial.begin(9600); // Kết nối UART
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  pinMode(pinA0, INPUT);
  pinMode(pinA1, INPUT);
  pinMode(pinA2, INPUT);
  displayLCD("  LAI 10 DO AN ", " Nha Thong Minh ", 3000);
  displayLCD("Nhap mat khau:", "", 0);
  setInitialPassword();
}

void loop() {
   checkfire(pinA0, buzzer);
  key_pressed = keypad_key.getKey();
  if (key_pressed) {
    offTime = millis(); 
    handleKeyPress();
  }
  if (millis() - offTime > setTime) { 
    lcd.noBacklight();
  } else {
    lcd.backlight(); 
  }
  if (key_pressed == '#') changePassword();
    unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    checkfire(pinA0, buzzer); 
    checkfire(pinA1, buzzer);  
    checkfire(pinA2, buzzer);
  }
}

void checkfire(int pin, int buzzerPin) {
  flameA0 = analogRead(pin);
  
  if (flameA0 < 100) {
    digitalWrite(buzzer, HIGH); 
  } else {
    digitalWrite(buzzer, LOW);  
  }

  // Serial.print("Flame Sensor Value: ");
  // Serial.println(flameA0);
}

void handleKeyPress() {
  static int i = 0;
  if (key_pressed != '#') {
    password[i++] = key_pressed;
    lcd.print('*');  
  }
  if (i == 4) { 
    i = 0;
    if (checkPassword()) {
      displayLCD("Mat khau dung", "", 2000);
      Serial.write('0'); // Gửi tín hiệu bật relay tới ESP32
      delay(3000);
      Serial.write('1'); // Gửi tín hiệu tắt relay tới ESP32
      wrong_attempts = 0; 
    } else {
      wrong_attempts++;
      if (wrong_attempts >= 3) {
        displayLCD("    Trom nha", "    bo a con", 2000);
        digitalWrite(buzzer,HIGH);
        Serial.write('1'); // Gửi tín hiệu tắt relay tới ESP32
      } else {
        displayLCD("Sai", "Thu lai", 2000);
      }
    }
    displayLCD("Nhap mat khau:", "", 0);
  }
}

bool checkPassword() {
  for (int j = 0; j < 4; j++) init_pass[j] = EEPROM.read(j);
  return strncmp(password, init_pass, 4) == 0;
}

void changePassword() {
  if (getPasswordInput("Nhap mat khau cu:", new_pass, true)) { 
    if (strncmp(new_pass, init_pass, 4) == 0) {
      if (getPasswordInput("Mat khau moi:", init_pass, false)) { 
        for (int j = 0; j < 4; j++) EEPROM.write(j, init_pass[j]);
        displayLCD("Da doi mat khau", "", 1000);
      }
    } else {
      displayLCD("Sai mat khau", "Thu lai", 1000);
    }
  }
  displayLCD("Nhap mat khau:", "", 0);
}

bool getPasswordInput(const char* msg, char* pass, bool showAsterisk) {
  int j = 0;
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0, 1);
  while (j < 4) {
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
    EEPROM.write(j, j + 49);
    init_pass[j] = EEPROM.read(j);
  }
}

void displayLCD(const char* line1, const char* line2, int delay_ms) {
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  if (delay_ms > 0) delay(delay_ms);
}
