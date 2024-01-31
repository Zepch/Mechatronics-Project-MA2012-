#include <Arduino.h> 
#include <SoftwareSerial.h> 
#include <string.h> 
 
using namespace std; 
 
//Master and Slave Pin 
#define RxD 8 
#define TxD 9 
SoftwareSerial master(RxD, TxD); 
 
// Pins for 74C922 keypad encoder 
int KB_DataAvailable=A5;//Data ready pin from Keyboard
int KB_A=10; //Output A
int KB_B=11; //Output B
int KB_C=12; //Output C
int KB_D=13; //Output D
 
int stage = 1; 
 
//LED 
int red_led = 2; 
int green_led = 5; 
 
//buzzer 
int buzz = 7; 
 
//limit switch 
int limit_switch = 4; 
 
//solenoid 
int solenoid = 6; 
 
//How many wrong password possible 
int hp = 5; 
 
//Elevator 
int elevator = 0; 
 
int index = 0; 
 
//Button state 
int buttonstate; 
 
char keys[] = {'1','2','3','F',
               '4','5','6','E',
               '7','8','9','D',
               'A','0','B','C'};
// mario death sound

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44 
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
void marioDeathSound() {
  int melody[] = {NOTE_G4, NOTE_F5, NOTE_F5, NOTE_F5, 
  NOTE_E5, NOTE_D5, NOTE_C5, 
  NOTE_G4, 0};
  int noteDurations[] = {4,2,4,4,4,4,5/3,4, 0};

  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(melody[0])); thisNote++) {  
    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(7, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(7);
    Serial.println(melody[thisNote]);
  }
}
// indiana jones theme song
long DO=523.25,
RE=587.33,
MI=659.26,
FA=698.46,
SOL=783.99,
LA=880,
SI=987.77,
DO2=1046.50,
RE2=1174.66,
MI2=1318.51,
FA2=1396.91;
long melody2[] = {MI,FA,SOL,DO2, 
RE,MI,FA,
SOL,LA,SI,FA2,
LA,SI,DO2,RE2,MI2,
MI,FA,SOL,DO2,
RE2,MI2,FA2,
SOL,SOL,MI2,RE2,SOL,
MI2,RE2,SOL,
MI2,RE2,SOL,
FA2,MI2,RE2,DO,
MI,SOL,FA,
RE,FA,MI,SOL,
DO,DO,MI,SOL,FA,
RE,FA,MI,RE,
DO,DO,MI,SOL,FA,
RE,FA,MI,SOL,
DO,DO,SOL,SOL,
MI2,RE2,SOL,
MI2,RE2,SOL,
MI2,RE2,SOL,
FA2,MI2,RE2,DO,
};

int b=800, n=400, c=200, s=100;
int noteDurations2[] = {c,c,n,b,
c,c,b,
c,c,n,b,
c,c,n,n,
n,c,c,n,
b,c,c,b,
c,c,n,
c,c,n,
c,c,n,
c,c,n,
c,c,b,
c,c,b,
c,c,s,s,c,
b,c,c,b,
c,c,s,s,c,b,
c,c,b,
c,c,s,s,c,
b,c,c,n,
c,c,n,
c,c,n,
c,c,n,
c,c,b,
};

void playsound(){
  for (int thisNote = 0; thisNote < 80; thisNote++) { 
    tone(7, melody2[thisNote],noteDurations2[thisNote]);
    delay(noteDurations2[thisNote]);
    noTone(7);
  }
}
//node for buzzer 
#define NOTE_C5  523 
#define NOTE_E5  659 
#define NOTE_G5  784 
#define NOTE_G4  392 
#define NOTE_DS2 78 
#define NOTE_E2  82 
#define NOTE_F2  87 
#define NOTE_FS2 93 
#define NOTE_G2  98 
#define NOTE_A2  110 
#define NOTE_AS2 117 
#define NOTE_B2  123 
#define NOTE_C3  131 
#define NOTE_D3  147 
 
//NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_G4 (Mario theme song) 
//8, 8, 4, 4, 4, 8, 4 
int melody[] = { 
  NOTE_C5, NOTE_G2, NOTE_E5, NOTE_G4,  
  NOTE_DS2, NOTE_E2, NOTE_E2, NOTE_F2, 
  NOTE_FS2, NOTE_G5, NOTE_E5, NOTE_A2, 
  NOTE_AS2, NOTE_B2, NOTE_C3, NOTE_D3 
}; 
 
int noteDurations[] = { 
  4, 4, 8, 4, 
  4, 4, 4, 4, 
  4, 8, 4, 4, 
  4, 4, 4, 4, 
}; 
const char correctpin[] = {'3','3','9','1','9','8','F'}; // based on super mario 
 
void sound(); 
 
void setup() { 
  Serial.begin(9600); 
  pinMode(KB_DataAvailable,INPUT); 
  pinMode(KB_A,INPUT); 
  pinMode(KB_B,INPUT); 
  pinMode(KB_C,INPUT); 
  pinMode(KB_D,INPUT); 
  pinMode(red_led, OUTPUT); 
  pinMode(green_led, OUTPUT); 
  pinMode(limit_switch, INPUT); 
  pinMode(solenoid, OUTPUT);
  master.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  //initialize
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, HIGH);
  digitalWrite(solenoid, LOW);
} 
 
void loop() { 
  // first stage the keypad game 
  while (stage == 1) { 
    if (digitalRead(KB_DataAvailable)) { 
      sound();
      Serial.println(readKeypad()); 
      delay(300);

       
      if (readKeypad() == correctpin[index] && index == 6) { 
        digitalWrite(green_led, LOW); 
        stage++; 
        break; 
      } else if (readKeypad() == correctpin[index] && index != 6) { 
        index++; 
      } 
      else{ 
        hp--; 
        if (hp == 0){ 
          digitalWrite(red_led, LOW); 
          marioDeathSound();
          stage++; 
          break; 
        } 
        blink();
        }
      } 
    }  
 
  // second stage after green led turns on 
  buttonstate = digitalRead(limit_switch);
  // Serial.println(elevator);
   
  if (buttonstate == 1){ // if limit switch get diactivated 
    elevator = 1; // elevator get activated
    delay(1000);
    digitalWrite(solenoid, HIGH); // solenoid get activated 
    delay(3000);
    master.write(elevator);
  }
  else{
    digitalWrite(solenoid, LOW);
  }
  if (master.read() != -1){
    playsound();
  }
   
} 
 
char readKeypad() {
  int ka,kb,kc,kd,k;
  ka=digitalRead(KB_A); //read encoder output A
  kb=digitalRead(KB_B); //read encoder output B
  kc=digitalRead(KB_C); //read encoder output C
  kd=digitalRead(KB_D); //read encoder output D
  k=ka+kb*2+kc*4+kd*8; // combine the encoder outputs 
  return keys[k];
}
 
void sound(){ 
  int ka,kb,kc,kd; 
  int node; 
 
  ka=digitalRead(KB_A); //read encoder output A 
  kb=digitalRead(KB_B); //read encoder output B 
  kc=digitalRead(KB_C); //read encoder output C 
  kd=digitalRead(KB_D); //read encoder output D 
 
  node=melody[ka+kb*2+kc*4+kd*8]; // combine the encoder outputs 
  int timer = 1000 / noteDurations[ka+kb*2+kc*4+kd*8]; 
 
  tone(7,node,timer); 
}

void blink(){
  unsigned long previousMillis = 0;
  unsigned long interval = hp * 100;
  boolean blinking = false;
  boolean keyPressed = false;

  while (hp != 0 && !keyPressed) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) { // it only execute the code if this corrects, so we don't need to use delay
      previousMillis = currentMillis;
      if (blinking) {
        digitalWrite(red_led, LOW);  // Turn on the LED
      } else {
        digitalWrite(red_led, HIGH);  // Turn off the LED
      }
      blinking = !blinking; // switch the value
    }

    if (digitalRead(KB_DataAvailable)) {
      // User pressed a key, exit the loop
      keyPressed = true;
    }
  }
  digitalWrite(red_led, HIGH);  // Turn off the LED when exiting the loop
}


