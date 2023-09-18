#include <Wire.h>

const int MPU6050_ADDRESS = 0x68; // MPU6050 I2C address
const int CONFIG = 0x1A;          // Configuration Register
const int PWR_MGMT_1 = 0x6B;      // Power Management 1 register
const int ACCEL_CONFIG = 0x1C;    // Accelerometer configuration register
const int GYRO_CONFIG = 0x1B;     // Gyroscope configuration register
const int ACCEL_XOUT_H = 0x3B;    // Accelerometer data registers
const int GYRO_XOUT_H = 0x43;     // Gyroscope data registers
const int AFS_SEL = 0x00;         // Accelerometer Full Scale Value set to +-2g
const int FS_SEL = 0x01;          // Gyroscope Full Scale Value set to +-500 degree/sec
const int DLPF_CFG = 0x01;        // Digital Low Pass Filter (Accl Bandwidth 184 Hz, delay 2.0 ms, Gyro Bandwidth 188 Hz, delay 1.9 ms)


int16_t accelerometer_x, accelerometer_y, accelerometer_z;
float ax, ay, az;
int16_t gyro_x, gyro_y, gyro_z;
float gx, gy, gz;
float ax_off = 0.0;
float ay_off = 0.0;
float az_off = 0.0;
float gx_off = 0.0;
float gy_off = 0.0;
float gz_off = 0.0;

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
  //Wire.write(0x00); // Set accelerometer full scale range to +/- 2g
  Wire.write(AFS_SEL);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(GYRO_CONFIG);
  //Wire.write(0x00); // Set gyroscope full scale range to +/- 250 degrees/s
  Wire.write(FS_SEL);
  Wire.endTransmission(true);

  calibrateMPU();
}

void loop() {
  // Read accelerometer data
  readMPUdata();
  
  ax = ax - ax_off;
  ay = ay - ay_off;
  az = az - az_off + 9.80;
  gx = gx - gx_off;
  gy = gy - gy_off;
  gz = gz - gz_off;
  
  displayMPUdata();
  delay(100); // Adjust the delay as needed
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
  gx = (float(gyro_x)/32767.0)*(500)*(3.1416/180);
  gy = (float(gyro_x)/32767.0)*(500)*(3.1416/180);
  gz = (float(gyro_x)/32767.0)*(500)*(3.1416/180);
}

void calibrateMPU(){
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
  ax_off = ax_off/1000.0;
  ay_off = ay_off/1000.0;
  az_off = az_off/1000.0;
  gx_off = gx_off/1000.0;
  gy_off = gy_off/1000.0;
  gz_off = gz_off/1000.0;

  Serial.print("Accelerometer Offset: (m/s2) ");
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

void displayMPUdata(){
  Serial.print("Accelerometer: (m/s2) ");
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