#include <Adafruit_MPU6050.h> //adafruit library import. (for MPU)
#include <Adafruit_Sensor.h>
#include <Wire.h> //wire.h library import. (for i2c communication)

//photo sensor
int Dstate = 0; //Dstate initialization

//setting dc motor PWM properties
int DC_EN1 = 2; //enable pin 
int DC_motor1Pin1 = 0;  //motorpin1
int DC_motor1Pin2 = 4;  //motorpin2
const int DCfreq = 3000;  //frequency: 3000
const int DCpwmChannel = 0; //DCpwnChannel: 0
const int DCresolution = 8; //DCresolution: 8
int DCdutyCycle = 200; //DCdutyCycle : 200


//MPU setup for acceleration calculation
Adafruit_MPU6050 mpu; //MPU setup
float ax_cal = 0.0; //ax_cal = 0
float ay_cal = 0.0; //ay_cal = 0
float az_cal = 0.0; //az_cal = 0
float ax_fil = 0.0; //filter
float ay_fil = 0.0; //filter
float az_fil = 0.0; //filter
float total_ac = 0.0; // total_acceleration
float alpha = 0.8; // 필터 강도 조절 파라미터 (0.0 ~ 1.0)

void setup(void) {

  //set dc motor pins as outputs;
  pinMode(DC_motor1Pin1, OUTPUT); // DC motor pin1
  pinMode(DC_motor1Pin2, OUTPUT); // DC motor pin2
  pinMode(DC_EN1, OUTPUT); //DC_EN1
  ledcSetup(DCpwmChannel, DCfreq, DCresolution); // for using pwm
  ledcAttachPin(DC_EN1, DCpwmChannel); // for using pwm

  Serial.begin(115200); //baud rate 115200
  while (!Serial) // Serial이 아닌 동안
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  // MPU6050 setup
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); 
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 
  print("Setup complete\n");
  delay(1000);
}

void loop() {

  //MPU6050
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp; //sensors_event
  mpu.getEvent(&a, &g, &temp); //getEvent
  ax_cal = ax_cal * alpha + (1.0 - alpha) * a.acceleration.x; //acceleration filtering
  ay_cal = ay_cal * alpha + (1.0 - alpha) * a.acceleration.y; //acceleration filtering
  az_cal = az_cal * alpha + (1.0 - alpha) * (a.acceleration.z-9.8); //acceleration filtering
  ax_fil = a.acceleration.x - ax_cal;
  ay_fil = a.acceleration.y - ay_cal;
  az_fil = a.acceleration.z - az_cal;
  total_ac = sqrt(ax_fil * ax_fil + ay_fil * ay_fil + az_fil * az_fil); //total acceleration 
  Serial.println("\ntotal: ");
  Serial.print(total_ac);

  //photodiode sensor
  int IsBall = digitalRead(16); //read photosensor
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
         if (IsBall==LOW){
      digitalWrite(DC_motor1Pin1, LOW);
      digitalWrite(DC_motor1Pin2, LOW); //dc stop
      //digitalWrite(WG_Pin1, LOW);
      //digitalWrite(WG_Pin2, HIGH); //contnraction at maximum speed
         }
      else(IsBall==HIGH){
        Serial.println("ball");
        break;
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
