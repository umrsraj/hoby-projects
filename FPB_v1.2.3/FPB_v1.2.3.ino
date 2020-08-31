/*
   Adding clear pin for reset all Fingerprint templets and id number except 1st thumb
   adding different soud functions for different operations
   adding 2 functions for enrol button(below 2sec press- enrollprocess, above 2sec process OFF RELAY)
   wont stuck in enroll prosess(wont waits for finger more than 5sec)
   At Enrol time taking 5 prints

*/
//#include <avr/wdt.h>
#include <stdio.h>
#include <EEPROM.h>
#include <Adafruit_Fingerprint.h>
#include<LiquidCrystal.h>
#include <stdlib.h>
LiquidCrystal lcd (A0, A1, A2, A3, A4, A5);

#define clearPin 10
#define en_sw  3  //5----------------------------------------------------+9++++++++++++
#define ver_sw 2  //6
#define bz     6
#define Rbat   12
#define Rign   11
uint16_t id;

int lowTime = 0;
int highTime = 0;


SoftwareSerial mySerial(4, 5);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial, 1337);

int getFingerprintIDez();

void setup()  {
  Serial.begin(115200);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);

  Serial.println(F("WELCOME"));

  pinMode(clearPin, INPUT_PULLUP);
  pinMode(en_sw, INPUT_PULLUP);
  pinMode(ver_sw, INPUT_PULLUP);
  pinMode(bz, OUTPUT);
  pinMode(Rign, OUTPUT);
  pinMode(Rbat, OUTPUT);

  digitalWrite(bz, LOW);
  digitalWrite(Rbat, LOW);
  digitalWrite(Rign, LOW);

  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  lcd.setCursor(0, 1);
  lcd.print(" .............. ");

start:
  //  Serial.println(F("WELCOME"));


  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    digitalWrite(bz, HIGH); delay(100);
    digitalWrite(bz, LOW); delay(50);
    digitalWrite(bz, HIGH); delay(100);
    digitalWrite(bz, LOW); delay(1);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1)
    {
      digitalWrite(bz, HIGH);
      delay(6000);
      digitalWrite(bz, LOW);
      delay(1000);
      goto start;
    }
  }
  //wdt_enable(WDTO_4S);

  //  Serial.println("pin 2 status - "+String(digitalRead(3)));
  //delay(2000);
  //  attachInterrupt(digitalPinToInterrupt(3), func1, RISING);
  if ((EEPROM.read(5)) != 10)
  {
    id = 0;
    Serial.println("id is = ZERO\n First RUN");
  }
  else {
    id = (EEPROM.read(4));
    id = (id & 0xff) | (EEPROM.read(3) << 8);
    Serial.print("id from EEPROM = "); Serial.println(id);
  }
  //  wdt_reset();
}//setup

void loop() {
  //wdt_reset();

  if (digitalRead(en_sw) == 0)
  {
    lowTime = millis();
    Serial.println("lowTime - " + String(lowTime));
    while (digitalRead(en_sw) == 0);
    highTime = millis();
    Serial.println("highTime - " + String(highTime));
    int bdelay = 1500; // time to be press the switch

    if (highTime - lowTime > bdelay)
    {
      Serial.println("millis - " + String(millis()));
      Serial.println(F("button pressed 2 times - Switching all relays OFF"));
      delay(300);
      digitalWrite(Rbat, LOW);
      delay(1000);
      digitalWrite(Rign, LOW);
      delay(300);
    }
    else if ((highTime - lowTime > 100) && (highTime - lowTime < bdelay))
    {
      Enrol_Process ();
    }
  }
  else if (digitalRead(clearPin) == 0)
  {
    id = 1;
    EEPROM.update(3, (id & 0xff00) >> 8);
    EEPROM.update(4, (id));
    Serial.println("id became 1");
    for (int temp = 2; temp < 256; temp++)
      deleteFingerprint(temp);
    Serial.println("Now database Cleared :)");
    digitalWrite(bz, HIGH); delay(100);
    digitalWrite(bz, LOW); delay(50);
    digitalWrite(bz, HIGH); delay(100);
    digitalWrite(bz, LOW); delay(1);
  }
  //  else if (digitalRead(ver_sw) == 0)
  //  {
  //    if (ver_process () > 0)
  //    {
  //      lcd.setCursor(0, 0);
  //      lcd.print("Switching BIKE ON        ");
  //      Serial.println("Switching BIKE ON");
  //      delay(50);
  //      digitalWrite(Rbat, HIGH);
  //      delay(1000);
  //      //      digitalWrite(Rign, HIGH);
  //      //      delay(1000);
  //    }
  //
  //  }
}//loop

