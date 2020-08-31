#include <String.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial bt(2, 3);

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define rtc_address 0x68
long int utime = 0;
#define loc_of_ONm 0
#define loc_of_ONh 1
#define loc_of_OFFm 2
#define loc_of_OFFh 3
#define loc_of_relay 4

#define relay1 8
#define relay2 9
#define relay3 10

int i1 = 0, j1 = 0, k1 = 0;
int i2 = 0, j2 = 0, k2 = 0;
int count = 0;
int flag = 0;
bool ontime = false, offtime = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Welcome to Street Light Control System......!!!!");

  bt.begin(9600);
  bt.println("Welcome to Street Light Control System......!!!!");

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  Wire.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    bt.println("Couldn't find RTC");
    delay(1000);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC Not Running");
    bt.println("RTC Not Running");
    delay(1000);
    set_rtc();
  }

  if ((EEPROM.read(loc_of_ONh) > 23) || (EEPROM.read(loc_of_ONm) > 59))
  {
    Serial.println("ON Alarm setting---------");
    onalarmset();
  }
  if ((EEPROM.read(loc_of_OFFh) > 23) || (EEPROM.read(loc_of_OFFm) > 59))
  {
    Serial.println("OFF Alarm setting---------");
    offalarmset();
  }


  j1 = EEPROM.read(loc_of_ONh);
  k1 = EEPROM.read(loc_of_ONh);
  j2 = EEPROM.read(loc_of_OFFh);
  k2 = EEPROM.read(loc_of_OFFh);

  digitalWrite(relay1, EEPROM.read(loc_of_relay));
  delay(500);
  digitalWrite(relay2, EEPROM.read(loc_of_relay));
  delay(500);
  digitalWrite(relay3, EEPROM.read(loc_of_relay));
  delay(500);



}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();

  if (((j1 <= now.hour()) || (j2 >= now.hour())) && (flag == 0))
  {
    if ((now.hour() == j1) && (now.minute() > k1))
    {
      relays_on();
      return;
    }
    else if ((now.hour() == j2) && (now.minute() < k2))
    {
      relays_on();
      return;
    }
    else
    {
      relays_on();
      return;
    }
  }
  else if (((j2 <= now.hour()) || (j1 >= now.hour())) && (flag == 1))
  {
    if ((now.hour() == j2) && (now.minute() > k2))
    {
      relays_off();
      return;
    }
    else if ((now.hour() == j1) && (now.minute() < k1))
    {
      relays_off();
      return;
    }
    else
    {
      relays_off();
      return;
    }
  }
  else if (bt.available() != 0)
  {
    String data = bt.readString();
    bt.print(data);
    delay(1000);
    Serial.println("Data: " + String(data));

    if (data.indexOf("ont?") != -1)
    {
      bt.print(j1); bt.print(":"); bt.println(k1);
      delay(200);
    }
    else if (data.indexOf("offt?") != -1)
    {
      bt.print(j2); bt.print(":"); bt.println(k2);
      delay(200);
    }
    else if (data.indexOf("ont") != -1)
    {
      //Serial.println("Calling onalarmset()");
      ontime = false;
      onalarmset();
    }
    else if (data.indexOf("offt") != -1)
    {
      //Serial.println("Calling offalarmset ()");
      offtime = false;
      offalarmset();
    }
    else if (data.indexOf("set_time") != -1)
    {
      //Serial.println("Calling set_time()");
      set_rtc();
    }
    else if (data.indexOf("time?") != -1)
    {
      bt.print(now.hour()); bt.print(":"); bt.println(now.minute());
      delay(200);
    }

    else
    {
      bt.println("Wrong Command!!\n"); delay(100);
      bt.println("Actual Commands are:\n1.ont?\n2.offt?\n3.ont\n4.offt\n5.set_time\n6.time?\n"); delay(300);
    }
  }
  else
  {
    if (millis() - utime > 30000) {
      Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());
      bt.print(now.hour()); bt.print(":"); bt.println(now.minute());
      delay(200);
      utime = millis();
    }
  }
}//loop


void onalarmset ()
{
  Serial.print("Waiting for Lights ON time ...........!");
  bt.println("Waiting for Lights ON time ...........!\n");
  delay(100);
  while (ontime == false)
  {
    if (bt.available() > 0)
    {
      // String buff = SerialData();
      String buff = bt.readString();
      bt.print(buff);
      delay(1000);
      Serial.println("Data: " + String(buff));
      Serial.print("buff: "); Serial.println(buff);
      String buff1 = buff.substring(0, buff.indexOf(":"));
      String buff2 = buff.substring(buff.indexOf(":") + 1);
      Serial.print("buff1: "); Serial.println(buff1);
      Serial.print("buff2: "); Serial.println(buff2);
      j1 = buff1.toInt();
      k1 = buff2.toInt();
      Serial.print("j1: "); Serial.println(j1);
      Serial.print("k1: "); Serial.println(k1);
      EEPROM.write(loc_of_ONh, j1);
      EEPROM.write(loc_of_ONm, k1);
      bt.println("ON Time stored Successfully @ "+String(EEPROM.read(loc_of_ONh))+":"+String(EEPROM.read(loc_of_ONm)));
      delay(300);
      ontime = true;
    }
  }//while
}// on alarm set ()

