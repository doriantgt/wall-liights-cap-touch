int value = 0;
//lots of global variables in here
void globalJsonAlarms(Alarm alarmData[], int numOfAlarms ){
//WiFi.mode(WIFI_STA); //restart wifi

    delay(5000);
    ++value;

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&value=";
    url += value;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("POST ") + "/arduino/practice/getJsonData.php" + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                  "Content-Type: application/x-www-form-urlencoded" + "\r\n" + 
                  "Content-Length: 20" + "\r\n\r\n" + //change content lenght for post
                  "userName=samepass" + "\r\n\r\n"  //maybe make dynamic
                 // "userName=samepass&password=password" + "\r\n\r\n");
                  "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  // copy paste you json code to in to the textArea on this page.
const size_t capacity = JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(2) + 7*JSON_OBJECT_SIZE(4) + 180;
DynamicJsonDocument doc(capacity);  



  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  
  Serial.println(F("Response:"));
  String namer = doc["name1"].as<char*>();
  Serial.println(namer);
  
  
  int alarmCount = doc["count"]; // 7
  if(alarmCount>7)alarmCount=7; //maximum alarmsJson == 7

JsonArray alarmsJson = doc["alarms"];  //name of my json array

//jsonStrings alarmStrings[7];
for(int i=0; i<alarmCount; i++){
 
 // const char* temp = alarmsJson[i]["alarmTime"];
 // alarmStrings[i].alarmTime = temp;
   alarmData[i].alarmTime =  StringTimeDaySeconds(alarmsJson[i]["alarmTime"])*1000;
   alarmData[i].duration = alarmsJson[i]["duration"].as<float>() * 1000;

    alarmData[i].alarmType = alarmsJson[i]["alarmType"].as<uint16_t>();
   alarmData[i].autoOff = alarmsJson[i]["autoOff"].as<bool>();

  //  alarmData[i].duration =  alarmStrings[i].duration.toInt()*1000;
   // alarmData[i].alarmType =  alarmsJson[i]["alarmType"].toInt()*1000;

     Serial.println( alarmData[i].alarmTime );
     Serial.println( alarmData[i].duration);
     Serial.println( alarmData[i].alarmType);
     Serial.println( alarmData[i].autoOff);



}




   /*
   Serial.println(root_0_alarmTime);
   Serial.println(root_0_alarmTime.charAt(1));
   Serial.println(StringTimeDaySeconds(root_0_alarmTime));
   Serial.println(root_0_alarmTime.charAt(3));*/

  /*
  Serial.println(doc["sensor"].as<char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);
*/
  // Disconnect
/* //print all of the stuff
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
*/
  client.stop();

  /*
WiFi.mode(WIFI_OFF);
WiFi.forceSleepBegin();
delay(1);

 WiFi.forceSleepWake();
  delay(1);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
*/
    Serial.println();
    Serial.println("closing connection");
}

//-----------------------------------------------------------------------
//-below not used original----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*
 //get rid of this
struct jsonStrings{ 

String id;
String userName;
String alarmType;
String duration;
String alarmTime;
String autoOff;
  
};
*/
void globalJson(){// first 1 without data returned
//WiFi.mode(WIFI_STA); //restart wifi

    delay(5000);
    ++value;

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&value=";
    url += value;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("POST ") + "/arduino/practice/getJsonData.php" + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                  "Content-Type: application/x-www-form-urlencoded" + "\r\n" + 
                  "Content-Length: 20" + "\r\n\r\n" + //change content lenght for post
                  "userName=samepass" + "\r\n\r\n");  //maybe make dynamic
                 // "userName=samepass&password=password" + "\r\n\r\n");
                  //"Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  // copy paste you json code to in to the textArea on this page.
const size_t capacity = JSON_ARRAY_SIZE(7) + 8*JSON_OBJECT_SIZE(2) + 130;
DynamicJsonDocument doc(capacity);  



  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  
  Serial.println(F("Response:"));
  String namer = doc["name1"].as<char*>();
  Serial.println(namer);
  
  
  int alarmCount = doc["count"]; // 7
  if(alarmCount>7)alarmCount=7; //maximum alarmsJson == 7

JsonArray alarmsJson = doc["alarmsJson"];

//jsonStrings alarmStrings[7];
for(int i=0; i<alarmCount; i++){
 /*
  const char* temp = alarmsJson[i]["alarmTime"];
   alarmStrings[i].alarmTime =temp;
   alarmStrings[i].alarmType = alarmsJson[i]["alarmType"].as<String>();

     Serial.println( alarmStrings[i].alarmTime);
     Serial.println( StringTimeDaySeconds(alarmsJson[i]["alarmTime"]));
     Serial.println( alarmStrings[i].alarmType);

*/
}




   /*
   Serial.println(root_0_alarmTime);
   Serial.println(root_0_alarmTime.charAt(1));
   Serial.println(StringTimeDaySeconds(root_0_alarmTime));
   Serial.println(root_0_alarmTime.charAt(3));*/

  /*
  Serial.println(doc["sensor"].as<char*>());
  Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);
*/
  // Disconnect
/* //print all of the stuff
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
*/
  client.stop();

  /*
WiFi.mode(WIFI_OFF);
WiFi.forceSleepBegin();
delay(1);

 WiFi.forceSleepWake();
  delay(1);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
*/
    Serial.println();
    Serial.println("closing connection");
}
