/*
 * 로봇 실행 코드(logic)의 input과 output:
1. 로봇실행코드에 입력되는 데이터: 
   ⓐ기울기가속도센서 - float[3] - x축, y축, z축 가속도
   ⓑsteer 정보 - int - forward(5), backward(2), right(3), left(1), stop(0)를 int형으로 선언, 추가로 4개 대각선 방향인 경우도 선언
   ⓒ포토센서 - int
   ⓓ수축/이완 명령 - int - 수축(1), 이완(0), NAN(아무것도 아닌값)
2. 로봇실행코드가 출력하는 데이터: 
   ⓐDC 모터x2 - bool 4개 - in1핀, in2핀으로 모터 한 개의 제어가 가능하고 핀에는 0 또는 1 값을 보내면 모터가 동작한다.
    ⓑ스텝모터x2 - bool 4개, int 2개 - direction핀, step핀에는 0 또는 1값 주면 되고 회전각도는 int로 주면 모터가 동작한다.

  웹소켓 통신 코드: 로봇과 단말기(스마트폰 또는 컴퓨터)와 TCP 통신
  송수신하는 데이터
1. 단말기 --> 로봇
    ⓐsteer 정보
    ⓑ수축/이완 명령
2. 로봇 --> 단말기
    ⓐ포토센서
 */

// accelerometer readings from MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Wifi TCP/IP client 구축
#include <WiFi.h>
#include <WiFiMulti.h>

Adafruit_MPU6050 mpu; //mpu
WiFiMulti WiFiMulti; //wifi
float total_ac = 0.0;
int photoSensor = 9; //photosensor pin
int detect_photo = 0; //photosensor data
int motorA1 =  5; //dc motor x2
int motorA2  = 6; 
int motorB1 =  9; 
int motorB2 =  10; 
int maxspeed = 255; //motor speed: 0 ~ 255
int dirPin = 2; //stepmotor direction 
int stepPin = 3; //steppin
int stepsPerRevolution = 200; //stepsPerrevolution
struct receivedData{
  int steer_data;
  int roll_data;
};
struct receivedData re_data = {0,0}; //received data

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause

  Serial.println("Adafruit MPU6050 test!");

  // initialize MPU
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //가속도 범위: +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  //필터의 대역폭: 21Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  //set photo sensor
  pinMode(photoSensor, INPUT);

  //set dc motor
  pinMode( motorA1 , OUTPUT); 
  pinMode( motorA2 , OUTPUT); 
  pinMode( motorB1 , OUTPUT); 
  pinMode( motorB2 , OUTPUT);

  //set step motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // connecting to a WiFi network
  WiFiMulti.addAP("SSID", "passpasspass"); //wifi id, password

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);

}

