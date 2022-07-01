/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/
#define FASTLED_ALLOW_INTERRUPTS 0
#include <iostream>
using namespace std;
 
#include <FastLED.h>
#include "FastLED_RGBW.hpp"
#include "stripFunctions.h"
#include "dayMillis.h"

//#include "DscreenAlarmClock.hpp"

#include "alarmClockFunctions.hpp"
//#include "multiPressGroup.h" 
#include "defualtComparesCodes.hpp"

#include <WiFi.h> //web stuff
#include "time.h"
#include <ArduinoJson.h>


#include <SPIFFS.h>

#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <DNSServer.h>

DNSServer dnsServer;
AsyncWebServer server(80);


#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <fourColor.hpp>

#define NUM_LEDS 132
#define DATA_PIN 19

class DscreenAlarmClock { public: //steps, change Title, main argument,and constructor
uint16_t currentChannel=0;uint16_t numChannels;

void (* screenMain)(DscreenAlarmClock &ThisScreen,DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[]);
void (** channels)(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[]);

DscreenAlarmClock(void (*channelsIN[])(DayMillisSeconds timestuff,RGBWstrip stripIn,ButtonInterpreter buttonsArrayIn[]), int NumChannelsIN){
    numChannels = NumChannelsIN;  channels = channelsIN;
}};


CRGBW leds[NUM_LEDS];
CRGB rgbLeds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];
 byte globalBrightness=127;//low brightness
RGBWstrip stripUtil(leds , NUM_LEDS);

 
Alarm testAlarm; 
DayMillisSeconds internalTime; //my timeStuff

ButtonInterpreter buttonsArray[3] = { ButtonInterpreter(defualtPressCodes, 3), //bottom 0 //if else tree in main determins this
                                      ButtonInterpreter(defualtPressCodes, 3), //right 2
                                      ButtonInterpreter(defualtPressCodes, 3)}; //left 3
                                
//uint8_t ButtonInterpreter::STATICnumOfButtons = 3;  
/*
ButtonInterpreter leftButton(defualtPressCodes, 3); //3
ButtonInterpreter rightButton(defualtPressCodes, 3);  //2
ButtonInterpreter bottomButton(defualtPressCodes, 3); //0 */
bool pin0State=false;bool pin2State=false;bool pin3State=false;


void fillStripRGBW(RGBW inputColor, byte starter, byte ender);
int16_t scale8bitAccurate(uint16_t input ,uint16_t startWidth );
void globalJsonAlarms(Alarm alarmData[], int numOfAlarms );
Alarm globalJsonAlarms();
uint32_t printLocalTime(uint32_t millisTime); //returns millis time;
uint32_t StringTimeDaySeconds(String stringTime);


//#include <FastLED.h>


//const char* ssid     = "wookwifi";
//const char* password = "Bassnectar6!";
 char ssid    [64] = "artholez";
 char password [64] = "dorian123";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600-3600; //mt - daylightSavings
const int   daylightOffset_sec = 0; //day light savings 0 or 3600// didn't work


uint32_t millisTime;
uint32_t lastMillisTime;
uint32_t millisTimeOffset;


const char* host = "ec2-18-237-26-31.us-west-2.compute.amazonaws.com"; //json
const char* streamId   = "....................";          
const char* privateKey = "....................";          //json

TaskHandle_t Task1;
TaskHandle_t Task2;

///////////////////////////////////////////////////////////////////////////////
//captive stuff
///////////////////////////////////////////////////////////////////////////////

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);  //this is why we reload the same page  
    }
  </script></head><body>
  <form action="/post" target="hidden-form" method="post">
    wifi name (current value %wifi%): <input type="text" name="wifi">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/post" target="hidden-form" method="post">
    wifi password (current value %password%): <input type="text" name="password">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  current time = %time%

  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>
)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "wifi"){
  String tempString =  readFile(SPIFFS, "/wifi.txt");
  tempString.toCharArray(ssid,tempString.length()+1);
    return ssid;
  }
  else if(var == "password"){
      String tempString =  readFile(SPIFFS, "/password.txt");
    tempString.toCharArray(password,tempString.length()+1);
    return tempString;
  }  else if(var == "time"){
   
    
    return readFile(SPIFFS, "/time.txt");
  }

  return String();
}


