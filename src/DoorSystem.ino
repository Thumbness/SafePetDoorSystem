/*
 * Project DoorSystem
 * Description:
 * Author:
 * Date:
 */

#include "MFRC522.h"

#define SWITCH_PIN D4
#define GREEN_LED D7
#define YELLOW_LED D8

#define SS_PIN A5
#define RST_PIN A2

int switchState;
bool inside;

unsigned long uid;
unsigned long feederCard;

MFRC522 mfrc522(SS_PIN, RST_PIN);

void getCardID(const char *event, const char *data) {
  Serial.println("Card Read from other device!");
  std::string temp = (std::string)data;
  feederCard = stoul(temp, 0, 10);
  Serial.println(feederCard);
}



// setup() runs once, when the device is first turned on.
void setup() {
  pinMode(SWITCH_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

 
  mfrc522.setSPIConfig();
  mfrc522.PCD_Init(); 

  Particle.subscribe("Card_ID", getCardID);
  
}



// loop() runs over and over again, as quickly as it can execute.
void loop() {
  switchState = digitalRead(SWITCH_PIN);
  if(switchState == HIGH)
  {
    digitalWrite(GREEN_LED, HIGH);
    Particle.publish("Press-A-Button", "inside");
    Serial.println(uid);
    Serial.println(feederCard);
    delay(1000);
  }

  if(switchState == LOW)
  {
    digitalWrite(GREEN_LED, LOW);
  }
   
  CardRead();
  
}


void CardRead()
{ 

  if(mfrc522.PICC_IsNewCardPresent()) {
      unsigned long uid = getID();
      Serial.print("Card detected, UID: "); Serial.println(uid);
      sendToFeeder();
      if(uid != -1)
      {
        return;
      }
    }
    
}
//get RFID tag ID
unsigned long getID(){
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long int hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

void sendToFeeder()
{
  if(uid == feederCard)
  {
    Particle.publish("Pet_Status" "Movement");
  }
}

