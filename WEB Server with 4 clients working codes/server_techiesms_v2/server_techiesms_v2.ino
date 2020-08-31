

  status[0][0]=0;
  console.log("Node0-Load1: OFF");
  send_data();
}

function n0Load2on()
{
  status[0][1]=1;
  console.log("Node0-Load2: ON");
  send_data();
}

function n0Load2off()
{
  status[0][1]=0;
  console.log("Node0-Load2: OFF");
  send_data();
}
function n0Load3on()
{
  status[0][2]=1;
  console.log("Node0-Load3: ON");
  send_data();
}
function n0Load3off()
{
  
  status[0][2]=0;
  console.log("Node0-Load3: OFF");
  send_data();
}

function send_data ()
{
  var full_data = {"Load1":status[0][0],"Load2":status[0][1],"Load3":status[0][2]}
  console.log(full_data);
  connection.send(full_data);

}


</script>
<body >

<!-----text alignment and text color combinedly not workig for this H1(heading 1)---->
<h1 >Control Your Devices</h1>


<!----testing purpose for go back---->
<a href="'http://'+location.hostname+'/'"> <h4  title="Go back"> <strong> <i> <-- Go Back</i> </strong> </h4> </a>


<!-- NODE0 -->
<h2> Node0 </h2>

<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="n0Load1on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="n0Load1off()">Off</button> </h3>

<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="n0Load2on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="n0Load2off()">Off</button> </h3>


<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="n0Load3on()">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="n0Load3off()">Off</button> </h3>


<!-- NODE1 -->
<h2> Node1 </h2>

<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load1/off'">Off</button> </h3>

<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load2/off'">Off</button> </h3>


<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node1load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node1load3/off'">Off</button> </h3>

<!-- NODE2 -->
<h2> Node2 </h2>

<h3> Load 1&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load1/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load1/off'">Off</button> </h3>

<h3> Load 2&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load2/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load2/off'">Off</button> </h3>


<h3> Load 3&nbsp;&nbsp; - &nbsp;&nbsp; <button onclick="window.location = 'http://'+location.hostname+'/node2load3/on'">On</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button onclick="window.location = 'http://'+location.hostname+'/node2load3/off'">Off</button> </h3>

<!-- NODE3 -->
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
WebSocketsServer webSocket(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
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
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Server Started.....");
}


void loop(void)
{
  webSocket.loop();
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected from Server");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
String message = String((char*)( payload));
      Serial.println(message);

      
     DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
             webSocket.broadcastTXT(payload);
            break;
     
    }
}
