#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;
int dhtPin = 16;
int relay_pin = 13;
int triggerPin1 = 33 int echoPin1 = 32 int triggerPin2 = 0 int echoPin2 = 4

    int cm1 = 0;
int cm2 = 0;
int duration1 = 50;
int duration2 = 30;
int count1 = 0;
int count2 = 0;
int status1 = 0;
int status2 = 0;

unsigned long currentTime = millis();

void setup()
{
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(triggerPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(relay_pin, OUTPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop()
{
  ledControl(cm1, 5, triggerPin1, echoPin1, count1, duration1, status1, 5);
  ledControl(cm2, 20, triggerPin2, echoPin2, count2, duration2, status2, 18);
  if (millis() - currentTime > 1000)
  {
    NHIETDO();
    currentTime = millis();
  }
  if (Serial.available())
  {
    handleRelay();
  }
}

void handleRelay()
{
  char command = Serial.read(); // Đọc lệnh từ Serial
  if (command == '1')
  {
    digitalWrite(relay_pin, LOW); // Bật relay
  }
  else if (command == '0')
  {
    digitalWrite(relay_pin, HIGH); // Tắt relay
  }
}

void NHIETDO()
{
  float h = dht.getHumidity();
  float t = dht.getTemperature();
  if (dht.getStatusString() == "OK")
  {
    lcd.setCursor(0, 0);
    lcd.print("Nhiet Do: " + dataForm(t, 4, 1) + (char)223 + "C");
    lcd.setCursor(0, 1);
    lcd.print("Do Am: " + dataForm(h, 3, 0) + "%");
  }
}

String dataForm(float value, int leng, int decimal)
{
  String str = String(value, decimal);
  while (str.length() < leng)
    str = " " + str;
  return str;
}

long readDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);
}

void ledControl(int &cm, int range, int triggerPin, int echoPin, int &count, int &duration, int &status, int output)
{
  cm = 0.0173 * readDistance(triggerPin, echoPin);
  Serial.print(cm);
  Serial.println("cm");
  if (cm <= range)
  {
    status = 1;
  }
  switch (status)
  {
  case 0:
    break;
  case 1:
    digitalWrite(output, HIGH);
    if (duration > 0)
    {
      count++;
      if (count >= duration)
      {
        status = 2;
        count = 0;
        Serial.print("Done. Change status to ");
        Serial.println(status);
      }
    }
    break;
  case 2:
    digitalWrite(output, LOW);
    break;
  }
}