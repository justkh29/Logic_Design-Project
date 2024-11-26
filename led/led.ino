// Define multiple sensor parameters
int cm1 = 0, cm2 = 0;
int status1 = 0, status2 = 0;
int duration1 = 5, duration2 = 4; 
long lastTime1 = 0, lastTime2 = 0;

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

void setup()
{
  Serial.begin(9600);

  // Set sensor pins and outputs
  pinMode(17, INPUT); // Echo 1
  pinMode(16, OUTPUT); // Trigger 1
  pinMode(5, OUTPUT); // LED 1

  pinMode(19, INPUT); // Echo 2
  pinMode(18, OUTPUT); // Trigger 2
  pinMode(4, OUTPUT); // LED 2
}

void loop()
{
  ledControl(cm1, 20, 16, 17, duration1, status1, 5, lastTime1);
  ledControl(cm2, 30, 18, 19, duration2, status2, 4, lastTime2);
}