bool getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println((id + 1));
  uint8_t waitCount = 0;
  while ((p != FINGERPRINT_OK) && (waitCount < 10)) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        waitCount++;
        delay(500);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  Serial.println();

  // OK success!
  if (waitCount < 10)
  {
    p = finger.image2Tz(1);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
    //    Serial.println("Remove finger");
    //    lcd.setCursor(0, 0);
    //    lcd.print("Remove Finger   ");
    //    digitalWrite(bz, HIGH); delay(50); digitalWrite(bz, LOW); delay(100);
    //digitalWrite(bz, HIGH); delay(50); digitalWrite(bz, LOW); delay(1);
    //    delay(200);
    p = 0;
    //    while (p != FINGERPRINT_NOFINGER) {
    //      p = finger.getImage();
    //      Serial.println("in WHILE.......");
    //    }
    id++;
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    // lcd.clear();
    //    lcd.setCursor(0, 0);
    //    lcd.print("Put Finger Again");
    //delay(1000);
    waitCount = 0;
    while ((p != FINGERPRINT_OK) && (waitCount < 10)) {
      p = finger.getImage();
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(",");
          waitCount++;
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    }
    Serial.println();
    // OK success!
    if (waitCount < 10)
    {
      Serial.println("waitCount: " + String(waitCount));
      p = finger.image2Tz(2);
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image converted");
          break;
        case FINGERPRINT_IMAGEMESS:
          Serial.println("Image too messy");
          return false;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          return false;
        case FINGERPRINT_FEATUREFAIL:
          Serial.println("Could not find fingerprint features");
          return false;
        case FINGERPRINT_INVALIDIMAGE:
          Serial.println("Could not find fingerprint features");
          return false;
        default:
          Serial.println("Unknown error");
          return false;
      }

      // OK converted!
      Serial.print("Creating model for #");  Serial.println(id);

      p = finger.createModel();
      if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return false;
      } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        id--;
        digitalWrite(bz, HIGH); delay(1000);
        digitalWrite(bz, LOW); delay(1);
        return false;
      } else {
        Serial.println("Unknown error");
        return false;
      }

      Serial.print("ID "); Serial.println(id);
      p = finger.storeModel(id);
      if (p == FINGERPRINT_OK) {
        Serial.println("Stored!");
        //   lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Enroll Success  ");

        digitalWrite(bz, HIGH); delay(100);
        digitalWrite(bz, LOW); delay(50);
        digitalWrite(bz, HIGH); delay(100);
        digitalWrite(bz, LOW); delay(1);
        return true;

      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return false;
      } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return false;
      } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return false;
      } else {
        Serial.println("Unknown error");
        return false;
      }
    }//waitCount
    else
    {
      id--;
      return false;
    }
  }//waitCount
}
//////////////  Enroll End  ///////////////////

//////////////   Verify    ///////////////////

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }
}


uint8_t Enrol_Process ()
{
  Serial.println(F("Enrolling finger......"));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enrolling.......");
  delay(50);

  if (ver_process() == 1)
  {
    //    id++;
    //    Serial.print(F("Enrolling ID #"));
    //    Serial.println(id);

    for (int temp = 0; temp < 5000; temp++)
    {
      if (digitalRead(ver_sw) == 1) {
        break;
      }
      delay(1);
    }
    uint8_t tID = 0;
    uint8_t trying = 0;
    while (tID < 5)
    {
      if (getFingerprintEnroll())
      {
        tID++;
        Serial.println("*************tID: " + String(tID) + "*************\n\n");
        digitalWrite(bz, HIGH); delay(100); digitalWrite(bz, LOW); delay(50);
        digitalWrite(bz, HIGH); delay(100); digitalWrite(bz, LOW); delay(500);
        trying = 0;
      }
      else
      {
        Serial.println("trying: " + String(trying) + ".................");
        trying++;
        digitalWrite(bz, HIGH); delay(100); digitalWrite(bz, LOW); delay(1);
        if (trying > 3)break;
      }
    }
    EEPROM.update(3, (id & 0xff00) >> 8);
    EEPROM.update(4, (id));
    if (EEPROM.read(5) != 10)
    {
      EEPROM.update(5, 10);
    }
    delay(1000);
  }
  else
  {
    Serial.println(F("authority finger not matched"));
    digitalWrite(bz, HIGH); delay(1000);
    digitalWrite(bz, LOW); delay(1);
  }
}
uint8_t ver_process ()
{
  lcd.setCursor(0, 0);
  lcd.print("Ver.Finger To Acc");
  delay(2);
  finger.getTemplateCount();
  //    Serial.print(F("Sensor contains ")); Serial.print(finger.templateCount); Serial.println(F(" templates"));
  //    Serial.println(F("Waiting for valid finger..."));
  int16_t ID;
  for (int temp = 0; temp < 5000; temp++)
  {
    if (digitalRead(ver_sw) == 0) {
      break;
    }
    delay(1);
  }
  ID = getFingerprintIDez(); //returns  Finger Id
  delay(5);            //don't ned to run this at full speed.
  Serial.println("ID = " + String(ID));

  if (ID > 0) {
    Serial.println(F("Match Found With ID"));
    lcd.setCursor(0, 1);
    lcd.print("MATCH FOUND..........");
    digitalWrite(bz, HIGH); delay(100); digitalWrite(bz, LOW); delay(50);
    digitalWrite(bz, HIGH); delay(100); digitalWrite(bz, LOW); delay(1);
    return 1;
  }
  else {
    Serial.println("Match NOT Found ");
    lcd.setCursor(0, 0);
    lcd.print("INVALID USER    ");
    lcd.setCursor(0, 1);
    lcd.print("PLEASE CHECK    ");
    digitalWrite(bz, HIGH); delay(1000); digitalWrite(bz, LOW);
    //digitalWrite(bz, HIGH); delay(50); digitalWrite(bz, LOW);
    return 0;
  }
}
