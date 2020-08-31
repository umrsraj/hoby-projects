/*

  adding websockets and JSON formetted data - DONE

  changing webpage data to bidirectional communication for EEPROM compatability - DONE

  adding changing credentials page


*/
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
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
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

#define locof_sta_ssid 20 // space max 20 char
#define locof_sta_pass 40 // space max 20 char
#define locof_temp_ap_ssid 60 // space max 20 char
#define locof_temp_ap_pass 80 // space max 20 char
#define locof_ap_ssid 100 // space max 20 char
#define locof_ap_pass 120 // space max 20 char



struct Nodes
{
  uint8_t load1: 1;
  uint8_t load2: 1;
  uint8_t load3: 1;
} node[no_of_nodes];
//typedef struct Nodes nodes;

//nodes node[no_of_nodes];

//////////////////////////////////////////////////////////////////////////////////////
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

<a href="http://192.168.4.1/cred"> <h4  title="You Can change SSID and PASSWORD For This LAN N/W" style="color:blue;"> <strong> <i> Change WiFi Credentials </i> </strong> </h4> </a>

<a href="http://192.168.4.1/automate"> <h4  title="Control Your Appliances" style="color: blue;"> <strong> <i> Automate Things </i> </strong> </h4> </a>

</body>
</html>
)=====";

//////////////////////////////////////////////////////////////////////////////////////
char cred_webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head >
<title >Raj's Home</title>

<style>
body {background-color: powderblue;}
h1   {color: tomato;text-align:left;}
h2   {color: blue;text-align:left;}
h3   {color: red;text-align:left;}
a    {text-align:left;color: blue;}
p    {color: red;}
</style>

</head>
<body>

<h2>Change WiFi Credentials</h2>

<a href="/"> <h4  title="Go back"> <strong> <i> <-- Go Back</i> </strong> </h4> </a>

<form action="/sta_cred">
  <label for="nSSID">WiFi Name:</label><br>
  <input type="text" id="nSSID" name="nSSID" value=""><br>
  <label for="nPASS">WiFi Password:</label><br>
  <input type="text" id="nPASS" name="nPASS" value=""><br><br>
  <input type="submit" value="Submit">
</form> 

<h2>Change AccessPoint(AP) Credentials</h2>

<form action="/ap_credentials">
  <label for="nSSID">AP Name:</label><br>
  <input type="text" id="nSSID" name="nSSID" value=""><br>
  <label for="nPASS">AP Password:</label><br>
  <input type="text" id="nPASS" name="nPASS" value=""><br><br>
  <input type="submit" value="Submit">
</form> 

</body>
</html>
)=====";

//////////////////////////////////////////////////////////////////////////////////////
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

//<! ............... Node 0 ......................>
var N0L1 = 0;
var N0L2 = 0;
var N0L3 = 0;

//<! ............... Node 1 ......................>
var N1L1 = 0;
var N1L2 = 0;
var N1L3 = 0;

//<! ............... Node 2 ......................>
var N2L1 = 0;
var N2L2 = 0;
var N2L3 = 0;

//<! ............... Node 3 ......................>
var N3L1 = 0;
var N3L2 = 0;
var N3L3 = 0;

var connection = new WebSocket('ws://'+location.hostname+':81/');

