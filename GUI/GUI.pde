import controlP5.*; //import ControlP5 library
import processing.serial.*; //import serial library

Serial port; //Create port variable
String val; //Data received from the serial port

ControlP5 cp5; //create ControlP5 object
PFont buttonfont; //create custom font for button
PFont titlefont; //create custom font for title
PFont textfont; //create custom font for text
color STOPColor, STARTColor, baseColor;
color STOPHighlight, STARTHighlight;
color currentColor;
boolean STOP = false;
boolean START = false;

void settings(){size(1000, 1000);}

void setup(){ //
  settings();
  println(Serial.list());
  port = new Serial(this, "COM10", 9600); //initializing serial port
  port.bufferUntil ('\n'); //receiving data from Arduino IDE
  
  //Adding button to empty window
  cp5 = new ControlP5(this);
  buttonfont = createFont("calibri", 20); //customizing font for button
  titlefont = createFont("calibri bold", 45); //customizing font for title
  textfont = createFont("calibri italic", 25); //customizing font for text
    
  cp5.addButton("START")
    .setPosition(350, 900)
    .setSize(200, 75)
    .setFont(buttonfont) //settting font for button
  ;
  
  cp5.addButton("STOP")
    .setPosition(1275, 900)
    .setSize(200, 75)
    .setFont(buttonfont)
  ;
   
}

void draw(){ //loop

    if (port.available() > 0){
      val = port.readStringUntil('\n'); //read and store it in val
    }
    println(val); //print it out in the console
    
    background(255); //background colour of window (r, g, b) or (0 to 255)
    textAlign(CENTER);
    text("MOBILE GAIT ANALYSIS LAB", width/2, 50); //title of window ("text", x coordinate, y coordinate)
    fill(100,100,100); //text colour (r, g, b)
    textFont(textfont); //setting font for text
    text("Please press START to begin analysis.", width/2, 100);
    textFont(titlefont); //setting font for title
    fill(100,100,175); //title colour (r, g, b)  

         
}

//void keyPressed()
//{
 
 
  //switch (keyCode) { //Switch case: Sending different signals and filling different buttons according to which button was pressed.   
    //case START: //In case the START button was pressed:
   //myPort.write('1'); // Send the signal 1
      //println("START"); // + Print "START" (Debugging only) 
 //fill(255,0,0); // + Fill the up START button with red.
//triangle(750, 235, 800, 160, 850, 235);
 
     //break;
     
    //case ' ' :
 //myPort.write('0');
      //println("STOP");
 //fill(255,0,0);
 //rect(750, 250, 100, 100,7);
   

//break;
    //default:
    //break;
  //}
//}

void update(int x, int y) {
  if (START) {
    START = true;
    STOP = false;
  } else if (STOP){
    STOP = true;
    START = false;
  } else {
    STOP = START = false;
  }
}

void mousePressed() {
  if (START) {
    port.write('1'); //start arduino program
  }
  if (STOP) {
    port.write('0'); //exit aruino program
  }
}

boolean START(int x, int y, int width, int height)  {
  if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

boolean STOP(int x, int y, int diameter) {
  float disX = x - mouseX;
  float disY = y - mouseY;
  if (sqrt(sq(disX) + sq(disY)) < diameter/2 ) {
    return true;
  } else {
    return false;
  }
}
