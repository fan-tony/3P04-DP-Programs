//CODE FOR INTERFACING TWO BNO055 ORIENTATION SENSORS AND THE ESP32 
//Sensor1 sda=GPIO21, scl-gpio22
//Sensor 2 sda=gpio33; scl=gpio32
//I2C address is 0x28
//Treat sensor 1 as the foot sensor and sensor 2 as the shin sensor

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BluetoothSerial.h"

//the second I2C device will have its sda and scl pins set to these values
#define SDA_2 33
#define SCL_2 32

/*Initial Setup*/
BluetoothSerial SerialBT;//setup bluetooth
Adafruit_BNO055 bno1; //initialize sensor object1
Adafruit_BNO055 bno2; //initialize sensor object2; this has the changed sda and scl 

void setup(void){
  //Initial Setup
  Serial.begin(9600);//begin serial information
  SerialBT.begin("ESP32");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code
  Serial.println("Begin Orientation Sensor test");

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

/**********************************************
 * Functions to be used in the main method    *
 **********************************************/
void getOrientation(sensors_event_t event, double orientation[]){
//Changes a passed in array to hold the values of the orientation found from the sensor
  orientation[0] = (event.orientation.x);
  orientation[1] = (event.orientation.y);
  orientation[2] = (event.orientation.z);
 }

void infoToString(double info[][3], int spot){//info is expressed as a double array where each array holds 3 elements denoting x, y, and z
  SerialBT.print("X: ");
  SerialBT.print(info[spot][0], 4);
  SerialBT.print("\tY: ");
  SerialBT.print(info[spot][1], 4);
  SerialBT.print("\tZ: ");
  SerialBT.print(info[spot][2], 4);
  SerialBT.println("");
}
/******************
* Global variables*
******************/
double infoS[200][3];//global variable holding angle information of the shin; is an array that holds other arrays
double infoF[200][3];//global variable holding angle information of the foot; is an array that holds other arrays
double angle[200][3];//holds the difference between the two angles 
int spot = 0;//initialize the spot of the array you're on


/********************************
 * Main method to be implemented*
 *******************************/
void loop(void){
  //first determine if the current data is useful; ie, if the foot is accelerating, meaning movement is occuring, then we want to start recording the data
  /* //testing the acceleration vectors*/
  imu::Vector<3> accel1 = bno1.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  imu::Vector<3> accel2 = bno2.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  //Display the floating point data 
  /*
  Serial.print("X: ");
  Serial.print(accel1.x());
  Serial.print(" Y: ");
  Serial.print(accel1.y());
  Serial.print(" Z: ");
  Serial.print(accel1.z());
  Serial.println("");
  */
  
  if (spot < int(200)){//only collect 200 instances of data
    sensors_event_t event1;//initialize event as a sensor event for sensor object 1
    sensors_event_t event2;//initialize event as a sensor event for sensor object 2

    /* Get a new sensor event for both sensor objects*/
    bno1.getEvent(&event1);//get the event occuring and store it in event 1
    bno2.getEvent(&event2);//store the event from sensor 2 in event2's address

    //empty arrays to store x y and z orientation for the respective sensor
    double* orient1 = new double[3];
    double* orient2 = new double[3];
    getOrientation(event1, orient1);//get the orientation from sensor 1 and manipulate the array orient1
    getOrientation(event2, orient2);//get the orientation from sensor 2

    /* Store x y and z values in orient in the global arrays of info    */  
    for(int i=0;i<3;i++){
      infoF[spot][i] = orient1[i];
      infoS[spot][i] = orient2[i];
      angle[spot][i] = orient2[i]-orient1[i];//angle becomes the difference between the two sensors
    }
    
    //print them out to verify 
    infoToString(angle,spot);


  spot++;//move to the next index of the arrays 
  delay(200);
  }
}
