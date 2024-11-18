#define triggerPin1 33
#define echoPin1 32

#define triggerPin2 0
#define echoPin2 4

int cm1 = 0;
int cm2 = 0;
int duration1 = 50;
int duration2 = 30;
int count1 = 0;
int count2 = 0;
int status1 = 0;
int status2 = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(triggerPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
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

void ledControl(int& cm, int range, int triggerPin, int echoPin, int& count, int& duration, int& status, int output)
{
  cm = 0.0173 * readDistance(triggerPin, echoPin);
  Serial.print(cm);
  Serial.println("cm");
  if (cm <= range)
  {
    status = 1;
  }
  switch(status)
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

void loop()
{
  ledControl(cm1, 5, triggerPin1, echoPin1, count1, duration1, status1, 5);
  ledControl(cm2, 20, triggerPin2, echoPin2, count2, duration2, status2, 18);
  delay(100);
}
