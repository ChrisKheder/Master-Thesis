#include "Magnet.h"

//IMU initializing
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

//---------Setup Function-----------------
void MagScanSetup(){
  Wire.begin(2, 1);
  if(!myMPU9250.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected");
  }
  if(!myMPU9250.initMagnetometer()){
    Serial.println("Magnetometer does not respond");
  }
  else{
    Serial.println("Magnetometer is connected");
  }

   //In trigger mode the AK8963 goes into power down after the measurement
  myMPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
  
  // In continuous mode you need to wait for the first data to be available.
  delay(200);
}
//----------------------------------------

//---------Measurment Function------------
MagData MagScan(int M){
  Serial.println("Measuring magnetic field strength...");
  float sumX = 0, sumY = 0, sumZ = 0;

  for (int i = 0; i < M; i++) {
    xyzFloat magValue = myMPU9250.getMagValues();
    
    float currX = magValue.x;
    float currY = magValue.y;
    float currZ = magValue.z;

    sumX += currX;
    sumY += currY;
    sumZ += currZ;

    delay(20); // Delay between measurements, adjust as needed
  }

  float avgX = sumX / M;
  float avgY = sumY / M;
  float avgZ = sumZ / M;
  float avgMag = sqrt(avgX * avgX + avgY * avgY + avgZ * avgZ);

  MagData result;
  result.avg.x = avgX ;
  result.avg.y = avgY ;
  result.avg.z = avgZ ;
  result.avgMag = int(avgMag);
  return result;
}
//----------------------------------------