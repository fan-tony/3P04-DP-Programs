//MAKE SURE YOU PLUG SDA INTO GPIO21 AND SCL INTO GPIO22
//I2C address is 0x28

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BluetoothSerial.h"

//the second I2C device will have its sda and scl pins set to these values
#define SDA_2 33
#define SCL_2 32

BluetoothSerial SerialBT;//setup bluetooth
Adafruit_BNO055 bno1; //initialize sensor object1
Adafruit_BNO055 bno2; //initialize sensor object2; this has the changed sda and scl 

void setup(void){
  //Initial Setup
  Serial.begin(9600);//begin serial information
  SerialBT.begin("ESP32");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code

  Wire.begin();
  Wire1.begin(SDA_2, SCL_2);
  bno1= Adafruit_BNO055(55);//declare sensor object1 to the adafruit class as sensor 55
  bno2= Adafruit_BNO055(55, 0x28, &Wire1);//go to overloaded constructor, by defining the I2C address along with a separate wire
  
  /* If there's an issue with setting up the sensors, then describe which sensor it was that had the issue */
  if(!bno1.begin()){/* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);//lock it in here because it wasn't detected
  }//end if statement
  
  if(!bno2.begin()){/* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 (2) detected ... Check your wiring or I2C ADDR!");
    while(1);//lock it in here because it wasn't detected
  }//end if statement

  delay(1000);
  bno1.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);
}

//Changes a passed in array to hold the values of the orientation found from the sensor
void getOrientation(sensors_event_t event, double orientation[]){
  orientation[0] = (event.orientation.x);
  orientation[1] = (event.orientation.y);
  orientation[2] = (event.orientation.z);
 }


double infoS[200][3];//global variable holding angle information of the shin; is an array that holds other arrays
double infoF[200][3];//global variable holding angle information of the foot; is an array that holds other arrays
int spot = 0;//initialize the spot of the array you're on
void loop(void){
  while (spot < int(200)){//only run this 200 times
    /* Get a new sensor event */
    sensors_event_t event1;//initialize event as a sensor event for sensor object 1
    sensors_event_t event2;//initialize event as a sensor event for sensor object 2
    
    bno1.getEvent(&event1);//get the event occuring and store it in event 1
    bno2.getEvent(&event2);//store the event from sensor 2 in event2's address

    //empty arrays to store x y and z orientation for the respective sensor
    double* orient1 = new double[3];
    double* orient2 = new double[3];
    getOrientation(event1, orient1);//get the orientation from sensor 1 and manipulate the array orient1
    getOrientation(event2, orient2);//get the orientation from sensor 2

    /* Store the obtained sensor x y and z, then push it to the global arrays storing the real time values; infoF and infoS*/
    infoF[spot][0] = orient1[0];
    infoF[spot][1] = orient1[1];
    infoF[spot][2] = orient1[2];

    infoS[spot][0] = orient2[0];
    infoS[spot][1] = orient2[1];
    infoS[spot][2] = orient2[2];
    
    Serial.print("X: ");
    Serial.print(infoF[0][0], 4);
    Serial.print("\tY: ");
    Serial.print(infoF[0][1], 4);
    Serial.print("\tZ: ");
    Serial.print(infoF[0][2], 4);
    Serial.println("");

    Serial.print("X: ");
    Serial.print(infoS[0][0], 4);
    Serial.print("\tY: ");
    Serial.print(infoS[0][1], 4);
    Serial.print("\tZ: ");
    Serial.print(infoS[0][2], 4);
    Serial.println("");
    
    
    /* Display the floating point data for orientation to your bluetooth com port
    SerialBT.print("X: ");
    SerialBT.print(event.orientation.x, 4);
    SerialBT.print("\tY: ");
    SerialBT.print(event.orientation.y, 4);
    SerialBT.print("\tZ: ");
    SerialBT.print(event.orientation.z, 4);
    SerialBT.println("");
    */
    
  /* //testing the acceleration vectors
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
  
  delay(1000);
  }
}