class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {
 

   server.on("/index", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/post", HTTP_ANY, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam("wifi", true)) {
      inputMessage = request->getParam("wifi", true)->value();
      writeFile(SPIFFS, "/wifi.txt", inputMessage.c_str());
    }
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam("password", true)) {
      inputMessage = request->getParam("password", true)->value();
      writeFile(SPIFFS, "/password.txt", inputMessage.c_str());
    }
    // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
  
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  
  server.onNotFound(notFound);

  
    }
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s", WiFi.softAPIP().toString().c_str() );
    response->printf("/index'>this link</a> instead</p>");
    response->print("</body></html>");
    request->send(response);
   // request->send_P(200, "text/html", index_html, processor);

  }
};
///////////////////////////////////////////////////////////////////////////////
// END captive stuff
///////////////////////////////////////////////////////////////////////////////


void setup() {

  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B,DATA_PIN,BRG>(rgbLeds, NUM_LEDS); //RGB
 // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS)); //RGBW ////
  stripUtil.fillwith( CRGBW(0, 0, 0, 100));

  FastLED.setBrightness(globalBrightness);
  FastLED.show();
 
 if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    
      String tempString =  readFile(SPIFFS, "/wifi.txt");
  tempString.toCharArray(ssid,tempString.length()+1);
     tempString =  readFile(SPIFFS, "/password.txt");
  tempString.toCharArray(password,tempString.length()+1);
  //your other setup stuff...

  
  WiFi.softAP("esp-Alarm");
  
  WiFi.mode(WIFI_MODE_APSTA);//from soft ap and station

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();



//---dualCore setup
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



}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  delay(1000);

  
  //----wifi setup  //// 
 //{
/*
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

//---time setup
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//}
*/

  Alarm alarmz[7];
  globalJsonAlarms(alarmz, 7); //get json from my website

   int argumentnotused;
   millisTime = printLocalTime( argumentnotused  ); //get millisTime
   internalTime.setDayOffset( millis(), millisTime);

/////////////
// TURNED OFF WIFI
WiFi.mode(WIFI_OFF);
btStop();
///////
 bool setuper =true;
  for (;;) {
Serial.println("wifi off");
delay(10000);
/*
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }else{
  Serial.println(" CONNECTED");
  


    
 //      digitalWrite(led1, HIGH);  delay(1000);digitalWrite(led1, LOW); delay(1000);

 delay(1000); //required to prevent crashing if no real code
  
   millisTime =  internalTime.currentTime(millis()); //

   Serial.println(millisTime);

    
    delay(2000);
  
  if(millisTime < lastMillisTime){ //midnight rollover
   int argumentnotused;
   millisTime = printLocalTime( argumentnotused  ); //get millisTime
   internalTime.setDayOffset( millis(), millisTime);

   millisTime =  internalTime.currentTime(millis()); //
    
     delay(10000); //at midnight wait so you don't ask twice
       globalJsonAlarms(alarmz, 7); //get json from my website


  }
  lastMillisTime = millisTime;


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime(argumentnotused);

  }
   WiFi.disconnect(true);
  
  
  */
  }
}
///end task1

void ambientLightingMAIN(DscreenAlarmClock &thisScreen,DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[]);

void mode1(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );
void mode2(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );
void mode3(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );
void mode4(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );
void mode5(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );
void ChasingRainbowsB(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[] );

void (*ambientLightingMODES[])(DayMillisSeconds timestuff,RGBWstrip stripIn, ButtonInterpreter buttonsArrayIn[]  ) 
= {mode1,mode2,mode3,mode4,mode5,ChasingRainbowsB}; //put the modes in an array. Very readable what order the modes are in.

