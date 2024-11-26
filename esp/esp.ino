#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;
int dhtPin = 16;
int relay_pin = 13;

int cm1 = 0, cm2 = 0;
int status1 = 0, status2 = 0;
int duration1 = 5, duration2 = 4; 
long lastTime1 = 0, lastTime2 = 0;



unsigned long currentTime = millis();

void setup()
{
  Serial.begin(9600);


  pinMode(33, INPUT); // Echo 1
  pinMode(32, OUTPUT); // Trigger 1
  pinMode(5, OUTPUT); // LED 1

  pinMode(4, INPUT); // Echo 2
  pinMode(0, OUTPUT); // Trigger 2
  pinMode(18, OUTPUT); // LED 2

  pinMode(relay_pin, OUTPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop()
{
   ledControl(cm1, 5, 33, 32, duration1, status1, 5, lastTime1);
  ledControl(cm2, 5, 4, 0, duration2, status2, 18, lastTime2);

  // ledControl(cm1, 5, triggerPin1, echoPin1, count1, duration1, status1, 5);
  // ledControl(cm2, 5, triggerPin2, echoPin2, count2, duration2, status2, 18);
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


void ledControl(int &cm, int range, int triggerPin, int echoPin, int duration, int &status, int output, long &lastTime)
{
  unsigned long now = millis();

  cm = 0.0173 * readDistance(triggerPin, echoPin);

  switch (status)
  {
  case 0: // Waiting for object in range
    if (cm <= range)
    {
      digitalWrite(output, HIGH); 
      lastTime = now;             
      status = 1;              
    }
    break;

  case 1: // Object detected
    Serial.print(now - lastTime);
    Serial.print("\n");
    if (now - lastTime >= duration * 1000) 
    {
      status = 2;
    }
    break;

  case 2:
    digitalWrite(output, LOW); 
    status = 0;               
    break;
  }
}