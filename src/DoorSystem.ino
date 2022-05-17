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
int cntr;

unsigned long uid;
unsigned long feederCard;

MFRC522 mfrc522(SS_PIN, RST_PIN);

void getCardID(const char *event, const char *data)
{
  Serial.println("Card Read from other device!");
  std::string temp = (std::string)data;
  feederCard = stoul(temp, 0, 10);
  Serial.println(feederCard);
}

void CheckPetStatus()
{
  if (cntr % 2 == 1)
  {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    if (uid == feederCard)
    {
      Particle.publish("Pet_status", "inside");
    }
  }
  if (cntr % 2 == 0)
  {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    if (uid == feederCard)
    {
      Particle.publish("Pet_status", "outside");
    }
  }
}

void CheckforManualRecalibrate()
{
  switchState = digitalRead(SWITCH_PIN);
  if (switchState == HIGH)
  {
    cntr += 1;
    CheckPetStatus();
    delay(2000);
  }
}

void CheckCounter()
{
  if (cntr > 10)
  {
    cntr = 1;
  }
}

// setup() runs once, when the device is first turned on.
void setup()
{
  pinMode(SWITCH_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  cntr = 1;

  mfrc522.setSPIConfig();
  mfrc522.PCD_Init();

  Particle.subscribe("Card_ID", getCardID);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{

  CheckforManualRecalibrate();
  ReadCard();
  CheckCounter();
}

void ReadCard()
{
  if (mfrc522.PICC_IsNewCardPresent())
  {
    cntr += 1;
    uid = getID();
    Serial.print("Card detected, UID: ");
    Serial.println(uid);
  }
}
// get RFID tag ID
unsigned long getID()
{
  if (!mfrc522.PICC_ReadCardSerial())
  { // Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long int hex_num;
  hex_num = mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] << 8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  CheckPetStatus();
  return hex_num;
}
