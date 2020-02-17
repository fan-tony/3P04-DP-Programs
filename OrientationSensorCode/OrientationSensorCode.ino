//MAKE SURE YOU PLUG SDA INTO GPIO21 AND SCL INTO GPIO22

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;//setup bluetooth
Adafruit_BNO055 bno = Adafruit_BNO055(55);//initialize sensor object
void setup(void)
{
SerialBT.begin("ESP32");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code
Serial.begin(9600);
Serial.println("Orientation Sensor Test");//making sure that everything's working by printing this statement
Serial.println("");

/* Initialise the sensor */
if(!bno.begin()){/* There was a problem detecting the BNO055 ... check your connections */
Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
while(1);
}

delay(1000);
bno.setExtCrystalUse(true);
}

int* getOrientation(){
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  //new array holding the orientation of sensor 1; the x y and z rotation  to 4 digits
  int orient1 [3];
  orient1[0] = (event.orientation.x, 4);
  orient1[1] = (event.orientation.y, 4);
  orient1[2] = (event.orientation.z, 4);
  return orient1;
  /*
  Serial.print(event.orientation.x, 4);
  Serial.print(event.orientation.y, 4);
  Serial.print(event.orientation.z, 4);
  */
 }
 
void loop(void){
  /*
  Serial.print(getOrientation()[0]);
  Serial.print(getOrientation()[1]);
  Serial.print(getOrientation()[2]);
  */
//Testing the orientation values
  sensors_event_t event;
  bno.getEvent(&event);
  /* Display the floating point data for orientation to your bluetooth com port*/
  SerialBT.print("X: ");
  SerialBT.print(event.orientation.x, 4);
  SerialBT.print("\tY: ");
  SerialBT.print(event.orientation.y, 4);
  SerialBT.print("\tZ: ");
  SerialBT.print(event.orientation.z, 4);
  SerialBT.println("");
  
/* //testing the acceeration vectors
imu::Vector<3> accel1 = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
//Display the floating point data 
Serial.print("X: ");
Serial.print(accel1.x());
Serial.print(" Y: ");
Serial.print(accel1.y());
Serial.print(" Z: ");
Serial.print(accel1.z());
Serial.println("");
*/

delay(100);
}
