#include <Arduino.h>


// put function declarations here:
//initiate LEDs and buttons
int LED_R=17; 
int LED_P=16;
int LED_Y=15;
int LED_O=14;
int BUTTON1=10;
int BUTTON2=13;
int BUTTON3=12;
int BUTTON4=11;
unsigned long startTime=0;

int LoadPhase=0;
int Phase1Complete=0;
int InsertMaterialPhase=0;
int Phase2Complete=0;
int DetonationPhase=0;
int Phase3Complete=0;
int LoadDevicePhase=0;
int Phase4Complete=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("testing");
  pinMode(LED_R, OUTPUT);
  pinMode(LED_P, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_O, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BUTTON1,INPUT_PULLUP);
  pinMode(BUTTON2,INPUT_PULLUP);
  pinMode(BUTTON3,INPUT_PULLUP);
  pinMode(BUTTON4,INPUT_PULLUP);
}

void loop() {
  static int prev1 = HIGH, prev2 = HIGH, prev3 = HIGH, prev4 = HIGH;

  int buttonState1 = digitalRead(BUTTON1);
  int buttonState2 = digitalRead(BUTTON2);
  int buttonState3 = digitalRead(BUTTON3);
  int buttonState4 = digitalRead(BUTTON4);

  if (prev1 == HIGH && buttonState1 == LOW && !Phase1Complete) {
    while (digitalRead(BUTTON1) == LOW) { delay(5); }  // wait release
    delay(30);                                         // debounce

    Serial.println("loading");
    delay(100);

    startTime = millis();
    int  prevInWindow = HIGH;   // edge tracker
    bool canceled      = false;

    while (millis() - startTime < 3000) {
      digitalWrite(LED_R, HIGH);

      int cur = digitalRead(BUTTON1);                  // use cur only here
      if (prevInWindow == HIGH && cur == LOW) {        // NEW press -> cancel
        canceled = true;
        break;
      }
      prevInWindow = cur;                              // update tracker
      delay(10);
    }

    digitalWrite(LED_R, LOW);

    if (canceled) {
      Serial.println("Canceled");
      Phase1Complete = 0;
      while (digitalRead(BUTTON1) == LOW) { delay(5); } // wait release
      delay(30);
    } else {
      Phase1Complete = 1;
      Serial.println("Loaded");
    }
    delay(20);
  }

  //Phase 2 (material insertion)
  if (prev2==HIGH  && Phase1Complete == 1 && buttonState2== LOW && Phase2Complete==0) {
    digitalWrite(LED_P,HIGH);
    delay(20);
    Serial.println("Material Inserted");
    Phase2Complete=1;
  }

  //Phase 3 (Detonation)
  if (prev3==HIGH && Phase2Complete == 1 && buttonState3== LOW && Phase3Complete==0) {
    
    Serial.println("Countdown Started");
    startTime=millis();

    while (millis() - startTime < 5000) {
      digitalWrite(LED_Y, HIGH);
      delay(500);
      digitalWrite(LED_Y,LOW);
      delay(500);
    }

    digitalWrite(LED_P,LOW);
    digitalWrite(LED_Y,LOW);
    digitalWrite(LED_O,HIGH);

    Serial.println("Detonation Complete, Device Ready for Loading");
    Phase3Complete=1;
  }

  //Phase 4 (load device)
  if (prev4==HIGH && Phase3Complete==1 && buttonState4==LOW) {
    Serial.println("device loaded");
    delay(10);
    Serial.println("have a nice day");
    digitalWrite(LED_O,LOW);
    Phase3Complete=0;
    Phase2Complete=0;
    Phase1Complete=0;
  }

  // update previous states at end of loop
  prev1 = buttonState1;
  prev2 = buttonState2;
  prev3 = buttonState3;
  prev4 = buttonState4;



}