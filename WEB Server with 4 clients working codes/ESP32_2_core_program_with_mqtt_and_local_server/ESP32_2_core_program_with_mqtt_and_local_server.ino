#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESPAsyncWebServer.h>

#include <String.h>
#include <stdlib.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

volatile uint8_t pin1 = 0;
volatile uint8_t pin2 = 0;
volatile uint8_t pin3 = 0;
volatile uint8_t pin4 = 0;
volatile uint8_t pin5 = 0;
volatile uint8_t pin6 = 0;
volatile uint8_t pin7 = 0;
volatile uint8_t pin8 = 0;
volatile uint8_t pin9 = 0;
volatile uint8_t pin10 = 0;

char* APssid = "ESP32";
char* APpassword = "123456789";

char* ssid = "project12";
char* password = "project123456";

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "rajender"
#define AIO_KEY         "78a4d22ccf2e4c17b54aa755ba8de599"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe onoffbutton1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay-1");
Adafruit_MQTT_Subscribe onoffbutton2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay-2");
Adafruit_MQTT_Subscribe onoffbutton3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay-3");
Adafruit_MQTT_Subscribe onoffbutton4 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay-4");
Adafruit_MQTT_Subscribe onoffbutton5 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay-5");
Adafruit_MQTT_Subscribe onoffbutton6 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/home control");
Adafruit_MQTT_Subscribe onoffbutton7 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led notice board");
Adafruit_MQTT_Subscribe onoffbutton8 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/noticeboard");
Adafruit_MQTT_Subscribe onoffbutton9 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/pir");
Adafruit_MQTT_Subscribe onoffbutton10 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/PIR Status");


AsyncWebServer server(80); // server port 80
void MQTT_connect();
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("********** Program Started **********");

  WiFi.mode(WIFI_AP_STA);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);
}//setup

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ) {
  Serial.print("\nTask1 running on core ");
  Serial.println(xPortGetCoreID());

repeat:

  Serial.println("\n Trying TO Connect WI-FI\n");
  WiFi.begin(ssid, password);

  for (uint8_t k = 0; ((WiFi.status() != WL_CONNECTED) && (k < 20)); k++) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED)
    goto repeat;

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton1);
  mqtt.subscribe(&onoffbutton2);
  mqtt.subscribe(&onoffbutton3);
  mqtt.subscribe(&onoffbutton4);
  mqtt.subscribe(&onoffbutton5);
  mqtt.subscribe(&onoffbutton6);
  mqtt.subscribe(&onoffbutton7);
  mqtt.subscribe(&onoffbutton8);
  mqtt.subscribe(&onoffbutton9);
  mqtt.subscribe(&onoffbutton10);

  for (;;) {
    delay(10);
    if (!(mqtt.connected()))
      MQTT_connect();

    // this is our 'wait for incoming subscription packets' busy subloop
    // try to spend your time here

    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(50))) {
      if (subscription == &onoffbutton1) {
        Serial.print(F("Relay-1 Got: "));
        Serial.println((char *)onoffbutton1.lastread);
        pin1 = atoi ((char *)onoffbutton1.lastread);
        Serial.println(pin1);
      }
      else if (subscription == &onoffbutton2) {
        Serial.print(F("Relay-2 Got: "));
        Serial.println((char *)onoffbutton2.lastread);
        pin2 = atoi ((char *)onoffbutton2.lastread);
        Serial.println(pin2);
      }
      else if (subscription == &onoffbutton3) {
        Serial.print(F("Relay-3 Got: "));
        Serial.println((char *)onoffbutton3.lastread);
        pin3 = atoi ((char *)onoffbutton3.lastread);
        Serial.println(pin3);
      }
      else if (subscription == &onoffbutton4) {
        Serial.print(F("Relay-4 Got: "));
        Serial.println((char *)onoffbutton4.lastread);
        pin4 = atoi ((char *)onoffbutton4.lastread);
        Serial.println(pin4);
      }
      else if (subscription == &onoffbutton5) {
        Serial.print(F("Relay-5 Got: "));
        Serial.println((char *)onoffbutton5.lastread);
        pin5 = atoi ((char *)onoffbutton5.lastread);
        Serial.println(pin5);
      }
      else if (subscription == &onoffbutton6) {
        Serial.print(F("Relay-6 Got: "));
        Serial.println((char *)onoffbutton6.lastread);
        pin6 = atoi ((char *)onoffbutton6.lastread);
        Serial.println(pin6);
      }
      else if (subscription == &onoffbutton7) {
        Serial.print(F("Relay-7 Got: "));
        Serial.println((char *)onoffbutton7.lastread);
        pin7 = atoi ((char *)onoffbutton7.lastread);
        Serial.println(pin7);
      }
      else if (subscription == &onoffbutton8) {
        Serial.print(F("Relay-8 Got: "));
        Serial.println((char *)onoffbutton8.lastread);
        pin8 = atoi ((char *)onoffbutton8.lastread);
        Serial.println(pin8);
      }
      else if (subscription == &onoffbutton9) {
        Serial.print(F("Relay-9 Got: "));
        Serial.println((char *)onoffbutton9.lastread);
        pin9 = atoi ((char *)onoffbutton9.lastread);
        Serial.println(pin9);
      }
      else if (subscription == &onoffbutton10) {
        Serial.print(F("Relay-10 lot: "));
        Serial.println((char *)onoffbutton10.lastread);
        pin10 = atoi ((char *)onoffbutton10.lastread);
        Serial.println(pin10);
      }
    }//while
  }
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ) {
  Serial.print("\nTask2 running on core ");
  Serial.println(xPortGetCoreID());

  WiFi.softAP(APssid, APpassword);
  Serial.print("SoftAP IP: "); Serial.println(WiFi.softAPIP());

  server.begin();  // it will start webserver
  Serial.println("Server Started.....");

  server.on("/", [](AsyncWebServerRequest * request)
  {
    //request->send_P(200, "text/html", index_webpage);
    char webpage[64];
    String msg = "Welcome to MY Project, Relay: " + String(pin1) + "-" + String(pin2) + "-" + String(pin3) + "-" + String(pin4) + "-" + String(pin5) + "-" + String(pin6) + "-" + String(pin7) + "-" + String(pin8) + "-" + String(pin9) + "-" + String(pin10);
    msg.toCharArray(webpage, msg.length() + 1);
    request->send_P(200, "text/plain", webpage);
  });

  for (;;) {
    delay(10);
  }
}

void loop() {

}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
