/*
  Countdown
  
  Counts down to zero from an adjustable value. 
  Uses a 3 digits 7 segments display to show the remaining seconds. 
  Plays a sound when the time is over. 
    
  created 24 Aug 2013 
  by Steffen Retzlaff
   
  Public Domain - available on GitHub
  https://github.com/pele1/Arduino/tree/master/countdown
  
  Breadboard drawing:
  https://github.com/pele1/Arduino/blob/master/countdown/arduino_countdown_breadboard.png
    
  Uses Arduino example code:
  - Melody by Tom Igoe 
  - Blink without Delay by David A. Mellis and Paul Stoffregen

 */

#include "pitches.h"


const int SEG_ON = LOW;
const int SEG_OFF = HIGH;

const int DIG_ON = HIGH;
const int DIG_OFF = LOW;

const int SEG_A = 8;
const int SEG_B = 9;
const int SEG_C = 2;
const int SEG_D = 3;
const int SEG_E = 4;
const int SEG_F = 5;
const int SEG_G = 6;
const int SEG_DP = 7;

const int BUTTON_PIN = 13;

int segments[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const int maxSegIndex = 7;

const int DIG_1 = 10;
const int DIG_10 = 11;
const int DIG_100 = 12;

unsigned char digit1Segs =   0b0000000;
unsigned char digit10Segs =  0b0000000;
unsigned char digit100Segs = 0b0000000;

unsigned char testPattern =  0b1000000;

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long currentMillis = 0;
long previousMillis = -10000;        // will store last time LED was updated
long countDownMillis = 0; 
const long DELAY_MS = 500;           // interval to count

int currentCountDownSecs = 0;
boolean countDownReached = false;


unsigned char getDigitSeg(char digitValue) {
   switch (digitValue) {
  case 0:  
    return 0b11111100;
  case 1: 
    return 0b01100000;  
    break;
  case 2:   
    return 0b11011010; 
    break;
  case 3:   
    return 0b11110010; 
    break;
  case 4:   
    return 0b01100110; 
    break;
  case 5:   
    return 0b10110110; 
    break;
  case 6:   
    return 0b10111110; 
    break;
  case 7:   
    return 0b11100000; 
    break;
  case 8:   
    return 0b11111110; 
    break;
  case 9:   
    return 0b11110110; 
    break;   
  case '-':   
    return 0b00000010; 
    break;   
  default: 
    return 0b00000000;   
  }

}

void setDisplayDigit(unsigned char digitValue, char digitPin) {
  char digitIndex = digitPin - DIG_1; // conversion to arrayVariable;
  
  switch (digitPin) {
  case DIG_1:  
    digit1Segs   = getDigitSeg(digitValue);
    break;
  case DIG_10: 
    digit10Segs  = getDigitSeg(digitValue);
    break;
  case DIG_100: 
    digit100Segs = getDigitSeg(digitValue);
    break;  
  }
}

void setDisplayNumber(int displayNumber) {
  if(displayNumber > 999) {
     setDisplayDigit(-1, DIG_100);
     setDisplayDigit(-1, DIG_10);
     setDisplayDigit(-1, DIG_1);    
  } 
  else if (displayNumber > 99) {
    setDisplayDigit(displayNumber%1000/100, DIG_100);
    setDisplayDigit(displayNumber%100/10,   DIG_10);
    setDisplayDigit(displayNumber%10,       DIG_1);      
  }
  else if (displayNumber > 9) {
    setDisplayDigit(-1, DIG_100);
    setDisplayDigit(displayNumber%100/10, DIG_10);
    setDisplayDigit(displayNumber%10,     DIG_1);      
  }  
  else if (displayNumber >= 0) {
    setDisplayDigit(-1, DIG_100);
    setDisplayDigit(-1, DIG_10);
    setDisplayDigit(displayNumber%10, DIG_1);      
  }  
  else if (displayNumber > -10) {
    setDisplayDigit(-1, DIG_100);
    setDisplayDigit('-', DIG_10);
    setDisplayDigit(abs(displayNumber%10), DIG_1);      
  }  
  else if (displayNumber > -100) {
    setDisplayDigit('-', DIG_100);
    setDisplayDigit(abs(displayNumber%100/10), DIG_10);
    setDisplayDigit(abs(displayNumber%10), DIG_1);      
  }  
  else {
    setDisplayDigit(-1, DIG_100);
    setDisplayDigit(-1, DIG_10);
    setDisplayDigit(-1, DIG_1);        
  }  


}


void playMelody() {

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(1, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(1);
 
  }

  
}


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pins as an output.
  for(int ledPin = 0; ledPin <= 12; ledPin++) {
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, HIGH);      
  }
  pinMode(BUTTON_PIN, INPUT);
  
  // initialize digits
  digitalWrite(DIG_1, DIG_OFF);  
  digitalWrite(DIG_10, DIG_OFF);  
  digitalWrite(DIG_100, DIG_OFF); 
  
  digitalWrite(1, LOW); 

  // Serial debug
  //Serial.begin(9200);
  
}