connection.onmessage = function(readmsg) {

  var str = readmsg.data;
  console.log(str);
  if(str.search('5')>0)
  {
    console.log("we have 5 in str");
    N0L1 = parseInt(str.substring(str.indexOf("N0L1")+5,str.indexOf("N0L1")+6));
    N0L2 = parseInt(str.substring(str.indexOf("N0L2")+5,str.indexOf("N0L2")+6));
    N0L3 = parseInt(str.substring(str.indexOf("N0L3")+5,str.indexOf("N0L3")+6));

    N1L1 = parseInt(str.substring(str.indexOf("N1L1")+5,str.indexOf("N1L1")+6));
    N1L2 = parseInt(str.substring(str.indexOf("N1L2")+5,str.indexOf("N1L2")+6));
    N1L3 = parseInt(str.substring(str.indexOf("N1L3")+5,str.indexOf("N1L3")+6));  

    N2L1 = parseInt(str.substring(str.indexOf("N2L1")+5,str.indexOf("N2L1")+6));
    N2L2 = parseInt(str.substring(str.indexOf("N2L2")+5,str.indexOf("N2L2")+6));
    N2L3 = parseInt(str.substring(str.indexOf("N2L3")+5,str.indexOf("N2L3")+6));

    N3L1 = parseInt(str.substring(str.indexOf("N3L1")+5,str.indexOf("N3L1")+6));
    N3L2 = parseInt(str.substring(str.indexOf("N3L2")+5,str.indexOf("N3L2")+6));
    N3L3 = parseInt(str.substring(str.indexOf("N3L3")+5,str.indexOf("N3L3")+6));
    
    console.log(str.substring(str.indexOf("N0L1")+5,str.indexOf("N0L1")+6));
    console.log(str.substring(str.indexOf("N0L2")+5,str.indexOf("N0L2")+6));
    console.log(str.substring(str.indexOf("N0L3")+5,str.indexOf("N0L3")+6));

    console.log(str.substring(str.indexOf("N1L1")+5,str.indexOf("N1L1")+6));
    console.log(str.substring(str.indexOf("N1L2")+5,str.indexOf("N1L2")+6));
    console.log(str.substring(str.indexOf("N1L3")+5,str.indexOf("N1L3")+6));  

    console.log(str.substring(str.indexOf("N2L1")+5,str.indexOf("N2L1")+6));
    console.log(str.substring(str.indexOf("N2L2")+5,str.indexOf("N2L2")+6));
    console.log(str.substring(str.indexOf("N2L3")+5,str.indexOf("N2L3")+6));

    console.log(str.substring(str.indexOf("N3L1")+5,str.indexOf("N3L1")+6));
    console.log(str.substring(str.indexOf("N3L2")+5,str.indexOf("N3L2")+6));
    console.log(str.substring(str.indexOf("N3L3")+5,str.indexOf("N3L3")+6));
  }
  
}
connection.onopen = function(event){
  if(connection.bufferedAmount == 0){
    connection.send("Ping");
    console.log("Sending Ping Message");
  }
  else
  {
    console.log("Failed to send PING Message");
  }
}
connection.onerror = function(error) {
  console.log('Websocket Error:'+error);
}
//<! ............... Node 0......................>
function N0L1on()
{
  console.log("Node0 Load1 ON");
  N0L1 = 1;
  send_data();
}
function N0L1off()
{
  console.log("Node0 Load1 OFF");
  N0L1 = 0;
  send_data();
}
function N0L2on()
{
  console.log("Node0 Load2 ON");
  N0L2 = 1;
  send_data();
}
function N0L2off()
{
  console.log("Node0 Load2 OFF");
  N0L2 = 0;
  send_data();
}
function N0L3on()
{
  console.log("Node0 Load3 ON");
  N0L3 = 1;
  send_data();
}
function N0L3off()
{
  console.log("Node0 Load3 OFF");
  N0L3 = 0;
  send_data();
}

//<! ............... Node 1 ......................>
function N1L1on()
{
  console.log("Node1 Load1 ON");
  N1L1=1;
  send_data();
}
function N1L1off()
{
  console.log("Node1 Load1 OFF");
  N1L1=0;
  send_data();
}
function N1L2on()
{
  console.log("Node1 Load2 ON");
  N1L2=1;
  send_data();
}
function N1L2off()
{
  console.log("Node1 Load2 OFF");
  N1L2=0;
  send_data();
}
function N1L3on()
{
  console.log("Node1 Load3 ON");
  N1L3=1;
  send_data();
}
function N1L3off()
{
  console.log("Node1 Load3 OFF");
  N1L3=0;
  send_data();
}

