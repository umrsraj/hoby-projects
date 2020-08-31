#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#error "Board not found"
#endif

#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <String.h>

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


#define no_of_nodes 4
#define startofnodes 0

#define node0load1 startofnodes+0
#define node0load2 startofnodes+1
#define node0load3 startofnodes+2

#define node1load1 node0load3+1
#define node1load2 node0load3+2
#define node1load3 node0load3+3

#define node2load1 node1load3+1
#define node2load2 node1load3+2
#define node2load3 node1load3+3

#define node3load1 node2load3+1
#define node3load2 node2load3+2
#define node3load3 node2load3+3

struct Nodes
{
  uint8_t load1: 1;
  uint8_t load2: 1;
  uint8_t load3: 1;
} node[no_of_nodes];
//typedef struct Nodes nodes;

//nodes node[no_of_nodes];

char index_webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head >
<title >Raj's Home</title>

<style>
body {background-color: powderblue;}
h1   {color: tomato;text-align:center;}
h2   {color: blue;text-align:center;}
h3   {color: red;text-align:center;}
a    {text-align: center;color: blue;}
p    {color: red;}
</style>

</head>
<body >

<h1 >Well Come To The Project</h1>

<a href="file:///C:/Users/Nagraju/Desktop/index%20page.html"> <h4  title="You Can change SSID and PASSWORD For This LAN N/W" style="color:blue;"> <strong> <i> Change WiFi Credentials </i> </strong> </h4> </a>

<a href="http://192.168.4.1/automate"> <h4  title="Control Your Appliances" style="color: blue;"> <strong> <i> Automate Things </i> </strong> </h4> </a>

</body>
</html>
)=====";
char automation_webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head >
<title >AUTOMATION</title>

<style>
body {background-color: powderblue;}
h1   {color: blue;text-align:center;}
h2   {color: dodgerblue;text-align:center;}
h3   {color: tomato;text-align:center;}
a    {text-align: left;color: blue;}
p    {color: red;}
</style>
</head>
<body >
<!-----text alignment and text color combinedly not workig for this H1(heading 1)---->
<h1 >Control Your Devices</h1>
<a href="http//192.168.4.1/"> <h4  title="Go back"> <strong> <i> <-- Go Back</i> </strong> </h4> </a>
<h2> Node0 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node0load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node0load1/off'">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node0load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node0load2/off'">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node0load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node0load3/off'">Off</button> </h3>
<h2> Node1 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load1/off'">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load2/off'">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load3/off'">Off</button> </h3>
<h2> Node2 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load1/off'">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load2/off'">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load3/off'">Off</button> </h3>
<h2> Node3 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node3load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node3load1/off'">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node3load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node3load2/off'">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node3load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node3load3/off'">Off</button> </h3>
</body>
</html>
)=====";

char* ssid = "ESP32";
char* password = "123456789";

AsyncWebServer server(80); // server port 80

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}

