/* dc 모터
 * // <전진>
  analogWrite( motorA1 , 150); 
  analogWrite( motorA2 , 0); 
  analogWrite( motorB1 , 150); 
  analogWrite( motorB2 , 0);
  delay(2000);
  // 후진
  analogWrite( motorA1 , 0);
  analogWrite( motorA2 , speed);
  analogWrite( motorB1 , 0);
  analogWrite( motorB2 , speed);
  delay(2000); 
  // 좌회전 
  analogWrite( motorA1 , speed);
  analogWrite( motorA2 , 0);
  analogWrite( motorB1 , 0);
  analogWrite( motorB2 , speed);
  delay(2000);
 
  // 우회전 
  analogWrite( motorA1 , 0);
  analogWrite( motorA2 , speed);
  analogWrite( motorB1 , speed);
  analogWrite( motorB2 , 0);
  delay(2000);
  // 정지 
  digitalWrite( motorA1 , 0);
  digitalWrite( motorA2 , 0);
  digitalWrite( motorB1 , 0);
  digitalWrite( motorB2 , 0);
  delay(2000);
 */
 /* 무선 통신
  * //    const uint16_t port = 80;
//    const char * host = "192.168.1.1"; // ip or dns
    const uint16_t port = 1337;
    const char * host = "192.168.1.10"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }

    // This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    //client.print("Send this data to the server");
    //uncomment this line to send a basic document request to the server
    client.print("GET /index.html HTTP/1.1\n\n");

  int maxloops = 0;

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  else
  {
    Serial.println("client.available() timed out ");
  }

    Serial.println("Closing connection.");
    client.stop();

    Serial.println("Waiting 5 seconds before restarting...");
    delay(5000);
  */




  /*step motor
   * digitalWrite(dirPin, HIGH);
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
   */
