// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

//worm gear motor
int enable1Pin = 12;
int motor1Pin1 = 13;
int motor1Pin2 = 14;

//setting PWM properties
const int freq = 3000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

//for acceleration calculation
float ax_cal = 0.0;
float ay_cal = 0.0;
float az_cal = 0.0;
float ax_fil = 0.0;
float ay_fil = 0.0;
float az_fil = 0.0;
float total_ac = 0.0;
float alpha = 0.8; // 필터 강도 조절 파라미터 (0.0 ~ 1.0)

void setup(void) {
  
  //set motor pins as outputs;
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  //configure LED
  ledcSetup(pwmChannel, freq, resolution);

  //attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(5000);

  Serial.println("");
  delay(100);
}

void loop() {
  //initially stop
  
  
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  ax_cal = ax_cal * alpha + (1.0 - alpha) * a.acceleration.x;
  ay_cal = ay_cal * alpha + (1.0 - alpha) * a.acceleration.y;
  az_cal = az_cal * alpha + (1.0 - alpha) * a.acceleration.z;
  ax_fil = a.acceleration.x - ax_cal;
  ay_fil = a.acceleration.y - ay_cal;
  az_fil = a.acceleration.z - az_cal;
  total_ac = sqrt(ax_fil * ax_fil + ay_fil * ay_fil + az_fil * az_fil); 
  
  
  /* Print out the values */
//  Serial.print("Acceleration X: ");
//  Serial.print(ax_fil);
//  Serial.print(", Y: ");
//  Serial.print(ay_fil);
//  Serial.print(", Z: ");
//  Serial.print(az_fil);
//  Serial.println(" m/s^2");

  Serial.println("total: ");
  Serial.print(total_ac);

//  Serial.print("\nRotation X: ");
//  Serial.print(g.gyro.x);
//  Serial.print(", Y: ");
//  Serial.print(g.gyro.y);
//  Serial.print(", Z: ");
//  Serial.print(g.gyro.z);
//  Serial.println(" rad/s");

  if(total_ac > 10){
    Serial.println("Shock detected!!");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH); //contnraction at maximum speed
    delay(1000);
    //stay contracted for 3s
    Serial.println("stay contracted");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    delay(5000);
    //relaxation(slowly)
    Serial.println("relaxing");
    dutyCycle = 100;
    ledcWrite(pwmChannel, dutyCycle);
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW); 
    delay(2000);
    ax_cal = 0.0;
    ay_cal = 0.0;
    az_cal = 0.0;
    Serial.println("stop");
    dutyCycle = 200;
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
  }

  Serial.println("");
  delay(500);
}
