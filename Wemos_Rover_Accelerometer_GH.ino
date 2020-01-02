
/*
SMARS Tracked Vehicle Controller via BLYNK and Wemos D1 mini WiFi 
On your phone in the Blynk app you need the following 4 widgets:
-SliderL on Virtual Pin: V0, Labelled Left -1023 to +1023
-SliderL on Virtual Pin V1, Labelled Right -1023 to +1023
-Switch on Virtual Pin V2, Labelled Run/Stop
-Accelerometer on Virtual Pin V3
-Switch on Virtual Pin V4 Labelled Run/Stop
The SMARS will operate in two modes with this setup
-Slider switches to increase or decrease and reverse track speed independent of each other to skid steer
and,
-Making use of the iPhone y and z axis accelerometers such that changing the orientation of the iphone
z-axis will increase or decrease and reverse track speed and y-axis will turn the bot left or right by
reducing the track speed relative to the amount of y-axis change.

Uses L9110 2-CHANNEL MOTOR DRIVER. Spec sheet-https://tinyurl.com/y88pgaka


// **********Use this code at your own risk!**********
*/
//**********Libraries**********

//#define BLYNK_USE_DIRECT_CONNECT
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//**********Blynk***********

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).

char auth[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Blynk Authentication Token

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "XXXXXXXXXXXX";// iphone wifi hotspot ssid
char pass[] = "XXXXXXXXXXXX";// iphone wifi hotspot password

//**********Blynk***********

//********************MOTORS***************************
#define motorA1A D1 //A1A Front Motor + 
#define motorA1B D2 //AIB Front Motor - 
#define motorB1A D5 //B1A Rear Motor +
#define motorB1B D6 //B1B Rear Motor -
//********************MOTORS***************************

//*************Slider Control Mode Variables****************
int leftMotorSpeed; //PWM value for motor speed
int rightMotorSpeed; //PWM value for motor speed
int stopGo = 0; //Blynk Switch for slider controls
//*************Slider Control Mode Variables****************

//*************Accelerometer Control Mode Variables*********
float y_Steer;  //Y-Axis accelerometer value
float z_Speed;  //Z-Axis accelerometer value
float motorSpeed; // PWM value from z_Speed for drive track
float motorSpeedADJ; //y-Steer adjusted PWM for skid turn track
int accel_Ready = 0; // Blynk V4 Switch On-Off Accelerometer Mode
//*************Accelerometer Control Mode Variables*********

//*****************************************************************************
//********************************VOID SETUP***********************************
//*****************************************************************************
void setup() {
  
Serial.begin(115200);
  
pinMode(motorA1A,OUTPUT); //Set Wemos D1 PIN to OUTPUT
pinMode(motorA1B,OUTPUT); //Set Wemos D2 PIN to OUTPUT
pinMode(motorB1A,OUTPUT); //Set Wemos D5 PIN to OUTPUT
pinMode(motorB1B,OUTPUT); //Set Wemos D6 PIN to OUTPUT

Blynk.begin(auth, ssid, pass); //START BLYNK
}
//**********************************************************************
//*************************END VOID SETUP*******************************
//**********************************************************************

//**********************************************************************
//***************************VOID LOOP**********************************
//**********************************************************************

  void loop(){

  Blynk.run();
 
}

//**************************************************************************
//*************************END VOID LOOP************************************
//**************************************************************************


//**********Read BLYNK LEFT Motor Slider - Slider Control Mode **********
BLYNK_WRITE(V0) {
  leftMotorSpeed = param.asInt();
  if (stopGo != 0 && accel_Ready != 1) {
  if (leftMotorSpeed > 0) {
    analogWrite(motorA1A, 0); //V0 is positive move left track forward
    analogWrite (motorA1B, leftMotorSpeed); 
  } else {
    leftMotorSpeed = -leftMotorSpeed;
    analogWrite (motorA1A, leftMotorSpeed); //V0 is negative move left track backward
    analogWrite (motorA1B, 0); //Turning Side slower than opposing side
  }
 }
}
//**********Read BLYNK LEFT Motor Slider - Slider Control Mode **********

 
//**********Read BLYNK RIGHT Motor Slider - Slider Control Mode **********
BLYNK_WRITE(V1) {
  rightMotorSpeed = param.asInt();
  if (stopGo != 0 && accel_Ready != 1) {
  if (rightMotorSpeed > 0) {
    analogWrite (motorB1A, 0); //V1 is positive move right track forward
    analogWrite (motorB1B, rightMotorSpeed); 
  } else {
    rightMotorSpeed = -rightMotorSpeed;
    analogWrite (motorB1A, rightMotorSpeed); //V1 is negative move right track backward
    analogWrite (motorB1B, 0); 
  }
 }
}
//**********Read BLYNK RIGHT Motor Slider - Slider Control Mode **********


//**********Read STOP / RUN Switch - Slider Control Mode **********
BLYNK_WRITE(V2) {
  stopGo = param.asInt();
  if (stopGo == 0)
      Blynk.virtualWrite (V0, 0); //Set the Blynk slide switches to zero
      Blynk.virtualWrite (V1, 0); //Set the Blynk slide switches to zero
      analogWrite (motorA1A, 0); 
      analogWrite (motorA1B, 0); 
      analogWrite (motorB1A, 0);
      analogWrite (motorB1B, 0); 
    }
//**********Read STOP / RUN Switch - Slider Control Mode **********


//**************** Accelerometer - Accelerometer Control Mode****************

BLYNK_WRITE(V3) {

  y_Steer = param [1].asFloat();  //Accelerometer y-axis for left(-)/right(+) turns
  z_Speed = param [2].asFloat();  //Accelerometer z-axis for forward(-)/reverse(+)

  if (accel_Ready == 0 && stopGo == 0) {    //if switch is off turn off motors
      analogWrite(motorA1A, 0); 
      analogWrite(motorA1B, 0); 
      analogWrite(motorB1A, 0);
      analogWrite(motorB1B, 0);
  }
  
  if (stopGo != 1 && accel_Ready != 0 ) {  //Skip this if stopGo button is ON.
    
  Serial.print ("  y_Steer: ");
  Serial.print (y_Steer);
  Serial.print ("  z_Speed: ");
  Serial.println (z_Speed);
  
  //if (accel_Ready ==1){  //ENGAGE Accelerometer Function
    
  //STOP 
 
  if ((z_Speed >= -0.3) && (z_Speed <= 0.3)) {
      analogWrite (motorA1A, 0); 
      analogWrite (motorA1B, 0); 
      analogWrite (motorB1A, 0);
      analogWrite (motorB1B, 0); 
      Serial.println (" stop  ");
     
  } else

  //FORWARD 

  if ((z_Speed < -0.3)  && (y_Steer < 0.3 && y_Steer > -0.3)) {
        
      motorSpeed = (z_Speed * -1023);
      
      Serial.print (" Forward  "); Serial.print ("  motorSpeed= "); Serial.println (motorSpeed);
      
      analogWrite (motorA1A, 0);
      analogWrite (motorA1B, motorSpeed); 
      analogWrite (motorB1A, 0);
      analogWrite (motorB1B, motorSpeed); 
      
  } else

  //BACKWARD
  
  if ((z_Speed) > 0.3 && (y_Steer < 0.3 && y_Steer > -0.3)) {
      
      motorSpeed = ((z_Speed) * 1023);
      
      Serial.println (" BACKWARD   "); Serial.print ("  motorSpeed= "); Serial.println (motorSpeed);
      
      analogWrite (motorA1A, motorSpeed); //Turning Side slower than opposing side
      analogWrite (motorA1B, 0); //Turning Side s0er than opposing side
      analogWrite (motorB1A, motorSpeed);
      analogWrite (motorB1B, 0); 
     
  } else

  //FORWARD DIAGONAL RIGHT
  
  if ((z_Speed) < -0.3 && (y_Steer) < -0.3) {

      
      motorSpeed = (z_Speed * -1023);
      motorSpeedADJ = (motorSpeed * (1 + y_Steer)); 
      
      Serial.println (" Forward  diagonal right  ");Serial.print ("  motorSpeed= "); Serial.print (motorSpeed);
      Serial.print ("  motorSpeedADJ= "); Serial.println (motorSpeedADJ);
      
      analogWrite (motorA1A, 0);
      analogWrite(motorA1B, motorSpeed); 
      analogWrite (motorB1A, 0);
      analogWrite(motorB1B, motorSpeedADJ);   
         
  } else

  //BACKWARD DIAGONAL RIGHT
  
  if ((z_Speed) > 0.3 && (y_Steer < 0.3)) {
      
      motorSpeed = ((z_Speed) * 1023);
      motorSpeedADJ = (motorSpeed * (1 + y_Steer));
      
      Serial.println (" BACKWARD DIAGONAL RIGHT  ");Serial.print ("  motorSpeed= "); Serial.print (motorSpeed);
      Serial.print ("  motorSpeedADJ= "); Serial.println (motorSpeedADJ);
      
      analogWrite (motorA1A, motorSpeedADJ); //Turning Side slower than opposing side
      analogWrite(motorA1B, 0); //Turning Side slower than opposing side
      analogWrite (motorB1A, motorSpeed);
      analogWrite(motorB1B, 0); 
            
  } else

  //BACKWARD DIAGONAL LEFT

  if ((z_Speed) > 0.3 && (y_Steer) > 0.3) {
      
      motorSpeed = ((z_Speed) * 1023);
      motorSpeedADJ = (motorSpeed * (1 - y_Steer));
      
      Serial.println (" BACKWARD DIAGONAL LEFT  ");Serial.print ("  motorSpeed= "); Serial.print (motorSpeed);
      Serial.print ("  motorSpeedADJ= "); Serial.println (motorSpeedADJ);
      
      analogWrite (motorA1A, motorSpeed); //Turning Side slower than opposing side
      analogWrite(motorA1B, 0); //Turning Side slower than opposing side
      analogWrite (motorB1A, motorSpeedADJ);
      analogWrite(motorB1B, 0); 
      
  } else
                                    
  //FORWARD DIAGONAL LEFT
  
  if ((z_Speed) < -0.3 && (y_Steer) > 0.3) {
      
      motorSpeed = (z_Speed * -1023);//z_Speed is negative here - needs to be +
      motorSpeedADJ = (motorSpeed * (1 - y_Steer)); //z_Speed is negative here, subtracting from 1
      
      Serial.println (" FORWARD DIAGONAL LEFT  ");Serial.print ("  motorSpeed= "); Serial.print (motorSpeed);
      Serial.print ("  motorSpeedADJ= "); Serial.println (motorSpeedADJ);
      
      analogWrite(motorA1A, 0);
      analogWrite (motorA1B, motorSpeedADJ); 
      analogWrite(motorB1A, 0);
      analogWrite (motorB1B, motorSpeed); 
 
     }  
   // }
  }
}

//**************** Accelerometer - Accelerometer Control Mode****************

//*************Read RUN/STOP Switch - Accelerometer Control Mode***************
  BLYNK_WRITE (V4) {    
    accel_Ready = param.asInt();
/*
    if (accel_Ready == 0) {
      analogWrite(motorA1A, 0); 
      analogWrite(motorA1B, 0); 
      analogWrite(motorB1A, 0);
      analogWrite(motorB1B, 0);  
    }
    */
  }

//*************Read RUN/STOP Switch - Accelerometer Control Mode***************