//<! ............... Node 2 ......................>
function N2L1on()
{
  console.log("Node2 Load1 ON");
  N2L1=1;
  send_data();
}
function N2L1off()
{
  console.log("Node2 Load1 OFF");
  N2L1=0;
  send_data();
}
function N2L2on()
{
  console.log("Node2 Load2 ON");
  N2L2=1;
  send_data();
}
function N2L2off()
{
  console.log("Node2 Load2 OFF");
  N2L2=0;
  send_data();
}
function N2L3on()
{
  console.log("Node2 Load3 ON");
  N2L3=1;
  send_data();
}
function N2L3off()
{
  console.log("Node2 Load3 OFF");
  N2L3=0;
  send_data();
}

//<! ............... Node 3 ......................>
function N3L1on()
{
  console.log("Node3 Load1 ON");
  N3L1=1;
  send_data();
}
function N3L1off()
{
  console.log("Node3 Load1 OFF");
  N3L1=0;
  send_data();
}
function N3L2on()
{
  console.log("Node3 Load2 ON");
  N3L2=1;
  send_data();
}
function N3L2off()
{
  console.log("Node2 Load2 OFF");
  N3L2=0;
  send_data();
}
function N3L3on()
{
  console.log("Node2 Load3 ON");
  N3L3=1;
  send_data();
}
function N3L3off()
{
  console.log("Node2 Load3 OFF");
  N3L2=0;
  send_data();
}
function send_data()
{
     var full_data = {
    "Type":1,
    "N0L1":N0L1,
    "N0L2":N0L2,
    "N0L3":N0L3,
    "N1L1":N1L1,
    "N1L2":N1L2,
    "N1L3":N1L3,
    "N2L1":N2L1,
    "N2L2":N2L2,
    "N2L3":N2L3,
    "N3L1":N3L1,
    "N3L2":N3L2,
    "N3L3":N3L3
  };
  connection.send(JSON.stringify(full_data));
}
</script>
<body >
<!-----text alignment and text color combinedly not workig for this H1(heading 1)---->
<h1 >Control Your Devices</h1>
<a href="/"> <h4  title="Go back"> <strong> <i> <-- Go Back</i> </strong> </h4> </a>
<h2> Node0 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N0L1on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N0L1off()">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N0L2on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N0L2off()">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N0L3on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N0L3off()">Off</button> </h3>
<h2> Node1 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N1L1on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N1L1off()">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N1L2on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N1L2off()">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N1L3on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N1L3off()">Off</button> </h3>
<h2> Node2 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N2L1on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N2L1off()">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N2L2on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N2L2off()">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N2L3on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N2L3off()">Off</button> </h3>
<h2> Node3 </h2>
<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N3L1on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N3L1off()">Off</button> </h3>
<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N3L2on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N3L2off()">Off</button> </h3>
<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="N3L3on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="N3L3off()">Off</button> </h3>
</body>
</html>
)=====";
//////////////////////////////////////////////////////////////////////////////////////

char* ssid = "ESP32";
char* password = "123456789";