// the loop routine runs over and over again forever:
void loop() {

  currentMillis = millis();
  if(currentMillis - previousMillis > DELAY_MS) {
  
     if(digitalRead(BUTTON_PIN) == LOW || previousMillis < 0) {
         currentCountDownSecs = analogRead(A0); 
         if (currentCountDownSecs > 999) currentCountDownSecs = 999;
         countDownReached = false;
     }
     else if(currentCountDownSecs <= 0) {
         if(!countDownReached) {
           countDownReached = true;
           digitalWrite(SEG_A, SEG_OFF);  
           digitalWrite(SEG_B, SEG_OFF);
           digitalWrite(SEG_C, SEG_OFF);
           digitalWrite(SEG_D, SEG_OFF);
           digitalWrite(SEG_E, SEG_OFF);
           digitalWrite(SEG_F, SEG_OFF);
           digitalWrite(SEG_G, SEG_ON);
         }
         if(digitalRead(DIG_1) == DIG_ON) {
           digitalWrite(DIG_1,  DIG_OFF); 
           digitalWrite(DIG_10, DIG_ON); 
           digitalWrite(DIG_100, DIG_OFF);
         } 
         else if(digitalRead(DIG_10) == DIG_ON) {
           digitalWrite(DIG_1, DIG_OFF); 
           digitalWrite(DIG_10, DIG_OFF); 
           digitalWrite(DIG_100, DIG_ON);
         }
         else {
           digitalWrite(DIG_1,   DIG_ON); 
           digitalWrite(DIG_10,  DIG_OFF);
           digitalWrite(DIG_100, DIG_OFF);
         }
         playMelody();
         if(digitalRead(BUTTON_PIN) != LOW) delay(2000);
                       
     }
     else {
         countDownMillis = countDownMillis + currentMillis - previousMillis;
         if(countDownMillis > 1000) {
           currentCountDownSecs--;
           countDownMillis = countDownMillis - 1000;
         }
     }
     
     setDisplayNumber(currentCountDownSecs); 
     previousMillis = currentMillis;   

  }  
  
  if(!countDownReached) {
    // light 7seg display
    testPattern = 0b10000000;
    digitalWrite(segments[0],    SEG_OFF); 
    for(int segIndex = 0; segIndex <= maxSegIndex; segIndex++) {
      
      if(segIndex != 0)            digitalWrite(segments[segIndex-1], SEG_OFF); 
      
      digitalWrite(DIG_100, DIG_OFF); 
      digitalWrite(DIG_1,   DIG_ON); 
      if(digit1Segs & testPattern) digitalWrite(segments[segIndex], SEG_ON);  
      
      digitalWrite(segments[segIndex], SEG_OFF);
      digitalWrite(DIG_1, DIG_OFF); 
      digitalWrite(DIG_10,   DIG_ON); 
      if(digit10Segs & testPattern) digitalWrite(segments[segIndex], SEG_ON);  
      
      digitalWrite(segments[segIndex], SEG_OFF);
      digitalWrite(DIG_10, DIG_OFF); 
      digitalWrite(DIG_100,   DIG_ON); 
      if(digit100Segs & testPattern) digitalWrite(segments[segIndex], SEG_ON);  
  
      testPattern = testPattern >> 1;
    }
    
    
  } 
  
 
    
    
  
   
  

}
