#include <Wire.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "LedControl.h"
#include "MyLedControl.h"

#define buffersize 10
#define detect_dist  10
#define prepare_delay 3000
#define deviceNum 4


const char* ssid = "SOCLAB";
const char* password = "1004soc1004";
//const char* ssid = "tien";
//const char* password = "pppppppp";

String postData = "";
String payload = "";
String userlog = "";
String website = "";

float volts;
int httpCode;
int dist ;
int distBuffer[buffersize] = {0};
int bCount = 0;
byte gameStart =0;
 

unsigned long clrTimer = 0;
unsigned long gameStart_timer=0;
unsigned long interval_time = 0;
unsigned long dist_timer = 0;
unsigned long flag2_timer = 0;
unsigned long getdata_timer=0;

//LedControl LEDarr=LedControl(12,13,15,1); // LedControl(DIN,CLK,CS,DIVCEnum)->(D6,D7,D8)
HTTPClient http; 
WiFiClient client;            
JSONVar myObject;

void access_dataBase(String data, String web){
  postData = data;   
  payload = "";
  http.begin(web);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
  httpCode = http.POST(postData); 
  payload = http.getString();
//  Serial.println(payload);    
  http.end(); 
}

void setup(){
//  pinMode(5,OUTPUT);
//  digitalWrite(5,HIGH);
  
  Serial.begin(115200);
  Serial.println("Initializing...");
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  postData = "ID=1";
  website = "http://140.120.14.51/device/getdata.php";
  access_dataBase(postData, website);
  myObject = JSON.parse(payload);
  gameStart = int(myObject["flag2"]);
  userlog = (const char*)myObject["user_log"];
  
  LEDarray.shutdown(0,false);  // 關閉省電模式
  LEDarray.setIntensity(0,0);  // 設定亮度為 0 (介於0~15之間)
  LEDarray.clearDisplay(0); 
  delay(2000);
}

void loop(){
  if(WiFi.status()== WL_CONNECTED) {
      display_num(deviceNum);
      volts = analogRead(A0)*0.0048828125;  // value from sensor * (5/1024) 
      dist = 29.988 * pow(volts, -1.173); // 10cm-80cm, 1080, Sharp IR GP2Y0A21SK0F
//      dist = 12.08 * pow(volts , -1.058);// 4cm-30cm,430
//      dist =  60.374 * pow(volts , -1.16);// 20cm-150cm,20150
      Serial.print(dist); 
      Serial.print("CM, ");   
//        postData = "ID=1&DISTANCE_" + Sring(deviceNum) + "=" + String(dist); 
//        website = "http://140.120.14.51/device/update_"+ String(deviceNum)" + ".php";
//        access_dataBase(postData, website);
     
//      if (millis()- dist_timer >= 50){
        if(dist <= detect_dist){ 
          distBuffer[bCount] = dist;
          bCount++;
          if(bCount == 5) interval_time = millis() - gameStart_timer;
          LEDarray.clearDisplay(0); 
          clrTimer = millis();
          dist_timer = millis();
        }       
//      }
      if(millis()> clrTimer + 2000){
        for(int i=0; i<buffersize; i++){
          distBuffer[i] = 0;  
        }
        bCount =0;
        clrTimer = millis();
      }
      
      String stmp="";
      for(int i=0; i< buffersize; i++){
          stmp += distBuffer[i];
//          Serial.print(distBuffer[i]);Serial.print(", ");
          stmp += ',';
      }
        
        Serial.print("distBuffer: ");
        Serial.println(stmp);
      if(millis() - getdata_timer > 1000){
        postData = "ID=1";
        website = "http://140.120.14.51/device/getdata.php";
        access_dataBase(postData, website);
        myObject = JSON.parse(payload);
        
        gameStart = int(myObject["flag2"]);
        getdata_timer = millis();
      }
        
        if(gameStart) {
          gameStart_timer = millis();
          delay(prepare_delay);    
          postData = "FLAG2=0"; 
          website = "http://140.120.14.51/device/update_flag2.php";
          access_dataBase(postData, website);     
          gameStart = 0;
          postData = "";
        }
        userlog = "";
        
        if(bCount == buffersize){
          String s = "L" + String(deviceNum) + "s";
          Serial.print("\ndistance sequence :[");
          for(int i=0; i< buffersize; i++){
            s += distBuffer[i];
  //          Serial.print(distBuffer[i]);Serial.print(", ");
            s += '/';
        }
        bCount =0;
        Serial.print(s);
        Serial.println(']');
        userlog = (const char*)myObject["user_log"];
        userlog += "t";
        userlog += interval_time;
        userlog += s;
        userlog += ',';
        
        postData = "USER_LOG=" + userlog ; 
        website = "http://140.120.14.51/device/update_userLog.php";
        access_dataBase(postData, website);     
        LEDarray.clearDisplay(0); 
        display_num(999);
        delay(1000);      
        }
  }
}