void setup(void)
{

    EEPROM.begin(512);
  Serial.begin(115200);
  Serial.println("\n********* SERVER CODE Started **********\n"); 

  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
    
  WiFi.softAP(ssid,password);
  Serial.print("softapIP: ");
  Serial.println(WiFi.softAPIP());


  Serial.println("");

  if (MDNS.begin("esp")) { //esp.local/
    Serial.println("MDNS responder started");
  }

for(uint8_t k=0; k< no_of_nodes; k++){
  node[k].load1=0;
  node[k].load2=0;
  node[k].load3=0;
}

  for(uint8_t i=0;i<no_of_nodes;i++)
  {
    static uint8_t t = startofnodes; 
  node[i].load1 = EEPROM.read(t);
  t++;  
  node[i].load2 = EEPROM.read(t);
  t++;  
  node[i].load3 = EEPROM.read(t);
  t++;
  }

  Serial.println();
  for (uint8_t i = 0; i < no_of_nodes; i++)
  {
    Serial.print("node["+String(i)+"].load1:"); Serial.println(node[i].load1);
    Serial.print("node["+String(i)+"].load2:"); Serial.println(node[i].load2);
    Serial.print("node["+String(i)+"].load3:"); Serial.println(node[i].load3);
    Serial.println();
  }

  digitalWrite(relay1,node[0].load1);
  digitalWrite(relay2,node[0].load2);
  digitalWrite(relay3,node[0].load3);

  server.on("/", [](AsyncWebServerRequest * request)
  { 
    request->send_P(200, "text/html", index_webpage);
    //request->send_P(200, "text/plain", webpage11);
  });
  server.on("/automate", [](AsyncWebServerRequest * request)
  { 
    request->send_P(200, "text/html", automation_webpage);
    //request->send_P(200, "text/plain", "UPDATE PAGE RESULTS");
  });
//////////////////////////////////////////////////////////////////////////
  
  server.on("/node1status", [](AsyncWebServerRequest * request)
  { 
    //request->send_P(200, "text/html", automation_webpage);
    String buff = "status-"+String(node[1].load1)+"-"+String(node[1].load2)+"-"+String(node[1].load3);
    char nodedata[buff.length()+1];
    buff.toCharArray(nodedata,buff.length()+1);
//    Serial.print("buff DATA: ");Serial.println(buff);
    Serial.print("node1 DATA: ");Serial.println(nodedata);
    request->send_P(200, "text/plain", nodedata);
  });
  server.on("/node2status", [](AsyncWebServerRequest * request)
  { 
    //request->send_P(200, "text/html", automation_webpage);
    String buff = "status-"+String(node[2].load1)+"-"+String(node[2].load2)+"-"+String(node[2].load3);
    char nodedata[buff.length()+1];
    buff.toCharArray(nodedata,buff.length()+1);
//    Serial.print("buff LEN: ");Serial.println(buff.length());
    Serial.print("node2 DATA: ");Serial.println(nodedata);
    request->send_P(200, "text/plain", nodedata);
  });
  server.on("/node3status", [](AsyncWebServerRequest * request)
  { 
    //request->send_P(200, "text/html", automation_webpage);
    String buff = "status-"+String(node[3].load1)+"-"+String(node[3].load2)+"-"+String(node[3].load3);
    char nodedata[buff.length()+1];
    buff.toCharArray(nodedata,buff.length()+1);
//    Serial.print("buff DATA: ");Serial.println(buff);
    Serial.print("node3 DATA: ");Serial.println(nodedata);
    request->send_P(200, "text/plain", nodedata);
  });
/////////////////////////////////////////////////////////////////////////////////////////////////////////
  /********** NODE 0  ***************/
  server.on("/node0load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load1,1);
    EEPROM.commit();
    node[0].load1 = 1;
    Serial.print("node[0].load1:"); Serial.println(node[0].load1);
    digitalWrite(relay1,node[0].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node0load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load1,0);
    EEPROM.commit();
    node[0].load1 = 0;
    Serial.print("node[0].load1:"); Serial.println(node[0].load1);
    digitalWrite(relay1,node[0].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
///////////////////////////////////////////////////////////////////  
  server.on("/node0load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load2,1);
    EEPROM.commit();
    node[0].load2 = 1;
    Serial.print("node[0].load2:"); Serial.println(node[0].load2);
    digitalWrite(relay2,node[0].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node0load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load2,0);
    EEPROM.commit();
    node[0].load2 = 0;
    Serial.print("node[0].load2:"); Serial.println(node[0].load2);
    digitalWrite(relay2,node[0].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  ///////////////////////////////////////////////////////////////////
  server.on("/node0load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load3,1);
    EEPROM.commit();
    node[0].load3 = 1;
    Serial.print("node[0].load3:"); Serial.println(node[0].load3);
    digitalWrite(relay3,node[0].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node0load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node0load3,0);
    EEPROM.commit();
    node[0].load3 = 0;
    Serial.print("node[0].load3:"); Serial.println(node[0].load3);
    digitalWrite(relay3,node[0].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  /////////////////////////////////////////////////////////
  
  
  /********** NODE 1 ***************/
  server.on("/node1load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load1,1);
    EEPROM.commit();
    node[1].load1 = 1;
    Serial.print("node[1].load1:"); Serial.println(node[1].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node1load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load1,0);
    EEPROM.commit();
    node[1].load1 = 0;
    Serial.print("node[1].load1:"); Serial.println(node[1].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
///////////////////////////////////////////////////////////////////  
  server.on("/node1load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load2,1);
    EEPROM.commit();
    node[1].load2 = 1;
    Serial.print("node[1].load2:"); Serial.println(node[1].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node1load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load2,0);
    EEPROM.commit();
    node[1].load2 = 0;
    Serial.print("node[1].load2:"); Serial.println(node[1].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  ///////////////////////////////////////////////////////////////////
  server.on("/node1load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load3,1);
    EEPROM.commit();
    node[1].load3 = 1;
    Serial.print("node[1].load3:"); Serial.println(node[1].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node1load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node1load3,0);
    EEPROM.commit();
    node[1].load3 = 0;
    Serial.print("node[1].load3:"); Serial.println(node[1].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  /////////////////////////////////////////////////////////

  
  /********** NODE 2 ***************/
  server.on("/node2load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load1,1);
    EEPROM.commit();
    node[2].load1 = 1;
    Serial.print("node[2].load1:"); Serial.println(node[2].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node2load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load1,0);
    EEPROM.commit();
    node[2].load1 = 0;
    Serial.print("node[2].load1:"); Serial.println(node[2].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
///////////////////////////////////////////////////////////////////  
  server.on("/node2load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load2,1);
    EEPROM.commit();
    node[2].load2 = 1;
    Serial.print("node[2].load2:"); Serial.println(node[2].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node2load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load2,0);
    EEPROM.commit();
    node[2].load2 = 0;
    Serial.print("node[2].load2:"); Serial.println(node[2].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  ///////////////////////////////////////////////////////////////////
  server.on("/node2load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load3,1);
    EEPROM.commit();
    node[2].load3 = 1;
    Serial.print("node[2].load3:"); Serial.println(node[2].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node2load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node2load3,0);
    EEPROM.commit();
    node[2].load3 = 0;
    Serial.print("node[2].load3:"); Serial.println(node[2].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  /////////////////////////////////////////////////////////
  
  /********** NODE 3 ***************/
  server.on("/node3load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load1,1);
    EEPROM.commit();
    node[3].load1 = 1;
    Serial.print("node[3].load1:"); Serial.println(node[3].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node3load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load1,0);
    EEPROM.commit();
    node[3].load1 = 0;
    Serial.print("node[3].load1:"); Serial.println(node[3].load1);
    request->send_P(200, "text/html", automation_webpage);
  });
///////////////////////////////////////////////////////////////////  
  server.on("/node3load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load2,1);
    EEPROM.commit();
    node[3].load2 = 1;
    Serial.print("node[3].load2:"); Serial.println(node[3].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node3load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load2,0);
    EEPROM.commit();
    node[3].load2 = 0;
    Serial.print("node[3].load2:"); Serial.println(node[3].load2);
    request->send_P(200, "text/html", automation_webpage);
  });
  ///////////////////////////////////////////////////////////////////
  server.on("/node3load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load3,1);
    EEPROM.commit();
    node[3].load3 = 1;
    Serial.print("node[3].load3:"); Serial.println(node[3].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  
  server.on("/node3load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    EEPROM.write(node3load3,0);
    EEPROM.commit();
    node[3].load3 = 0;
    Serial.print("node[3].load3:"); Serial.println(node[3].load3);
    request->send_P(200, "text/html", automation_webpage);
  });
  /////////////////////////////////////////////////////////


  server.onNotFound(notFound);

  server.begin();  // it will start webserver
  Serial.println("Server Started.....");
}


void loop(void)
{
}
