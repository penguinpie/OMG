#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200

void setup()
{
  Serial.begin(115200);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}
void loop()
{
  digitalWrite(dirPin, HIGH);
  Serial.println("Spinning Clockwise...");
  
  for(int i = 0; i<stepsPerRevolution; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  delay(1000); 
  
  digitalWrite(dirPin, LOW);
  Serial.println("Spinning Anti-Clockwise...");

  for(int i = 0; i<stepsPerRevolution; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);

  digitalWrite(dirPin, HIGH);
  Serial.println("Spinning Clockwise...");
  
  for(int i = 0; i<5*stepsPerRevolution; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  delay(1000); 
  
  digitalWrite(dirPin, LOW);
  Serial.println("Spinning Anti-Clockwise...");

  for(int i = 0; i<5*stepsPerRevolution; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);
}
