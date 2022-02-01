/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>
 
 ***************************************************
 This example shows the basic function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

Updated by Chris Welder 2022-1-30 for D-O V2 MBedadley Build
Designed for Nano and FlySky 10 Channel Controller

Must alos include the Robot Control Libary

From Left to Right looking at the Flysky Controller
Switch 1 (2 positions up and Down) = SW1 -- Plays I am DO
Swtich 2 (2 Positions up and Down) = SW2 -- Plays Friends Ahead 
Switch 3 (3 positions up, middle, Down) = SW3 -- Plays Randon one of the "Hello" on middle and then random one of the No Thankyou at Down
Switch 4 (2 positions up and Down) = SW4 -- Plays Random Sad, SoSorrySheIsGone, SqueekEliminated, SqueekyWheel1, WhatIsThat
Added a Feature for Volume control. Put SW3 in middle position then sw1 down.  All others in up Position. Then sw3 Up From Middle will increase Vol by 5
   and down from Middle will Decrease Vol by 5. When done adjusting put Sw3 in middle position and SW1 back to up. Then SW1 back to up.

 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Nano
 ****************************************************/

/*
 * Sounds are as follows, must be loaded on the card in this order. 
 
Battery Charged = 1
Friends Ahead = 2
Hello1 = 3
Hello2 = 4
Hello3 = 5
IAMDO = 6
IMissHer = 7 
NoThankYou1 = 8
NoThankYou2 = 9
Sad= 10
SoSorrySheIsGone = 11
SqueekEliminated = 12 
SqueekyWheel1 = 13
WhatIsThat = 14
VolUp = 15 -- Note I created my own Vol up file. You can use mine or create your own
VolDown = 16 -- Note I created my own Vol Down File. You can use mine or create your own.
*/

/*Volume values are 5, 10, 15, 20, 25, 30
vol is an integrer from 1 to 6, 5*Vol = output Vol
Max vol is 30
Min Vol is 5 
 */
/* On the FlySky, you need to Assign the Switches to the following:
 Channel 7 = SWA = Left most Switch or SW1 in this code
 Channel 8 = SWB = Second from left or SW2 in this code
 Channel 9 = SWC = 3 position Switch 3rd from left or SW3 in this code
 Channel 10 = SWD = right most Switch or SW4 in this code 
 */

#include "Arduino.h"
//#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//SoftwareSerial mySoftwareSerial (7, 8); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//Variables for reading the RC switches
int readsw1; 
int readsw2; 
int readsw3; 
int readsw4; 

/* Assign the Pins so Pin 1 = SW1 and Pin 4 = SW4. if your cable is not straight through from Channel 7 to Pin 12, you can re-assign here */
int pin1 = 12; /* SW1 */
int pin2 = 11; /* SW2 */
int pin3 = 10; /* SW3 */
int pin4 = 9;  /* SW4 */

int currentsound = 1; /* Battery Charged */
int soundstate =1;
int vol = 3;  /* Vol 3 = 15, 1 = 5, 2, =10, 4 = 20, 5 = 25, 6 = 30 */
int volstate = 1;
int prevsoundstate = 1;


