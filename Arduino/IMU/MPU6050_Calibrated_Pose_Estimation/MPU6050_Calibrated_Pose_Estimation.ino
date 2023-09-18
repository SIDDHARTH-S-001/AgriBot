#include <Wire.h>
// Registers (Refer to Register-Map of MPU6050)
const int MPU6050_ADDRESS = 0x68; // MPU6050 I2C address
const int CONFIG = 0x1A;          // Configuration Register
const int PWR_MGMT_1 = 0x6B;      // Power Management 1 register
const int ACCEL_CONFIG = 0x1C;    // Accelerometer configuration register
const int GYRO_CONFIG = 0x1B;     // Gyroscope configuration register
const int ACCEL_XOUT_H = 0x3B;    // Accelerometer data registers
const int GYRO_XOUT_H = 0x43;     // Gyroscope data registers
const int AFS_SEL = 0x00;         // Accelerometer Full Scale Value set to +-2g
const int FS_SEL = 0x01;          // Gyroscope Full Scale Value set to +-500 degree/sec
const int DLPF_CFG = 0x05;        // Digital Low Pass Filter (Accl Bandwidth 184 Hz, delay 2.0 ms, Gyro Bandwidth 188 Hz, delay 1.9 ms)
// Variables
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // Accelerometer Readings (16 bit INT)
float ax, ay, az;                                          // Acceleration Values (m/s2)
int16_t gyro_x, gyro_y, gyro_z;                            // Gyroscope Readings (16 bit INT)
float gx, gy, gz;                                          // Gyroscope Values (rads/s)
float gx_deg = 0.0;
float gy_deg = 0.0;
float gz_deg = 0.0;                                     
// Acceleration and Gyroscope offsets
float ax_off = 0.0;                                        
float ay_off = 0.0;
float az_off = 0.0;
float gx_off = 0.0;
float gy_off = 0.0;
float gz_off = 0.0;
// Exponential Moving Average Values
float ema_ax, ema_ay, ema_az, ema_gx, ema_gy, ema_gz;   
float beta = 0.2;                                
// Variables for Orientation Estimation
float roll = 0.0;
float pitch = 0.0;
float yaw = 0.0;
float accel_roll = 0.0;                                    // Roll due to acceleration values
float accel_pitch = 0.0;                                   // Pitch due to acceleration values
float alpha = 0.98;                                        // Complementary Filter Parameter
float pi = 3.1416;
float time_step = 0.025;
// Variables for Position Estimation
float vx = 0.0;
float vy = 0.0;
float dvx = 0.0;
float dvy = 0.0;
float x = 0.0;
float y = 0.0;
float w = gz;                                              // Angular Velocity about Z axis (Veritcally Upwards).

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // Wake up the MPU6050 by clearing the sleep bit in PWR_MGMT_1
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(PWR_MGMT_1);
  Wire.write(0); // Clear the sleep bit
  Wire.endTransmission(true);
  // Configure the accelerometer and gyroscope
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(CONFIG);
  Wire.write(DLPF_CFG);
  Wire.write(ACCEL_CONFIG);
  Wire.write(AFS_SEL);
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(GYRO_CONFIG);
  Wire.write(FS_SEL);
  Wire.endTransmission(true);

  calibrateMPU(); // Calibrate MPU6050 to find offset values
  delay(2000);
}

void loop() {
  // readMPUdata();   // Read accelerometer data  
  emaMPUdata();
  ax = ax - ax_off;
  ay = ay - ay_off;
  az = az - az_off + 9.80; // Taking acceleration due to gravity as 9.80 m/s2
  gx = gx - gx_off;
  gy = gy - gy_off;
  gz = gz - gz_off;  
  //orientation_estimate();
  // position_estimate_non_holonomic();
  position_estimate_holonomic();
  display_pose();
  // displayMPUdata(); // Display the acceleration and gyro values
  delay(10); // Adjust the delay as needed
}

void readMPUdata(){
  // Read accelerometer data
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDRESS, 6, true);
  accelerometer_x = Wire.read() << 8 | Wire.read();
  accelerometer_y = Wire.read() << 8 | Wire.read();
  accelerometer_z = Wire.read() << 8 | Wire.read();

  ax = (float(accelerometer_x)/32767.0)*(2*9.8);
  ay = (float(accelerometer_y)/32767.0)*(2*9.8);
  az = (float(accelerometer_z)/32767.0)*(2*9.8);
  
  // Read gyroscope data
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDRESS, 6, true);
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();  

  gx = (float(gyro_x)/32767.0)*(500)*(pi/180);
  gy = (float(gyro_y)/32767.0)*(500)*(pi/180);
  gz = (float(gyro_z)/32767.0)*(500)*(pi/180);
}