AsyncWebServer server(80); // server port 80
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        websockets.sendTXT(num, "Connected from server");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      //Serial.println(message);

      if(message == "Ping")
      {
        // send data to all connected clients

        String msgg = "{Type:";
          msgg+="5,";
          
          msgg+="N0L1:";
          msgg+=String(EEPROM.read(node0load1));
          msgg+=",N0L2:";
          msgg+=String(EEPROM.read(node0load2));
          msgg+=",N0L3:";
          msgg+=String(EEPROM.read(node0load3));
          
          msgg+=",N1L1:";
          msgg+=String(EEPROM.read(node1load1));
          msgg+=",N1L2:";
          msgg+=String(EEPROM.read(node1load2));
          msgg+=",N1L3:";
          msgg+=String(EEPROM.read(node1load3));
          
          msgg+=",N2L1:";
          msgg+=String(EEPROM.read(node2load1));
          msgg+=",N2L2:";
          msgg+=String(EEPROM.read(node2load2));
          msgg+=",N2L3:";
          msgg+=String(EEPROM.read(node2load3));
          
          msgg+=",N3L1:";
          msgg+=String(EEPROM.read(node3load1));
          msgg+=",N3L2:";
          msgg+=String(EEPROM.read(node3load2));
          msgg+=",N3L3:";
          msgg+=String(EEPROM.read(node3load3));

          msgg+= "}";
         websockets.broadcastTXT(msgg); 
      }
      else
      {
      DynamicJsonDocument doc(516);
      // deserialize the data
      DeserializationError error = deserializeJson(doc, message);
      // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }
      node[0].load1 = doc["N0L1"];
      node[0].load2 = doc["N0L2"];
      node[0].load3 = doc["N0L3"];

      node[1].load1 = doc["N1L1"];
      node[1].load2 = doc["N1L2"];
      node[1].load3 = doc["N1L3"];

      node[2].load1 = doc["N2L1"];
      node[2].load2 = doc["N2L2"];
      node[2].load3 = doc["N2L3"];

      node[3].load1 = doc["N3L1"];
      node[3].load2 = doc["N3L2"];
      node[3].load3 = doc["N3L3"];
      
/**************************** Node 0 *******************************************/
      if(node[0].load1 != EEPROM.read(node0load1))
      {
        EEPROM.write(node0load1,node[0].load1);
        EEPROM.commit();
        Serial.print("node[0].load1:"); Serial.println(node[0].load1);
        digitalWrite(relay1,node[0].load1);
      }      
      if(node[0].load2 != EEPROM.read(node0load2))
      {
        EEPROM.write(node0load2,node[0].load2);
        EEPROM.commit();
        Serial.print("node[0].load2:"); Serial.println(node[0].load2);
        digitalWrite(relay2,node[0].load2);
      }
      if(node[0].load3 != EEPROM.read(node0load3))
      {
        EEPROM.write(node0load3,node[0].load3);
        EEPROM.commit();
        Serial.print("node[0].load3:"); Serial.println(node[0].load3);
        digitalWrite(relay3,node[0].load3);
      }


/**************************** Node 1 *******************************************/
      if(node[1].load1 != EEPROM.read(node1load1))
      {
        EEPROM.write(node1load1,node[1].load1);
        EEPROM.commit();
        Serial.print("node[1].load1:"); Serial.println(node[1].load1);
      }
      if(node[1].load2 != EEPROM.read(node1load2))
      {
        EEPROM.write(node1load2,node[1].load2);
        EEPROM.commit();
        Serial.print("node[1].load2:"); Serial.println(node[1].load2);
      }
      if(node[1].load3 != EEPROM.read(node1load3))
      {
        EEPROM.write(node1load3,node[1].load3);
        EEPROM.commit();
        Serial.print("node[1].load3:"); Serial.println(node[1].load3);
      }


/**************************** Node 2 *******************************************/
      if(node[2].load1 != EEPROM.read(node2load1))
      {
        EEPROM.write(node2load1,node[2].load1);
        EEPROM.commit();
        Serial.print("node[2].load1:"); Serial.println(node[2].load1);
      }
      if(node[2].load2 != EEPROM.read(node2load2))
      {
        EEPROM.write(node2load2,node[2].load2);
        EEPROM.commit();
        Serial.print("node[2].load2:"); Serial.println(node[2].load2);
      }
      if(node[2].load3 != EEPROM.read(node2load3))
      {
        EEPROM.write(node2load3,node[2].load3);
        EEPROM.commit();
        Serial.print("node[2].load3:"); Serial.println(node[2].load3);
      }


/**************************** Node 3 *******************************************/
      if(node[3].load1 != EEPROM.read(node3load1))
      {
        EEPROM.write(node3load1,node[3].load1);
        EEPROM.commit();
        Serial.print("node[3].load1:"); Serial.println(node[3].load1);
      }
      if(node[3].load2 != EEPROM.read(node3load2))
      {
        EEPROM.write(node3load2,node[3].load2);
        EEPROM.commit();
        Serial.print("node[3].load2:"); Serial.println(node[3].load2);
      }
      if(node[3].load3 != EEPROM.read(node3load3))
      {
        EEPROM.write(node3load3,node[3].load3);
        EEPROM.commit();
        Serial.print("node[3].load3:"); Serial.println(node[3].load3);
      }
      
      // send data to all connected clients
      websockets.broadcastTXT(message);
      }
    }
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

  if (MDNS.begin("ESP")) { //esp.local/
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
  });
  server.on("/automate", [](AsyncWebServerRequest * request)
  { 
    request->send_P(200, "text/html", automation_webpage);
    //request->send_P(200, "text/plain", "UPDATE PAGE RESULTS");
  });
  
  server.on("/cred", [](AsyncWebServerRequest * request)
  { 
    request->send_P(200, "text/html", cred_webpage);
  });