void setup()
{
 
  Serial.begin(9600);
  
  if (!myDFPlayer.begin(Serial)) {  //Use softwareSerial to communicate with mp3.
    //Serial.println(F("Unable to begin:"));
    //Serial.println(F("1.Please recheck the connection!"));
    //Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  //Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(vol*5);  
  delay (2000);
  myDFPlayer.play(1);  
  /*  use these to test your sound files match
  delay (3000);
  myDFPlayer.play(3);
  delay (2000);
  myDFPlayer.play(4);
  delay (2000);
  myDFPlayer.play(5);
  delay (2000);
  myDFPlayer.play(6);
  delay (2000);
  myDFPlayer.play(7);
  delay (2000);
  myDFPlayer.play(8);
  delay (2000);  
  myDFPlayer.play(9);
  delay (2000);
  myDFPlayer.play(10);
  delay (2000);  
  myDFPlayer.play(11);
  delay (2000);  
  myDFPlayer.play(12);
  delay (2000);  
  myDFPlayer.play(13);
  delay (2000);  
  myDFPlayer.play(14);
  delay (2000);
  myDFPlayer.play(15);
  delay (2000);
  myDFPlayer.play(16);
  delay (2000); */      
}

void loop()
{
  static unsigned long timer = millis();

  readsw1 = pulseIn (pin1, HIGH);
  readsw2 = pulseIn (pin2, HIGH);
  readsw3 = pulseIn (pin3,HIGH);
  readsw4 = pulseIn (pin4,HIGH);

/* Values you should see from the PIN ins. If your Values are different you may need to correct the code < or > statements below
// Pin                  12    11     10   9
// Switch               sw1   sw2   sw3   sw4
// all up               995   995   995   995
// sw1 dwn              1975   995   995   995
// sw2 dwn              995   1975   995   995
// sw3 mid              995   995   1490   995
// SW3 dwn              995   995   1975   995
// sw1 dwn sw3 mid      1975  995   1490   995
// sw1 dwn sw3 up       1975  995   955   995
// sw1 dwn sw3 dwn      1975  995   1975  995
*/

  if ((readsw1 < 1100) and (readsw2 < 1100) and (readsw3 < 1100) and (readsw4 < 1100)) {
    soundstate=1;
    currentsound=1;
  }
  if ((volstate == 1) and (readsw1 > 1500) and (readsw2 < 1100) and (readsw3 < 1100) and (readsw4 < 1100)) {   
    soundstate=2;
    currentsound=6;
  }
  if ((readsw1 < 1100) and (readsw2 > 1500) and (readsw3 < 1100) and (readsw4 < 1100)) {  
    soundstate=3;
    currentsound = 2; 
  }
  if ((volstate == 1) and (readsw1 < 1100) and (readsw2 < 1100) and (readsw3 >1200) and (readsw3 < 1700) and (readsw4 < 1100)) {  
    soundstate=4;
    currentsound = random(3,6); 
  }
  if ((readsw1 < 1100) and (readsw2 < 1100) and (readsw3 > 1700) and (readsw4 < 1100)) {
    soundstate=5;
    currentsound = random(8,10); 
  }
  if ((readsw1 < 1100) and (readsw2 < 1100) and (readsw3 < 1100) and (readsw4 > 1500)) { 
    soundstate=6;
    currentsound = random(10,15); 
  }
  
  /* code for controlling Volume */
  
  if ((readsw1 > 1500) and (readsw2 < 1100) and (readsw3 > 1200) and (readsw3 < 1700) and (readsw4 < 1100)) {
    soundstate = 1;
    currentsound = 7;
    volstate = 2;
  }
  if ((volstate == 2) and (readsw1 > 1500) and (readsw2 < 1100) and (readsw3 > 1800) and (readsw4 < 1100)) { 
    soundstate=8;
    currentsound = 16;
    volstate = 1;
  }
  if ((volstate == 2) and (readsw1 > 1500) and (readsw2 < 1100) and (readsw3 < 1200) and (readsw4 < 1100)) { 
    soundstate=7;
    currentsound = 15;
    volstate = 1;
  }
   
  
  if (soundstate != prevsoundstate){
    prevsoundstate = soundstate;
    if (soundstate == 7) {
      vol = vol+1;
      myDFPlayer.volume(vol*5);
    }
    if (soundstate == 8) {
      vol = vol-1;
      myDFPlayer.volume(vol*5);
    }
    if (soundstate != 1) {
      myDFPlayer.play(currentsound);  
    }
  }



  //Use this code to troubleshout your Pin out. You need to turn on SerialMonitor
  //Flip the Switches up and Down, see the values change. This will help you confirm 
  //Which Switch is assigned to which readsw (you can fix this is the pin assignments above)
  // as well as confirm the values your are getting which will affect the IF Statements above
  /*Serial.print('V');
  Serial.print(':');
  Serial.print(vol);
  Serial.print(' ');
  Serial.print(readsw1);
  Serial.print(' ');
  Serial.print(readsw2);
  Serial.print(' ');
  Serial.print(readsw3);
  Serial.print(' ');
  Serial.print(readsw4);
  Serial.println(' '); 
*/

  
 // if (myDFPlayer.available()) {
 //   printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
 // }
}

 
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      //Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      //Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      //Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      //Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      //Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      //Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      //Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      //Serial.print(F("Number:"));
      //Serial.print(value);
      //Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      //Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          //Serial.println(F("Card not found"));
          break;
        case Sleeping:
         // Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
         //Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          //Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          //Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
         // Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          //Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
