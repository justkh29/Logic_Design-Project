#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;
int dhtPin = 16;
int relay_pin = 13;
unsigned long currentTime = millis();

void setup() {
  Serial.begin(9600);
  pinMode(relay_pin, OUTPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop() {
  if (millis() - currentTime > 1000) {
    NHIETDO(); 
    currentTime = millis();
  }
   if (Serial.available()) {
   handleRelay();
  }
}

void handleRelay() {
  char command = Serial.read(); // Đọc lệnh từ Serial
  if (command == '1') {
    digitalWrite(relay_pin, LOW); // Bật relay
  } else if (command == '0') {
    digitalWrite(relay_pin, HIGH); // Tắt relay
  }
}


void NHIETDO() {
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  if (dht.getStatusString() == "OK") {
    lcd.setCursor(0, 0);
    lcd.print("Nhiet Do: " + dataForm(t, 4, 1) + (char)223 + "C");
    lcd.setCursor(0, 1);
    lcd.print("Do Am: " + dataForm(h, 3, 0) + "%");
  }
}

String dataForm(float value, int leng, int decimal) {
  String str = String(value, decimal);
  while (str.length() < leng) str = " " + str;
  return str;
}
