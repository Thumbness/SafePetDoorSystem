/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/facef/OneDrive/Desktop/Embedded_Project/SafePetDoorSystem/src/DoorSystem.ino"
/*
 * Project DoorSystem
 * Description:
 * Author:
 * Date:
 */

#include "MFRC522.h"

void setup();
void loop();
void InitFlashLED();
#line 10 "c:/Users/facef/OneDrive/Desktop/Embedded_Project/SafePetDoorSystem/src/DoorSystem.ino"
#define SWITCH_PIN D4
#define BLUE_LED D5
#define GREEN_LED D7
#define YELLOW_LED D8
#define SS_PIN A5
#define RST_PIN A2

int switchState;
int cntr;

unsigned long uid;
unsigned long feederCard;
bool initSetup;

MFRC522 mfrc522(SS_PIN, RST_PIN);

void getCardID(const char *event, const char *data);
void updateStatusLED(const char *event, const char *data);
void GetPetStatus();
void PetInside();
void PetOutside();
void CheckforManualRecalibrate();
void CheckCounter();
void ReadCard();
unsigned long GetID();

// setup() runs once, when the device is first turned on.
void setup()
{
  pinMode(SWITCH_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  feederCard = 0;
  cntr = 1;
  initSetup = false;
  mfrc522.setSPIConfig();
  mfrc522.PCD_Init();
  Particle.subscribe("Card_ID", getCardID);
  Particle.subscribe("Pet_Fed", updateStatusLED);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  if (!initSetup)
  {
    InitFlashLED();
  }
  CheckforManualRecalibrate();
  ReadCard();
  CheckCounter();
}

void InitFlashLED()
{
  do
  {
    digitalWrite(BLUE_LED, HIGH);
    delay(500);
    digitalWrite(BLUE_LED, LOW);
    delay(500);
  } while (feederCard == 0);
  initSetup = true;
}

void getCardID(const char *event, const char *data)
{
  Serial.println("Card Read from other device!");
  std::string temp = (std::string)data;
  feederCard = stoul(temp, 0, 10);
  Serial.println(feederCard);
}

void GetPetStatus()
{
  if (cntr % 2 == 1)
  {
    PetInside();
  }
  if (cntr % 2 == 0)
  {
    PetOutside();
  }
}
void updateStatusLED(const char *event, const char *data)
{
  std::string temp = (std::string)data;
  if (temp == "true")
  {
    cntr = 1;
    GetPetStatus();
  }
}

void PetInside()
{
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  if (uid == feederCard)
  {
    Particle.publish("Pet_status", "inside");
  }
}

void PetOutside()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  if (uid == feederCard)
  {
    Particle.publish("Pet_status", "outside");
  }
}

void CheckforManualRecalibrate()
{
  switchState = digitalRead(SWITCH_PIN);
  if (switchState == HIGH)
  {
    cntr += 1;
    GetPetStatus();
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

void ReadCard()
{
  if (mfrc522.PICC_IsNewCardPresent())
  {
    cntr += 1;
    uid = GetID();
    Serial.print("Card detected, UID: ");
    Serial.println(uid);
    if (uid == feederCard)
    {
      GetPetStatus();
    }
  }
}
// get RFID tag ID
unsigned long GetID()
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

  return hex_num;
}
