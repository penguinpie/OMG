#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>



//worm gear motor
int WG_EN = 12;
int WG_Pin1 = 13;
int WG_Pin2 = 14;
const int WGfreq = 3000;
const int WGpwmChannel = 0;
const int WGresolution = 8;
int WGdutyCycle = 200;

//photo sensor
int Dstate = 0;

//setting dc motor PWM properties
int DC_EN1 = 2;
int DC_motor1Pin1 = 0;
int DC_motor1Pin2 = 4;
const int DCfreq = 3000;
const int DCpwmChannel = 0;
const int DCresolution = 8;
int DCdutyCycle = 200;


//MPU setup for acceleration calculation
Adafruit_MPU6050 mpu;
float ax_cal = 0.0;
float ay_cal = 0.0;
float az_cal = 0.0;
float ax_fil = 0.0;
float ay_fil = 0.0;
float az_fil = 0.0;
float total_ac = 0.0;
float alpha = 0.8; // 필터 강도 조절 파라미터 (0.0 ~ 1.0)

void setup(void) {
  
  //set WG motor pins as outputs;
  pinMode(WG_Pin1, OUTPUT);
  pinMode(WG_Pin2, OUTPUT);
  pinMode(WG_EN, OUTPUT);
  ledcSetup(WGpwmChannel, WGfreq, WGresolution);
  ledcAttachPin(WG_EN, WGpwmChannel);

  //set dc motor pins as outputs;
  pinMode(DC_motor1Pin1, OUTPUT);
  pinMode(DC_motor1Pin2, OUTPUT);
  pinMode(DC_EN1, OUTPUT);
  ledcSetup(DCpwmChannel, DCfreq, DCresolution);
  ledcAttachPin(DC_EN1, DCpwmChannel);

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  // MPU6050 setup
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  digitalWrite(WG_Pin1, LOW);
  digitalWrite(WG_Pin2, LOW);
  delay(5000);

  Serial.println("go");
  delay(100);
}

void loop() {

  //MPU6050
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  ax_cal = ax_cal * alpha + (1.0 - alpha) * a.acceleration.x;
  ay_cal = ay_cal * alpha + (1.0 - alpha) * a.acceleration.y;
  az_cal = az_cal * alpha + (1.0 - alpha) * (a.acceleration.z-9.8);
  ax_fil = a.acceleration.x - ax_cal;
  ay_fil = a.acceleration.y - ay_cal;
  az_fil = a.acceleration.z - az_cal;
  total_ac = sqrt(ax_fil * ax_fil + ay_fil * ay_fil + az_fil * az_fil); 
  Serial.println("\ntotal: ");
  Serial.print(total_ac);

  //photodiode sensor
  int IsBall = digitalRead(16);
  int IsBug = digitalRead(17);
  
  /* Print out the values */
//  Serial.print("Acceleration X: ");
//  Serial.print(ax_fil);
//  Serial.print(", Y: ");
//  Serial.print(ay_fil);
//  Serial.print(", Z: ");
//  Serial.print(az_fil);
//  Serial.println(" m/s^2");
//  Serial.print("\nRotation X: ");
//  Serial.print(g.gyro.x);
//  Serial.print(", Y: ");
//  Serial.print(g.gyro.y);
//  Serial.print(", Z: ");
//  Serial.print(g.gyro.z);
//  Serial.println(" rad/s");

  //IDLE: dc motor running
  digitalWrite(DC_motor1Pin1, HIGH);
  digitalWrite(DC_motor1Pin2, LOW);
  delay(100);

  if(total_ac > 10){ //shock detected
    Serial.println("Shock detected!! - Contracting\n");
    while(!IsBall){
      digitalWrite(DC_motor1Pin1, LOW);
      digitalWrite(DC_motor1Pin2, LOW); //dc stop
      digitalWrite(WG_Pin1, LOW);
      digitalWrite(WG_Pin2, HIGH); //contnraction at maximum speed
      delay(100);
      if(IsBall==HIGH){
        Serial.println("ball");
        break;
      }
    }
    
    //stay contracted for 3s
    Serial.println("sensed: stay contracted\n");
    delay(3000);
    
    if(total_ac<2){
      //acc reset
      ax_cal = 0.0;
      ay_cal = 0.0;
      az_cal = 0.0;
      //relaxation(slowly)
      Serial.println("relaxing\n");
      while(IsBug==LOW){
        WGdutyCycle = 100;
        ledcWrite(WGpwmChannel, WGdutyCycle);
        digitalWrite(WG_Pin1, HIGH);
        digitalWrite(WG_Pin2, LOW);
        delay(100);
      }
    Serial.println("Bug, ready to move\n");
    WGdutyCycle = 200;
    delay(1000);
  }

  Serial.println("");
  delay(500);
  }
}