/*****************************************************************/
  server.on("/sta_cred", [](AsyncWebServerRequest * request)
  {
    String qsid,qpas;
    int no_of_params = request->params();
    Serial.print("no of parameters = ");
    Serial.println(no_of_params);

      AsyncWebParameter* p = request->getParam(0);
      AsyncWebParameter* q = request->getParam(1);
      Serial.print(p->name());Serial.print(" : ");Serial.println(p->value());
      Serial.print(q->name());Serial.print(" : ");Serial.println(q->value());
       qsid = p->value();
       qpas = q->value(); 
       Serial.print("qsid : ");Serial.println(qsid);
       Serial.print("qpas : ");Serial.println(qpas);
     if(qsid.length() >0 && qpas.length() >0)
     {
      Serial.println("cleaning fields");
      for(uint8_t i = locof_sta_ssid; i < (locof_sta_ssid+20);i++)
      {
        EEPROM.write(i,0);
        EEPROM.commit();
      }
      for(uint8_t i = locof_sta_pass; i < (locof_sta_pass+20);i++)
      {
        EEPROM.write(i,0);
        EEPROM.commit();
      }
     Serial.println("cleaning Completed...!");
     Serial.println("writing eeprom STA_ssid:");
        for (int k=0, i = locof_sta_ssid; i < (locof_sta_ssid+qsid.length()); k++,i++)
        {
          EEPROM.write(i, qsid[k]);
          Serial.print("Wrote: ");
          Serial.println(qsid[k]);
        }
        EEPROM.commit();
        Serial.println("writing eeprom pass:");

        for (int k=0, i = locof_sta_pass; i < (locof_sta_pass+qpas.length()); k++,i++)
        {
          EEPROM.write(i, qpas[k]);
          Serial.print("Wrote: ");
          Serial.println(qpas[k]);
        }
        EEPROM.commit();
//        ESP.reset();

      request->send_P(200, "text/plain", "WiFi Detailes Sent Successfully");
     }
     else
     {
      Serial.println("Sending 404");
      request->send_P(200, "text/plain", "Something Went Wrong Please try again");
     }

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


  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  /********** NODE 0  ***************/
//  server.on("/node0load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load1,1);
//    EEPROM.commit();
//    node[0].load1 = 1;
//    Serial.print("node[0].load1:"); Serial.println(node[0].load1);
//    digitalWrite(relay1,node[0].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node0load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load1,0);
//    EEPROM.commit();
//    node[0].load1 = 0;
//    Serial.print("node[0].load1:"); Serial.println(node[0].load1);
//    digitalWrite(relay1,node[0].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
/////////////////////////////////////////////////////////////////////  
//  server.on("/node0load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load2,1);
//    EEPROM.commit();
//    node[0].load2 = 1;
//    Serial.print("node[0].load2:"); Serial.println(node[0].load2);
//    digitalWrite(relay2,node[0].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node0load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load2,0);
//    EEPROM.commit();
//    node[0].load2 = 0;
//    Serial.print("node[0].load2:"); Serial.println(node[0].load2);
//    digitalWrite(relay2,node[0].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  ///////////////////////////////////////////////////////////////////
//  server.on("/node0load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load3,1);
//    EEPROM.commit();
//    node[0].load3 = 1;
//    Serial.print("node[0].load3:"); Serial.println(node[0].load3);
//    digitalWrite(relay3,node[0].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node0load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node0load3,0);
//    EEPROM.commit();
//    node[0].load3 = 0;
//    Serial.print("node[0].load3:"); Serial.println(node[0].load3);
//    digitalWrite(relay3,node[0].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  /////////////////////////////////////////////////////////
//  
//  
//  /********** NODE 1 ***************/
//  server.on("/node1load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load1,1);
//    EEPROM.commit();
//    node[1].load1 = 1;
//    Serial.print("node[1].load1:"); Serial.println(node[1].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node1load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load1,0);
//    EEPROM.commit();
//    node[1].load1 = 0;
//    Serial.print("node[1].load1:"); Serial.println(node[1].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
/////////////////////////////////////////////////////////////////////  
//  server.on("/node1load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load2,1);
//    EEPROM.commit();
//    node[1].load2 = 1;
//    Serial.print("node[1].load2:"); Serial.println(node[1].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node1load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load2,0);
//    EEPROM.commit();
//    node[1].load2 = 0;
//    Serial.print("node[1].load2:"); Serial.println(node[1].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  ///////////////////////////////////////////////////////////////////
//  server.on("/node1load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load3,1);
//    EEPROM.commit();
//    node[1].load3 = 1;
//    Serial.print("node[1].load3:"); Serial.println(node[1].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node1load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node1load3,0);
//    EEPROM.commit();
//    node[1].load3 = 0;
//    Serial.print("node[1].load3:"); Serial.println(node[1].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  /////////////////////////////////////////////////////////
//
//  
//  /********** NODE 2 ***************/
//  server.on("/node2load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load1,1);
//    EEPROM.commit();
//    node[2].load1 = 1;
//    Serial.print("node[2].load1:"); Serial.println(node[2].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node2load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load1,0);
//    EEPROM.commit();
//    node[2].load1 = 0;
//    Serial.print("node[2].load1:"); Serial.println(node[2].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
/////////////////////////////////////////////////////////////////////  
//  server.on("/node2load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load2,1);
//    EEPROM.commit();
//    node[2].load2 = 1;
//    Serial.print("node[2].load2:"); Serial.println(node[2].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node2load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load2,0);
//    EEPROM.commit();
//    node[2].load2 = 0;
//    Serial.print("node[2].load2:"); Serial.println(node[2].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  ///////////////////////////////////////////////////////////////////
//  server.on("/node2load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load3,1);
//    EEPROM.commit();
//    node[2].load3 = 1;
//    Serial.print("node[2].load3:"); Serial.println(node[2].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node2load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node2load3,0);
//    EEPROM.commit();
//    node[2].load3 = 0;
//    Serial.print("node[2].load3:"); Serial.println(node[2].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  /////////////////////////////////////////////////////////
//  
//  /********** NODE 3 ***************/
//  server.on("/node3load1/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load1,1);
//    EEPROM.commit();
//    node[3].load1 = 1;
//    Serial.print("node[3].load1:"); Serial.println(node[3].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node3load1/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load1,0);
//    EEPROM.commit();
//    node[3].load1 = 0;
//    Serial.print("node[3].load1:"); Serial.println(node[3].load1);
//    request->send_P(200, "text/html", automation_webpage);
//  });
/////////////////////////////////////////////////////////////////////  
//  server.on("/node3load2/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load2,1);
//    EEPROM.commit();
//    node[3].load2 = 1;
//    Serial.print("node[3].load2:"); Serial.println(node[3].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node3load2/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load2,0);
//    EEPROM.commit();
//    node[3].load2 = 0;
//    Serial.print("node[3].load2:"); Serial.println(node[3].load2);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  ///////////////////////////////////////////////////////////////////
//  server.on("/node3load3/on", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load3,1);
//    EEPROM.commit();
//    node[3].load3 = 1;
//    Serial.print("node[3].load3:"); Serial.println(node[3].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  
//  server.on("/node3load3/off", HTTP_GET, [](AsyncWebServerRequest * request)
//  { 
//    EEPROM.write(node3load3,0);
//    EEPROM.commit();
//    node[3].load3 = 0;
//    Serial.print("node[3].load3:"); Serial.println(node[3].load3);
//    request->send_P(200, "text/html", automation_webpage);
//  });
//  /////////////////////////////////////////////////////////
server.onNotFound(notFound);
  server.begin();  // it will start webserver
    websockets.begin();
  websockets.onEvent(webSocketEvent);
  Serial.println("Server Started.....");
}
void loop(void)
{
   websockets.loop();
}
