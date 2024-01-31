#include <Arduino.h> 
#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

#include "BasicStepperDriver.h"
#include <Servo.h>
#include <SoftwareSerial.h>
#define RxD A14
#define TxD A13
SoftwareSerial slave(RxD, TxD); 


// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step 
#define MOTOR_STEPS 200   
#define RPM 60  
 
// Since microstepping is set externally, make sure this matches the selected mode 
// If it doesn't, the motor will move at a different RPM than chosen 
// 1=full step, 2=half step etc. 
#define MICROSTEPS 16
 
// All the wires needed for full functionality 
#define DIR 55
#define STEP 54
//Uncomment line to use enable/disable functionality 
#define SLEEP 38
#define onemm 80

int floor_number = 1; // where the elevator
int homeSwitch = 18; 
int flag = 0; 
// 2-wire basic config, microstepping is hardwired on the driver 
//Uncomment line to use enable/disable functionality   
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, SLEEP);   

//Uncomment line to use enable/disable functionality 
//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, SLEEP); 

#define greenLed 16
#define redLed 17

#define toggle_switch_pin 25

#define motorA1_in 4   // motor pin1
#define motorA2_in 5  // motor pin2
#define encoderPinA 2
#define encoderPinB 3
volatile long int encoder_pos = 0; 
int motor_speed;
int error = 10; // permissible error
int setPos = 0;   // Max value is 65535
char incomingByte;

// Variable resistor, password checking
#define adc_0 A11
#define adc_ref A12

int password = 330; // Var Res to check against
float R1 = 330.0;
float adc_0_correct = password/(password + R1);
int adc_error = 30;
float error_voltage_16bit = 0.07;
float check;
float voltage;

bool elevator_up = false;

void setup() {
  Serial.begin(9600);
  pinMode(RxD, INPUT);                               //set mode of receive pin (from bluetooth)
  pinMode(TxD, OUTPUT);                              //set mode of transmit pin (to bluetooth)
  slave.begin(9600);
  stepper.begin(RPM, MICROSTEPS); 
  // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line 
  // stepper.setEnableActiveState(LOW); 

  // LED Pins
  pinMode(greenLed, OUTPUT); // Green LED, Low = Turn on
  pinMode(redLed, OUTPUT); // Red LED, Low = Turn on

  // Toggle switch
  pinMode(toggle_switch_pin, INPUT_PULLUP);

  //DC Encoder
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(motorA1_in, OUTPUT);
  pinMode(motorA2_in, OUTPUT);

  //Arduino adc
  pinMode(adc_0, INPUT);


  stepper.begin(RPM, MICROSTEPS);   
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line   
  stepper.setEnableActiveState(LOW);   
  //homeing();
  //Wire.begin();
  //if(!adc.init())
    //{
      //Serial.println("ADS1115 not connected!");
    //}
  //adc.setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range
  //adc.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channel
  //adc.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode
}

bool check_password(){
  check = adc_0_correct * analogRead(adc_ref) + 6;
  Serial.print("check: ");
  Serial.print(check);
  Serial.print(", A0 reading: ");
  Serial.println(analogRead(adc_0));
  if (analogRead(adc_0) < check + adc_error && analogRead(adc_0) > check - adc_error)
  {
    return true;
  }
  return false;
}

// To be modified for exact length and speed after assembly is almost done
void elevator(int move_cm) {   
    stepper.enable();  
    stepper.move(onemm * move_cm); 
    stepper.disable();
    delay(1000);  
}

void homeing(){    
  stepper.enable();     
  while(flag == 0){      
    int sw = digitalRead(homeSwitch);  
    stepper.move(-onemm * 1);      
    if (sw == LOW){
      stepper.move(onemm * 20);        
      stepper.disable();
      flag = 1;        
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);        
      break;
      }    
  }
  delay(3000);
}

bool check_password_16bit(){
  delay(1000);
  check = adc_0_correct * readChannel(ADS1115_COMP_0_GND);
  voltage = readChannel(ADS1115_COMP_3_GND);
  Serial.print("check: ");
  Serial.print(check);
  Serial.print(", voltage: ");
  Serial.println(voltage);
  
  if (voltage > check - error_voltage_16bit && voltage < check + error_voltage_16bit)
  {
    Serial.println("Correct!");
    return true;
  }
  Serial.println("Wrong!!");
  return false;
}

void motorBlade(){
  analogWrite(motorA1_in, 1023); // Spin CW 90% duty cycle
  digitalWrite(motorA2_in, LOW);
}

float readChannel(ADS1115_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);
  voltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
  return voltage;
}


void loop() {
  homeing();
  Serial.print("Floor: ");
  Serial.println(floor_number);
  if (digitalRead(toggle_switch_pin) == LOW){
    // reset LEDs and motorBlade
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    analogWrite(motorA1_in, 0);
    digitalWrite(motorA2_in, LOW);
  }

  if (floor_number == 1 && slave.read() != -1)
  { //if data is available and at first floor
    Serial.println(slave.read());
    Serial.println("elevator up");
    elevator(160);
    floor_number ++;
    //delay(3000);
  }
  else
  { // elevator is not going up
    //delay(3000);
    if (digitalRead(toggle_switch_pin) == HIGH && floor_number == 2)
    {
      bool check_pass = false;
      check_pass = check_password();
      //check_pass = check_password_16bit();
      if (check_pass == true) // password correct motorblade not started
      {
        analogWrite(motorA1_in, 0); // Stops
        digitalWrite(motorA2_in, LOW);
        digitalWrite(greenLed, LOW); // active low
        digitalWrite(redLed, HIGH);
        delay(1000);
        elevator(150);
        Serial.println("elevator up");
        floor_number ++;
        //delay(3000);
        //slave.write("1");
        //Serial.println("Sending 1");
      }
      else // password wrong
      {
        digitalWrite(redLed, LOW); // active low
        digitalWrite(greenLed, HIGH);
        motorBlade();
        Serial.println("motorblade");
        delay(1000);
      }
    }
  }
}
