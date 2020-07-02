/*
   Small Satellite Position Control Software.
   Filename: Open_Loop_SAT1_main.ino
   Author: Ajin Sunny
   Last Modified by: Ajin Sunny


   Written for Thesis: One dimensional Electromagnetic Actuation and Pulse Sensing.
   Version: 1.0
   Date: 02-25-2019
   Last Updated: 05-24-2019
*/

//HEADER FILES

#include <DueTimer.h>
#include <SineWaveDue.h>
#include <SD.h>
#include <SPI.h>
#include <VL53L0X.h>
#include "Arduino.h"
#include "DFRobot_VL53L0X.h"

char incomingByte;
DFRobotVL53L0X sensor;

unsigned long period = 50000; 
double dist[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
unsigned int i = 0;
double V_final = 0.0;
double velocity_final_final = 0.0;
double velocity_final[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
double vel[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
double previous_velocity = 0.0;
double current_velocity = 0.0;
double a = 0.9;
float Amplitude;
File myFile;

void setup()  
{ 
  analogReadResolution(10);
  analogWriteResolution(10);
  pinMode(9, OUTPUT);
  Serial.begin(115200);
  //delay(20000);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  

  Serial.print("Initializing SD card...");
  
    // see if the card is present and can be initialized:
    if (!SD.begin(4)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      while (1);
  }
  Serial.println("card initialized.");

  Wire.begin();
  //Set I2C sub-device address
  sensor.begin(0x50);
  //Set to Back-to-back mode and high precision mode
  sensor.setMode(Continuous, High);
  //Laser rangefinder begins to work
  sensor.start();
  myFile = SD.open("sat1.csv", FILE_WRITE);
  
  myFile.println(" ");
  myFile.print("Time");
  myFile.print(",");
  myFile.print("Distance"); 
  myFile.print(",");
  myFile.print("Velocity");
  myFile.print(",");
  myFile.print("Digital Amplitude");
  myFile.print(",");
  myFile.println("Amplitude");
  
//  myFile.println("Instantaneous Velocity"); 
//  myFile.print(" ");
//  myFile.print("Time");
//  myFile.print(" ");
//  myFile.print("Distance");
//  myFile.print(",");
//  myFile.print("Velocity"); 
//  
  
   
  while(Serial.available()==0){}
  incomingByte = Serial.read();
  if(incomingByte == 'A')
  {
  Serial.println(incomingByte);
  }
} 

void loop()  
{  
  S.startSinusoid1(10); 
  while(millis() < period) 
  {
    
    if(myFile)
    {
    //delay(7);
//    //Serial.print("Voltage value: ");
//    //Serial.println(S.return_voltage());
    //dist[i] = (sensor.getDistance()/10)+20;
   // i++;
   
    V_final = velocity_func();
    
    Serial.print("Time: ");
    Serial.println(millis());
    myFile.print(millis());
    myFile.print(",");
    
    Serial.print("Distance: ");
    Serial.println(dist[i]);
    myFile.print(dist[i]);
    myFile.print(",");
//    i++;
    Serial.print("Velocity: ");
    Serial.print(V_final);
    myFile.print(V_final);
    myFile.print(",");

    Serial.print("Digital Amplitude: ");
    Serial.println(S.returnAmplitude());
    myFile.print(S.returnAmplitude());
    myFile.print(",");

    Amplitude = (S.returnAmplitude() * 2.16)/490;
    
    Serial.print("Amplitude: ");
    Serial.println(Amplitude);
    myFile.println(Amplitude);

    
//    myFile.print(",");
//   // myFile.print("Time: ");
//    myFile.println(millis());
//
//    
//    if (i == 3)
//    {
//      dist[0] = dist[i-1];
//      i = 1;
//    }
////    
//    //delay(1000);
    }
    
  }
    myFile.close();
    //SD.remove("sat1.csv")
    S.stopSinusoid();
    exit(0);
} 


//double velocity_func(double dist[2])
//{
//  delta_pos = dist[i] - dist[i - 1];
//  velocity = delta_pos/0.016;
//  return velocity;
//}

double velocity_func()
{ 
  
  for(int k = 0; k < 8; k++)
  { 
    dist[i] = sensordistRead();   // Captures the distance  
    if(dist[i] > 220.00)
    {
      dist[i] = dist[i-1]; 
    }
    vel[i] = (dist[i] - dist[i - 1])/0.038;   // Calculates the velocity 
    current_velocity = vel[i+1];        // current velocity 
    previous_velocity = vel[i-1];     // previous velocity
    velocity_final[i+1] = a*velocity_final[i-1] + (1-a)*current_velocity;    // forward euler formula for the velocity. 
    velocity_final_final = velocity_final_final + velocity_final[i+1];      //sum the velocity to a double point variable. 
    
//  //Time
//  Serial.print("Time: ");
//  Serial.println(millis());
//  myFile.print(millis());
//  myFile.print(",");
//  
//  //Distance
//  Serial.print("Distance: ");
//  Serial.println(dist[i]);
//  myFile.print(dist[i]);
//  myFile.print(",");
//
//  //Instantaneous Velocity
//  Serial.print("Instantaneous Velocity: ");
//  Serial.println(velocity_final[i+1]);
//  myFile.println(velocity_final[i+1]);

    if (i == 7)
      {
        dist[0]=dist[i-1];    //shifts the array back to the 0th element of the array. 
        vel[0]=vel[i-1];    // shifts the velocity array back to the 0th element of the array. 
        velocity_final[0] = velocity_final[i-1]; // forward euler velocity
        i = 0;                // sets the counter back to the first position. 
      }     
      i++;         
  }


//  //Time
//  Serial.print("Time: ");
//  Serial.println(millis());
//  myFile.print(millis());
//  myFile.print(",");
//  
//  //Distance
//  Serial.print("Distance: ");
//  Serial.println(dist[i]);
//  myFile.print(dist[i]);
//  myFile.print(",");
  
  velocity_final_final = velocity_final_final/7;       //Average the velocity. 
  return velocity_final_final;
  
//  Serial.print("Velocity: ");
//  Serial.println(velocity_final_final);
//  myFile.println(velocity_final_final);
  
}

double sensordistRead()
{
  double relative_dist;
  relative_dist = ((sensor.getDistance()/10)+20);
  return relative_dist; 
}  