void loop() {

  /* Get new sensor event */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out acceleration */
  // mpu를 수평으로 놓았을 때 X축은 정면 방향의 직선이다.
  Serial.print("Acceleration X: "); //x축 가속도: 수평에서 0
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y); //y축 가속도: 수평에서 0
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z); //z축 가속도: 중력가속도 영향으로 수평인 상태에서 큰 값을 나타냄
  Serial.println(" m/s^2");
  Serial.println("");
  delay(500);

  //TCP/IP 통신
  // const uint16_t port = 80;
  // const char * host = "192.168.1.1"; // ip or dns
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
  //Send this data to the server
  client.print(detect_photo); //photo sensor 값
  client.print("\n");

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
    String line = client.readStringUntil('\r'); //steer 정보, 수축이완 명령 data
    Serial.println(line);
    //line에서 데이터를 적절한 자료형으로 빼오기
    re_data.steer_data = line[0] - '0'; //char을 int형으로 바꾸기
    re_data.roll_data = line[1] - '0'; //char을 int형으로 바꾸기
  }
  else
  {
    Serial.println("client.available() timed out ");
  }
  
  Serial.println("Closing connection.");
  client.stop();

  Serial.println("Waiting 5 seconds before restarting...");
  delay(5000);

  //작동 시스템 구현
  //가속도 센서로부터 값을 받고 수축/이완, 그리고 포토 센서로 값을 받을 때까지 진행
  //total acceleration = x,y,z 벡터 연산 - 중력가속도
  total_ac = sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y + a.acceleration.y * a.acceleration.y)-g_accel; 
  Serial.println("\ntotal: ");
  Serial.print(total_ac);
  Serial.print("\n");
  if(total_ac > 10){ //shock detected
    Serial.println("Shock detected!! Roll body now\n");
    digitalWrite(DC_motor1Pin1, LOW);
    digitalWrite(DC_motor1Pin2, LOW); //dc stop
    digitalWrite(dirPin, HIGH); //수축방향
    while(digitalRead(photoSensor)==LOW){//스텝모터 작동
      Serial.println("Rolling body...");
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500); 
    }
    delay(100); //작동 종료
    Serial.println("ball state");
    delay(5000); //5초 기다리기
    digitalWrite(dirPin, LOW); //이완방향
    while(int i = 0; i<stepsPerRevolution; i++){//스텝모터 작동
      Serial.println("unfolding body...");
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500); 
    }
  }
  //수축/이완 명령을 스마트폰으로부터 수신받아 작동
  if(re_data.roll_data == 1){ //수축 명령
    Serial.println("Roll control!! Roll body now\n");
    digitalWrite(DC_motor1Pin1, LOW);
    digitalWrite(DC_motor1Pin2, LOW); //dc stop
    digitalWrite(dirPin, HIGH); //수축방향
    while(digitalRead(photoSensor)==LOW){//스텝모터 작동
      Serial.println("Rolling body...");
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500); 
    }
    delay(100); //작동 종료
    Serial.println("ball state");
  }
  else if(re_data.roll_data == 0){
    digitalWrite(dirPin, LOW); //이완방향
    while(int i = 0; i<stepsPerRevolution; i++){//스텝모터 작동
      Serial.println("unfolding body...");
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500); 
    }
  }
  
  //steer 정보 데이터 받아서 로봇 이동
  switch (re_data.steer_data){   //forward부터 1이고 이어서 시계방향으로 진행, stop은 0
    case 1:     
    // <전진>
    analogWrite( motorA1 , maxspeed); 
    analogWrite( motorA2 , 0); 
    analogWrite( motorB1 , maxspeed); 
    analogWrite( motorB2 , 0);
    delay(200);
    break;
    
    case 2:     
    // <우상향>
    analogWrite( motorA1 , maxspeed); 
    analogWrite( motorA2 , 0); 
    analogWrite( motorB1 , 150); 
    analogWrite( motorB2 , 0);
    delay(200);
    break;

    case 3:
    // 우회전 
    analogWrite( motorA1 , maxspeed);
    analogWrite( motorA2 , 0);
    analogWrite( motorB1 , 0);
    analogWrite( motorB2 , maxspeed);
    delay(200);
    break;

    case 4:
    // 우하향
    analogWrite( motorA1 , 0);
    analogWrite( motorA2 , maxspeed);
    analogWrite( motorB1 , 150);
    analogWrite( motorB2 , 0);
    delay(200);
    break;

    case 5:
    // 후진
    analogWrite( motorA1 , 0);
    analogWrite( motorA2 , maxspeed);
    analogWrite( motorB1 , 0);
    analogWrite( motorB2 , maxspeed);
    delay(200); 
    break;

    case 6:
    // 좌하향
    analogWrite( motorA1 , 0);
    analogWrite( motorA2 , 150);
    analogWrite( motorB1 , 0);
    analogWrite( motorB2 , maxspeed);
    delay(200); 
    break;

    case 7:
    // 좌회전 
    analogWrite( motorA1 , maxspeed);
    analogWrite( motorA2 , 0);
    analogWrite( motorB1 , 0);
    analogWrite( motorB2 , maxspeed);
    delay(200);
    break;

    case 8:     
    // <좌상향>
    analogWrite( motorA1 , 150); 
    analogWrite( motorA2 , 0); 
    analogWrite( motorB1 , maxspeed); 
    analogWrite( motorB2 , 0);
    delay(200);
    break;

    case 0:
    // 정지
    digitalWrite( motorA1 , 0);
    digitalWrite( motorA2 , 0);
    digitalWrite( motorB1 , 0);
    digitalWrite( motorB2 , 0);
    delay(200);
    break;

    default:
    break;
  }
}
