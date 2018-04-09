/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>

 ***************************************************
 This example shows the basic function of library for DFPlayer.

Modified from the Angelo qiao DFPlayer example by SB8887 in 2018
[By []Angelo qiao](Angelo.qiao@dfrobot.com)

 Modified example to play 1st pl3 at boot and then be activated by a PPM
 signal to allow it to be controlled via an RC car remote.

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/



#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

volatile int pwm_value = 0;
volatile int prev_time = 0;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX pins
DFRobotDFPlayerMini myDFPlayer;


//int button3 =2;
int button2 =3;

int trigger1 = LOW;


int lastButtonState = LOW;
int lastTriggerstate= LOW;


//lastButtonState = 0;
//lastButtonState1 =0;


void printDetail(uint8_t type, int value);



void setup()
{
    //attach interupt at int0 which is D2 on the nano
    attachInterrupt(0, rising, RISING);

    //input for button
    pinMode(button2, INPUT);


    lastButtonState = 0;


  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  myDFPlayer.enableDAC();
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30

    Serial.print ("State  ") ; Serial.println(myDFPlayer.readState()); //read mp3 state
    Serial.print ("Volume "); Serial.println(myDFPlayer.readVolume()); //read current volume
    Serial.println(myDFPlayer.readEQ()); //read EQ setting
    Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    Serial.println(myDFPlayer.readFileCountsInFolder(1)); //read fill counts in folder SD:/03


myDFPlayer.playMp3Folder(1); //play track 1 in MP3 folder
delay(200); // need to delay to ensure it starts paused.
lastTriggerstate = 0;
trigger1 =0;
myDFPlayer.pause();  // pause the player.


}

void loop()
{


    int buttonRead2 = digitalRead(button2);
    //int buttonRead3 = digitalRead(button3);

    //if PWM is over this set the trigger state
if (pwm_value >= 1300) {
    trigger1 =1;
}

    if (pwm_value <= 1299){
        trigger1 = 0;
                            }

//detect change in the state of the trigger int , if it has changed, so something
if (trigger1 != lastTriggerstate)
    {
        if (trigger1 <=0)
            {myDFPlayer.pause();
                Serial.println("Play Paused");
                Serial.println(myDFPlayer.readState());
            lastTriggerstate = 0;}

                if (trigger1 >=1)
                    {myDFPlayer.start();
                        Serial.println ( "play started ");
                    lastTriggerstate = 1;}


    }

//if you push the button this happens....
    if (buttonRead2 != lastButtonState && buttonRead2 >=1)
    {
      if(buttonRead2 >= 1){
          myDFPlayer.next();
          Serial.println("Next Track");
          lastButtonState=0;
          delay(200); // shitty debounce
        }

}

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
    //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      delay(50);
      myDFPlayer.next();
      Serial.println(F(" Play Playing next"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}



//reads PWM frtom the intput

void rising() {
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}

void falling() {
  attachInterrupt(0, rising, RISING);
  pwm_value = micros()-prev_time;
  //Serial.println(pwm_value); // for testing only, if you leave this active it fails to work properly
}
