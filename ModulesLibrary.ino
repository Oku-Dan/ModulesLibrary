#include "Wire.h"

TwoWire Wire1(1,I2C_FAST_MODE,100000);
TwoWire Wire2(2,I2C_FAST_MODE,100000);

#include "BMP180.h"
#include "MPU9250.h"

#include "SD.h"

File file;

bool bmp_available = false;
bool mpu_available = false;
bool sd_available = false;

void setup(){
  Serial.begin(115200);
  while(!Serial);


  Serial.print("STM32 is initializing BMP180...");
  
  bmp_available = BMP180_Initialize();
  
  if(bmp_available)Serial.println("success!!");
  else Serial.println("failed.");

  
  Serial.print("STM32 is initializing MPU9250...");
  
  mpu_available = MPU9250_Initialize(MPU9250_ACCEL_SCALE_16G,MPU9250_GYRO_SCALE_2000DPS,MPU9250_MAG_RATE_100Hz);
  
  if(mpu_available)Serial.println("success!!");
  else Serial.println("failed.");


  Serial.print("STM32 is making new file on SD card...");
  
  sd_available = SD.begin(PA4);
  
  if(sd_available){
    if(SD.exists("DATA.CSV"))SD.remove("DATA.CSV");
    file = SD.open("DATA.CSV",FILE_WRITE);
    file.println("Time[ms],Pressure[Pa],Temperature[deg],AccelX[G],AccelY[G],AccelZ[G],");
    Serial.println("success!!");
  }else{
    Serial.println("failed.");
  }
  
}

void loop(){
  if(bmp_available){
    float pressure,temperature;
    BMP180_ReadPrs(&pressure);
    BMP180_ReadTmp(&temperature);

    if(sd_available){
      file.print(millis());
      file.print(',');
      file.print(pressure);
      file.print(',');
      file.print(temperature);
      file.println(",,,,");
    }
  }

  if(mpu_available){
    float x,y,z;
    MPU9250_ReadAcc(&x,&y,&z);
    
    if(sd_available){
      file.print(millis());
      file.print(",,,");
      file.print(x);
      file.print(',');
      file.print(y);
      file.print(',');
      file.print(z);
      file.println(',');
    }
  }

  delay(100);
}
