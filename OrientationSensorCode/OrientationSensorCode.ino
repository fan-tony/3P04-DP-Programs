//CODE FOR INTERFACING TWO BNO055 ORIENTATION SENSORS AND THE ESP32 
//Sensor1 sda=GPIO21, scl-gpio22 ;foot
//Sensor 2 sda=gpio33; scl=gpio32 ; shin
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

/******************
* Global variables*
******************/
double stop1[3]={0,0,0};//standing still acceleration of x y and z
double stop2[3]={0,0,0};
int stopFlag = 0;

void setup(void){
  //Initial Setup
  Serial.begin(9600);//begin serial information
  SerialBT.begin("ESP32Team5");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code
  SerialBT.println("Begin Orientation Sensor test");

  Wire.begin();
  Wire1.begin(SDA_2, SCL_2);
  bno1= Adafruit_BNO055(55);//declare sensor object1 to the adafruit class as sensor 55
  bno2= Adafruit_BNO055(55, 0x28, &Wire1);//go to overloaded constructor, by defining the I2C address along with a separate wire
  
  /* If there's an issue with setting up the sensors, then describe which sensor it was that had the issue */
  if(!bno1.begin()){/* There was a problem detecting the BNO055 ... check your connections */
    SerialBT.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);//lock it in here because it wasn't detected
  }//end if statement
  
  if(!bno2.begin()){/* There was a problem detecting the BNO055 ... check your connections */
    SerialBT.print("Ooops, no BNO055 (2) detected ... Check your wiring or I2C ADDR!");
    while(1);//lock it in here because it wasn't detected
  }//end if statement

  delay(1000);
  bno1.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);

  //average the user's standing still value over 5 seconds, and take 100 samples
  for (int i=0;i<100;i++){
    sensors_event_t stopEvent1;//initialize event as a sensor event for sensor object 1
    sensors_event_t stopEvent2;//initialize event as a sensor event for sensor object 2
  
    //Get a new sensor event for both sensor objects
    bno1.getEvent(&stopEvent1,Adafruit_BNO055::VECTOR_LINEARACCEL);//get the event occuring and store it in event 1
    bno2.getEvent(&stopEvent2,Adafruit_BNO055::VECTOR_LINEARACCEL);//store the event from sensor 2 in event2's address
  
    stop1[0]+=stopEvent1.acceleration.x;
    stop1[1]+=stopEvent1.acceleration.y;
    stop1[2]+=stopEvent1.acceleration.z;
    stop2[0]+=stopEvent2.acceleration.x;
    stop2[1]+=stopEvent2.acceleration.y;
    stop2[2]+=stopEvent2.acceleration.z;
    delay(50);
  }
  
  for (int i=0;i<3;i++){//average them out now because 100 samples were just taken
    stop1[i]/=100;
    stop1[i]+=0.05;//add a small buffer zone for stopping
    stop2[i]/=100;
    stop2[i]+=0.05;//add a small buffer zone for stopping
  }
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

void infoToString(double info[3]){//info is expressed as a double array where each array holds 3 elements denoting x, y, and z
  SerialBT.print("X: ");
  SerialBT.print(info[0], 4);
  SerialBT.print("\tY: ");
  SerialBT.print(info[1], 4);
  SerialBT.print("\tZ: ");
  SerialBT.print(info[2], 4);
  SerialBT.println("");
}

boolean checkStop(){//gets the current acceleration, and compares it to the average stopping acceleration
  double accel1[3];
  double accel2[3];
  sensors_event_t stopEvent1;//initialize event as a sensor event for sensor object 1
  sensors_event_t stopEvent2;//initialize event as a sensor event for sensor object 2

  //Get a new sensor event for both sensor objects
  bno1.getEvent(&stopEvent1, Adafruit_BNO055::VECTOR_LINEARACCEL);//get the event occuring and store it in event 1
  bno2.getEvent(&stopEvent2, Adafruit_BNO055::VECTOR_LINEARACCEL);//store the event from sensor 2 in event2's address
  
  accel1[0]+=stopEvent1.acceleration.x;
  accel1[1]+=stopEvent1.acceleration.y;
  accel1[2]+=stopEvent1.acceleration.z;
  accel2[0]+=stopEvent2.acceleration.x;
  accel2[1]+=stopEvent2.acceleration.y;
  accel2[2]+=stopEvent2.acceleration.z;

  SerialBT.print("X: ");
  SerialBT.print(stop1[0]);
  SerialBT.print(" Y: ");
  SerialBT.print(stop1[1]);
  SerialBT.print(" Z: ");
  SerialBT.print(stop1[2]);
  SerialBT.println("");

  SerialBT.print("X: ");
  SerialBT.print(abs(accel2[0]));
  SerialBT.print(" Y: ");
  SerialBT.print(abs(accel2[1]));
  SerialBT.print(" Z: ");
  SerialBT.print(abs(accel2[2]));
  SerialBT.println("");

  if(abs(accel1[0]) < stop1[0]  || abs(accel1[1]) < stop1[1] || abs(accel1[2]) < stop1[2] || abs(accel2[0]) < stop2[0] || abs(accel2[1]) < stop2[1] || abs(accel2[2]) < stop2[2]){//if any acceleration values are under the average stopping, then stop sending data 
    return true;
    }
    return false;//otherwise you are in motion
  }
  

/********************************
 * Main method to be implemented*
 *******************************/
void loop(void){
  //first determine if the current data is useful; take measurements for 5 seconds 

  //Initialize local variables to be used
  double* orient1 = new double[3];//holds xyz for sensor 1
  double* orient2 = new double[3];//holds xyz for sensor 2
  double angle[3];//holds angle between two sensors for xyz
  sensors_event_t event1;//initialize event as a sensor event for sensor object 1
  sensors_event_t event2;//initialize event as a sensor event for sensor object 2

  //Get a new sensor event for both sensor objects
  bno1.getEvent(&event1);//get the event occuring and store it in event 1
  bno2.getEvent(&event2);//store the event from sensor 2 in event2's address

  //store xyz in orient1 and orient2
  getOrientation(event1, orient1);//get orientation values
  getOrientation(event2, orient2);

  //calculate the angle between the two
  for(int i=0;i<3;i++){
    angle[i] = orient2[i]-orient1[i];//angle becomes the difference between the two sensors
  }
  
  //print them out to verify 
  //infoToString(angle);
  SerialBT.println("DATA OUTGOING");



  if (checkStop()==true){stopFlag +=1;}//check if the person is standing still 
  if(stopFlag==50){//if the person has been standing still for 5 second (50 * 100ms delay at the end of the loop)
    while(checkStop()==true){
    //then do nothing, until the program reads that they're moving again
    SerialBT.println("PROGRAM STOPPED");
    delay(100);
    }
  stopFlag=0;//if you're out here then you've started moving again, so reset the stopFlag counter
  }
  
  delay(100);//100ms wait time between each reading

}
