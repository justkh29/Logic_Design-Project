#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dht;
int dhtPin = 32;
int relay_pin = 13;

int cm1 = 0, cm2 = 0, cm3 = 0;
int status1 = 0, status2 = 0, status3 = 0;
int duration1 = 2, duration2 = 2, duration3 = 2;
long lastTime1 = 0, lastTime2 = 0, lastTime3 = 0;



unsigned long currentTime = millis();

void setup()
{
  Serial.begin(9600);


  pinMode(17, INPUT); // Echo 1
  pinMode(16, OUTPUT); // Trigger 1
  pinMode(5, OUTPUT); // LED 1

  pinMode(19, INPUT); // Echo 2
  pinMode(18, OUTPUT); // Trigger 2
  pinMode(4, OUTPUT); // LED 2

  pinMode(26, INPUT); // Echo 3
  pinMode(25, OUTPUT); // Trigger 3
  pinMode(33, OUTPUT); // LED 3

  pinMode(relay_pin, OUTPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop()
{
  if (millis() - currentTime > 1000)
  {
    NHIETDO();
    currentTime = millis();
  }
  if (Serial.available())
  {
    handleRelay();
  }
  static unsigned long lastReadTime = 0; // Timer for reading sensors
  unsigned long now = millis();

  // Read sensors only every 100 milliseconds
  if (now - lastReadTime >= 100) { 
      lastReadTime = now;

      // Update sensor distances
      cm1 = 0.0173 * readDistance(16, 17);
      cm2 = 0.0173 * readDistance(18, 19);
      cm3 = 0.0173 * readDistance(25, 26);


  }
      // Print results
      Serial.print("Sensor 1 Distance: ");
      Serial.println(cm1);
  // Update LED statuses based on latest readings
  ledControl(cm1, 20, duration1, status1, 5, lastTime1);
  ledControl(cm2, 20, duration2, status2, 4, lastTime2);
  ledControl(cm3, 20, duration3, status3, 33, lastTime3);

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


void ledControl(int cm, int range, int duration, int &status, int output, long &lastTime) 
{
  unsigned long now = millis();

  switch (status) {
    case 0: // Waiting for object in range
        if (cm <= range) {
            digitalWrite(output, HIGH); 
            lastTime = now;             
            status = 1;              
        }
        break;

    case 1: // Object detected
        if (now - lastTime >= duration * 1000) {
            status = 2;
        }
        break;

    case 2:
        digitalWrite(output, LOW); 
        status = 0;               
        break;
  }
}