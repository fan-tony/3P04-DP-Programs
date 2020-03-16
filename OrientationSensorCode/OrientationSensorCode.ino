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
double stop1 = 0;//standing still acceleration of x y and z
double stop2 = 0;
int stopFlag = 0;

void setup(void){

  //Initial Setup
  Serial.begin(9600);//begin serial information
  SerialBT.begin("ESP32Team5");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code
  SerialBT.println("zAccel");//print header for CSV file

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
  
    stop1 = stop1 + ((abs(stopEvent1.acceleration.x) + abs(stopEvent1.acceleration.y) + abs(stopEvent1.acceleration.z))/3);//
    stop2 = stop2 + ((abs(stopEvent2.acceleration.x) + abs(stopEvent2.acceleration.y) + abs(stopEvent2.acceleration.z))/3);
    delay(50);
  }
  
  //average them out now because 100 samples were just taken
  stop1/=100;
  stop1+=0.05;//add a small buffer zone for stopping
  stop2/=100;
  stop2+=0.05;//add a small buffer zone for stopping
  
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


boolean checkStop(){//gets the current acceleration, and compares it to the average stopping acceleration
  double accel1 = 0;
  double accel2 = 0;
  sensors_event_t stopEvent1;//initialize event as a sensor event for sensor object 1
  sensors_event_t stopEvent2;//initialize event as a sensor event for sensor object 2

  //Get a new sensor event for both sensor objects
  bno1.getEvent(&stopEvent1, Adafruit_BNO055::VECTOR_LINEARACCEL);//get the event occuring and store it in event 1
  bno2.getEvent(&stopEvent2, Adafruit_BNO055::VECTOR_LINEARACCEL);//store the event from sensor 2 in event2's address
  
  accel1 = accel1 + ((abs(stopEvent1.acceleration.x) + abs(stopEvent1.acceleration.y) + abs(stopEvent1.acceleration.z))/3);
  accel2 = accel2 + ((abs(stopEvent2.acceleration.x) + abs(stopEvent2.acceleration.y) + abs(stopEvent2.acceleration.z))/3);

  if(accel1<stop1 || accel2<stop2){//if current accleration doesn't exceed threshold of stopping
    return true;//you are stopped and not in motion
    }
    return false;//otherwise you are in motion
  }
  

void infoToString(double info[3]){//info is expressed as a double array where each array holds 3 elements denoting x, y, and z
  //SerialBT.print("X: ");
  //SerialBT.print(info[0], 4);
  //SerialBT.print("\tY: ");
  //SerialBT.print(info[1], 4);
  //SerialBT.print("\tZ: ");
  SerialBT.println(info[2], 4);//prints only the z component of the change in angle
  //SerialBT.println("");
}
/********************************
 * Main method to be implemented*
 *******************************/
void loop(void){
  int state = SerialBT.read();

  if (state == 0){}

  else if (state == 1){
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
    infoToString(angle);
  
  
    if (checkStop()==true){stopFlag +=1;}//check if the person is standing still 
    if(stopFlag==50){//if the person has been standing still for 5 second (50 * 100ms delay at the end of the loop)
      while(checkStop()==true){
      //then do nothing, until the program reads that they're moving again
      delay(50);
      }
    stopFlag=0;//if you're out here then you've started moving again, so reset the stopFlag counter
    }
    
    delay(50);//100ms wait time between each reading
  }
}