void offalarmset ()
{
  Serial.print("offtime "); Serial.println(offtime);
  bt.println("Waiting for Lights OFF time ...........!");
  delay(100);
  while (offtime == false)
  {
    if (bt.available() > 0)
    {
      //      String buff = SerialData();
      String buff = bt.readString();
      bt.print(buff);
      delay(1000);
      Serial.print("buff: "); Serial.println(buff);
      String buff1 = buff.substring(0, buff.indexOf(":"));
      String buff2 = buff.substring(buff.indexOf(":") + 1);
      Serial.print("buff1: "); Serial.println(buff1);
      Serial.print("buff2: "); Serial.println(buff2);
      j2 = buff1.toInt();
      k2 = buff2.toInt();
      Serial.print("j2: "); Serial.println(j2);
      Serial.print("k2: "); Serial.println(k2);
      EEPROM.write(loc_of_OFFh, j2);
      EEPROM.write(loc_of_OFFm, k2);
      Serial.print("j2: "); Serial.println(EEPROM.read(loc_of_OFFh));
      Serial.print("k2: "); Serial.println(EEPROM.read(loc_of_OFFm));
      bt.println("OFF Time stored Successfully @ "+String(EEPROM.read(loc_of_OFFh))+":"+String(EEPROM.read(loc_of_OFFm)));
      delay(300);
      offtime = true;
    }
  }//while
}//off alarm set ()
//-----------------------------------------------------------------
/***************************rtc*******************************/

void set_rtc()
{
  char rtc_str[48];
  bt.println("Please Enter Time in HH:MM formet");
  delay(100);
  while (bt.available() == 0);
  if (bt.available() != 0)
  {
    //    String str = SerialData();
    String str = bt.readString();
    bt.print(str);
    delay(1000);
    Serial.print("str: "); Serial.println(str);
    str.toCharArray(rtc_str, str.length());
    Serial.print("rtc_str: "); Serial.println(rtc_str);
  }
  char *ptr_rtc;
  int rcv_sep[7];
  int index = 0;
  byte Sec = 0, Min = 0, Hr = 0, Weakd = 0, Date = 0, Month = 0, Year = 0;

  ptr_rtc = strtok (rtc_str, ":");
  int a = atoi(ptr_rtc);
  rcv_sep[2] = a;
  ptr_rtc = strtok (NULL, ":");
  a = atoi(ptr_rtc);
  rcv_sep[1] = a;
  
  Sec = byte(rcv_sep[0]);
  Min = byte(rcv_sep[1]);
  Hr = byte(rcv_sep[2]);
  Weakd = byte(rcv_sep[3]);
  Date = byte(rcv_sep[4]);
  Month = byte(rcv_sep[5]);
  Year = byte(rcv_sep[6]);

  Serial.print("Sec: "); Serial.println(Sec);
  Serial.print("Min: "); Serial.println(Min);
  Serial.print("Hr: "); Serial.println(Hr);
  Serial.print("Weakd: "); Serial.println(Weakd);
  Serial.print("Date: "); Serial.println(Date);
  Serial.print("Month: "); Serial.println(Month);
  Serial.print("Year: "); Serial.println(Year);

  rtc.adjust(DateTime(2020, 8, 6, Hr, Min, 0));
  // setDS3231time(Sec, Min, Hr, Weakd, Date, Month, Year);
}
String SerialData()
{
  char bufer[256];
  int cnt = 0;
  String str = "";
  while (Serial.available() > 0)
  {
    bufer[cnt++] = Serial.read();
  }
  for (int i = 0; i < cnt; i++)
  {
    str += bufer[i];
  }
  Serial.print("str: "); Serial.println(str);
  return str;
}
void relays_on()
{
  EEPROM.update(loc_of_relay, 0);
  delay(10);

  Serial.println("alarm ON Time Matched");
  digitalWrite(relay1, LOW);
  delay(1000);
  digitalWrite(relay2, LOW);
  delay(1000);
  digitalWrite(relay3, LOW);
  delay(1000);
  flag = 1;
}

void relays_off()
{
  EEPROM.update(loc_of_relay, 1);
  delay(10);

  Serial.println("alarm OFF Time Matched");
  digitalWrite(relay1, HIGH);
  delay(1000);
  digitalWrite(relay2, HIGH);
  delay(1000);
  digitalWrite(relay3, HIGH);
  delay(1000);
  flag = 0;
}
