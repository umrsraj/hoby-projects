/*
   Node-3 client Code
   Latest Code

*/
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "Board not found"
#endif

#include <EEPROM.h>
#include <String.h>
#include <stdlib.h>

#ifdef ESP8266
#define relay1 D0
#define relay2 D1
#define relay3 D2
#elif defined(ESP32)
#define relay1 25
#define relay2 26
#define relay3 27
#else
#error "Board not found"
#endif

#define no_of_loads 3
#define startofnodes 0

#define load1 startofnodes+0
#define load2 startofnodes+1
#define load3 startofnodes+2

uint8_t L1;
uint8_t L2;
uint8_t L3;

uint8_t pL1 = 0;
uint8_t pL2 = 0;
uint8_t pL3 = 0;

const char* ssid = "ESP32";
const char* password = "123456789";

const char* host = "192.168.4.1";

WiFiClient client;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n***********Client code Starts***********");

repeat:

  Serial.println("\n Trying TO Connect WI-FI\n");
  WiFi.begin(ssid, password);

  for (uint8_t k = 0; ((WiFi.status() != WL_CONNECTED) && (k < 20)); k++) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED)
    goto repeat;

  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED)
  {
    connect_wifi();
  }
  else
  {
    if (client.connect(host, 80))
    {
      String url = "/node3status";
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" +
                   "Connection: keep-alive\r\n\r\n");
      delay(10);
      Serial.println("Response: ");
      while (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print("Line: "); Serial.println(line);
        if (line.indexOf("statu") != -1)
          data_process (line);
      }
    }
    Serial.println("--------------------------------------------------------");
    delay(300);
  }

}//loop
void data_process(String line)
{
  Serial.print("Line-2: "); Serial.println(line);
  char nodedata[line.length() + 1];
  line.toCharArray(nodedata, line.length() + 1);
  Serial.print("Nodedata: "); Serial.println(nodedata);
  char *temp, *temp1, *temp2, *temp3;
  temp = strtok(nodedata, "-");
  temp1 = strtok(NULL, "-");
  temp2 = strtok(NULL, "-");
  temp3 = strtok(NULL, "-");
  Serial.print("temp1: "); Serial.println(temp1);
  Serial.print("temp2: "); Serial.println(temp2);
  Serial.print("temp3: "); Serial.println(temp3);

  L1 = atoi(temp1);
  L2 = atoi(temp2);
  L3 = atoi(temp3);
  Serial.print("L1: "); Serial.println(L1);
  Serial.print("L2: "); Serial.println(L2);
  Serial.print("L3: "); Serial.println(L3);

  if (pL1 != L1)
  {
    pL1 = L1;
    Serial.println("Load-1 Changed");
    digitalWrite(relay1, L1);
    delay(500);
  }
  if (pL2 != L2)
  {
    pL2 = L2;
    Serial.println("Load-2 Changed");
    digitalWrite(relay2, L2);
    delay(500);
  }
  if (pL3 != L3)
  {
    pL3 = L3;
    Serial.println("Load-3 Changed");
    digitalWrite(relay3, L3);
    delay(500);
  }
}
void connect_wifi ()
{
repeat:
  Serial.println("\n Trying TO Connect WI-FI\n");
  WiFi.begin(ssid, password);

  for (uint8_t k = 0; ((WiFi.status() != WL_CONNECTED) && (k < 20)); k++) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED)
    goto repeat;

  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
}