void avgMPUval(){
  for (int j = 0; j < 100; j++){
    readMPUdata();
    ax += ax;
    ay += ay;
    gx += az;
    gy += gy;
    gz += gz;
  }
  ax = ax/100;
  ay = ay/100;
  az = az/100;
  gx = gx/100;
  gy = gy/100;
  gz = gz/100;
}

void emaMPUdata(){
  ema_ax = ax; ema_ay = ay; ema_az = az; ema_gx = gx; ema_gy = gy; ema_gz = gz;

  for (int j = 0; j < 10; j++){
    readMPUdata();
    ema_ax = beta*ax + (1-beta)*ema_ax;
    ema_ay = beta*ay + (1-beta)*ema_ay;
    ema_az = beta*az + (1-beta)*ema_az;
    ema_gx = beta*gx + (1-beta)*ema_gx;
    ema_gy = beta*gy + (1-beta)*ema_gy;
    ema_gz = beta*gz + (1-beta)*ema_gz;
  }
  ax = ema_ax;
  ay = ema_ay;
  az = ema_az;
  gx = ema_gx;
  gy = ema_gy;
  gz = ema_gz;
}

void calibrateMPU(){
  // Sample the readings for 1000 iterations
  for (int i = 0; i < 1000; i++) { // Adjust the number of readings as needed
    readMPUdata();
    ax_off += ax;
    ay_off += ay;
    az_off += az;
    gx_off += gx;
    gy_off += gy;
    gz_off += gz;
    delay(1); // Adjust delay as needed
  }  
  // Calculate all the offset values
  ax_off = ax_off/1000.0;
  ay_off = ay_off/1000.0;
  az_off = az_off/1000.0;
  gx_off = gx_off/1000.0;
  gy_off = gy_off/1000.0;
  gz_off = gz_off/1000.0;

  // Print all offset values
  Serial.print(" Accelerometer Offset: (m/s2) ");
  Serial.print(ax_off);
  Serial.print(", ");
  Serial.print(ay_off);
  Serial.print(", ");
  Serial.print(az_off);
  Serial.print(" | Gyroscope Offset: (rads/s) ");
  Serial.print(gx_off);
  Serial.print(", ");
  Serial.print(gy_off);
  Serial.print(", ");
  Serial.println(gz_off);
}

void orientation_estimate(){
  accel_roll = atan2(ay, sqrt((ax*ax)+(az*az)))*(180/pi);
  accel_pitch = atan2(ax, sqrt((ay*ay)+(az*az)))*(180/pi);

  gx_deg = gx*(180/pi);
  gy_deg = gy*(180/pi);
  gz_deg = gz*(180/pi);
  
  roll = alpha*(roll + (gx_deg*time_step)) + (1-alpha)*accel_roll;
  pitch = alpha*(pitch + (gy_deg*time_step)) + (1-alpha)*accel_pitch;
  yaw = yaw + (gz_deg*time_step);

  Serial.print(" | Angles: (rads/s) ");
  Serial.print(roll);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.println(yaw);
}

void position_estimate_holonomic(){
  dvx = ax*(time_step);
  dvy = ay*(time_step);
  vx = vx + dvx;
  vy = vy + dvy;
  x = x + dvx*(time_step);
  y = y + dvy*(time_step);
}

void position_estimate_non_holonomic(){
  dvx = ax*(time_step);
  dvy = ay*(time_step);
  vx = vx + dvx;
  x = x + dvx*cos(yaw*pi/180)*(time_step);
  y = y + dvx*sin(yaw*pi/180)*(time_step);
}

void display_pose(){
  Serial.print(" x (m): ");
  Serial.print(x);
  Serial.print(" y (m): ");
  Serial.print(y); 
  Serial.print(" dvx (m/s): ");
  Serial.print(dvx);
  Serial.print(" dvy (m/s): ");
  Serial.print(dvy); 
  Serial.print(" Yaw (deg): ");
  Serial.println(yaw);
}

void displayMPUdata(){
  // Print acceleration and gyro values
  Serial.print(" Accelerometer: (m/s2) ");
  Serial.print(ax);
  Serial.print(", ");
  Serial.print(ay);
  Serial.print(", ");
  Serial.print(az);
  Serial.print(" | Gyroscope: (rads/s) ");
  Serial.print(gx);
  Serial.print(", ");
  Serial.print(gy);
  Serial.print(", ");
  Serial.println(gz);
}