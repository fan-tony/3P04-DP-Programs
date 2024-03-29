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


/******************
* Global variables*
******************/
double stop1 = 0;//standing still acceleration of x y and z combined

int stopFlag = 0;

void setup(void){
  //Initial Setup
  Serial.begin(9600);//begin serial information
  SerialBT.begin("ESP32Team5");//search your bluetooth devices for ESP32 and connect once the esp is powered and running the code
  Serial.println("angle");//print header for CSV file

  Wire.begin();
  Wire1.begin(SDA_2, SCL_2);
  bno1= Adafruit_BNO055(55);//declare sensor object1 to the adafruit class as sensor 55

  
  /* If there's an issue with setting up the sensors, then describe which sensor it was that had the issue */
  if(!bno1.begin()){/* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);//lock it in here because it wasn't detected
  }//end if statement

  delay(1000);
  bno1.setExtCrystalUse(true);

  //average the user's standing still value over 5 seconds, and take 100 samples
  for (int i=0;i<100;i++){
    sensors_event_t stopEvent1;//initialize event as a sensor event for sensor object 1

    bno1.getEvent(&stopEvent1,Adafruit_BNO055::VECTOR_LINEARACCEL);//get the event occuring and store it in event 1
  
    stop1 = stop1 + ((abs(stopEvent1.acceleration.x) + abs(stopEvent1.acceleration.y) + abs(stopEvent1.acceleration.z))/3);//

    delay(50);
  }
  
  //average them out now because 100 samples were just taken
  stop1 = (stop1/100) + 0.05;//add a small buffer zone for stopping

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

  sensors_event_t stopEvent1;//initialize event as a sensor event for sensor object 1

  //Get a new sensor event for both sensor objects
  bno1.getEvent(&stopEvent1, Adafruit_BNO055::VECTOR_LINEARACCEL);//get the event occuring and store it in event 1

  accel1 = accel1 + ((abs(stopEvent1.acceleration.x) + abs(stopEvent1.acceleration.y) + abs(stopEvent1.acceleration.z))/3);

  if(accel1<stop1){//if current accleration doesn't exceed threshold of stopping
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
  Serial.println(info[2], 4);//prints only the z component of the change in angle
  //SerialBT.println("");
}
/********************************
 * Main method to be implemented*
 *******************************/
void loop(void){
  //first determine if the current data is useful; take measurements for 5 seconds 

  //Initialize local variables to be used
  double orient1[3];//holds xyz for sensor 1
  
  sensors_event_t event1;//initialize event as a sensor event for sensor object 1


  //Get a new sensor event for both sensor objects
  bno1.getEvent(&event1);//get the event occuring and store it in event 1


  //store xyz in orient1 and orient2
  getOrientation(event1, orient1);//get orientation values

  //calculate the angle between the two
  
  //print them out to verify 
  infoToString(orient1);


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