//construct our screen
DscreenAlarmClock ambientScreen(  ambientLightingMODES   ,sizeof(ambientLightingMODES)/sizeof(void *)); //divided by the size of a function pointer

byte globalSceenSelector = 2; //0 off, 1 alarams, 2 ambient



void Task2code( void * pvParameters ) {
   Serial.print("Task2 running on core "); Serial.println(xPortGetCoreID());
  ambientScreen.screenMain =  ambientLightingMAIN;
  
  for (;;) {

     millisTime =  internalTime.currentTime(millis());

        byte threshold =20;
          
         
          if ( touchRead(T4) < threshold){
             delayMicroseconds(5); //Remove Ghost press
          if ( touchRead(T4) < threshold){     
          pin0State = true; cout << "|" <<  touchRead(T4) ; }}else{  pin0State = false; }
         
    
          if ( touchRead(T2) < threshold){
             delayMicroseconds(5); 
          if ( touchRead(T2) < threshold){
          pin2State = true;  cout << "-";}}else{  pin2State = false; }
          
          touchRead(T3);
          if ( touchRead(T3) < threshold){
             delayMicroseconds(5); 
          if ( touchRead(T3) < threshold){
          pin3State = true; cout << "/"; }}else{  pin3State = false; }

      
          buttonsArray[0].go(pin0State,  internalTime.millisTime); //bottom
          buttonsArray[1].go(pin2State,  internalTime.millisTime); //left 
          buttonsArray[2].go(pin3State,  internalTime.millisTime); // right

        
        //  leftButton.go(pin3State,  internalTime.millisTime);  rightButton.go(pin2State,  internalTime.millisTime);  bottomButton.go(pin0State,  internalTime.millisTime);




 
    if(globalSceenSelector != 2 && clickUp.is(buttonsArray[1])){
      globalSceenSelector=2;
    }
    
    if(clickUp.is(buttonsArray[2])){

      if(globalSceenSelector==1){
        globalSceenSelector = 0; //off
      }else{ globalSceenSelector=1;}
    }

    
    if( globalSceenSelector == 0){
  CRGBW crgwfromhsv ;
    crgwfromhsv = CHSV(0, 0,0);
  stripUtil.fillwith(crgwfromhsv);
      stripUtil.leds[2].r = 16;
            stripUtil.leds[NUM_LEDS - 4].r = 16;

    }
   
   
   if( globalSceenSelector == 2){
    ambientScreen.screenMain(ambientScreen, internalTime, stripUtil, buttonsArray);
   }



   
bool hasChanged = false;
CRGBW ledsChangeCheck[NUM_LEDS] = leds;


for(int i=0; i<NUM_LEDS; i++){
if(ledsChangeCheck[i].r != leds[i].r){ hasChanged = true; break;}
if(ledsChangeCheck[i].g != leds[i].g){ hasChanged = true; break;}
if(ledsChangeCheck[i].b != leds[i].b){ hasChanged = true; break;}
if(ledsChangeCheck[i].w != leds[i].w){ hasChanged = true; break;}

}
hasChanged = true;
if(hasChanged){
//  if(ledsChangeCheck != leds){ //don't know if this works or not

  EVERY_N_MILLISECONDS( 30 ){ 
   for(int i=0; i<NUM_LEDS; i++){
  
      rgbLeds[i].r = leds[i].r; //this was to change all of RGB
      rgbLeds[i].g = leds[i].g; //this was to change all of RGB
      rgbLeds[i].b = leds[i].b; //this was to change all of RGB

   }
   
 FastLED.show(globalBrightness); }
 }

   

    
    uint16_t sunriseDuration = 10000;//ten seconds
 //  color1.w = scale8bitAccurate( millis()%sunriseDuration , sunriseDuration );

   // strip.show();
    //xSemaphoreTake( millisTimeMutex, portMAX_DELAY );
    //  xSemaphoreGive( millisTimeMutex );
    
    

  }
}

void loop() {

}
