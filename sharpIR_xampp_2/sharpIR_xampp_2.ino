#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "LedControl.h"
LedControl LEDarr=LedControl(12,13,15,1); // LedControl(DIN,CLK,CS,DIVCEnum)->(D6,D7,D8)
#define sensor A0 // Sharp IR GP2Y0A21F (4-30cm, analog)
#define buffersize 10
#define detect_dist  10


void display_num(int n){
  byte num2[8] = {                                    // 2
        0b00010000,0b00110000,0b01111111,0b11111111,
        0b01111111,0b00110000,0b00010000,0b00000000};
 byte num_all[8] = {
        0b11111111,0b11111111,0b11111111,0b11111111,
        0b11111111,0b11111111,0b11111111,0b11111111};  
 byte num_some[8] = {
        0b00000000,0b00000000,0b00111100,0b00111100,
        0b00111100,0b00111100,0b00000000,0b00000000};
  switch(n){
    case 2:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num2[j]); break;
    case 99:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_some[j]); break;
    case 999:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_all[j]); break;
  }
}
//
//const char* ssid = "SOCLAB";
//const char* password = "1004soc1004";
const char* ssid = "tien";
const char* password = "pppppppp";

String postData = "";
String payload = "";

float volts;
int dist ;
int distBuffer[buffersize] = {0};
unsigned long clrTimer = 0;
int bCount = 0;
int timer1=0;
int interval_time = 0;

unsigned long last_time = 0;
WiFiClient client;            


void setup(){
  
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
  
  LEDarr.shutdown(0,false);  // 關閉省電模式
  LEDarr.setIntensity(0,0);  // 設定亮度為 0 (介於0~15之間)
  LEDarr.clearDisplay(0); 
  delay(2000);

}
void loop(){

  if(WiFi.status()== WL_CONNECTED) {
      display_num(2);
      HTTPClient http;  //--> Declare object of class HTTPClient.
      int httpCode;     //--> Variables for HTTP return code. 
      postData = "ID=1";
      volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
     
      dist = 29.988 * pow(volts, -1.173); // 10cm-80cm, 1080
//      dist = 12.08 * pow(volts , -1.058);// 4cm-30cm,430
//      dist =  60.374 * pow(volts , -1.16);// 20cm-150cm,20150
      Serial.print(dist); Serial.print("CM, ");
      

      postData += "&DISTANCE_2=" + String(dist);  
      payload = "";
      http.begin("http://140.120.14.51/device/update_2.php");  //--> Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
      httpCode = http.POST(postData); //--> Send the request
      payload = http.getString();     //--> Get the response payload
      postData = "";
      
//      if (dist <=5) display_num(999);
//      else display_num(1);
      http.end();
      Serial.print("timer1: ");
      Serial.print(timer1);
      Serial.print("millis(): ");
      Serial.println(millis());
//      if (millis()-last_time >= 20){
      if(dist <= detect_dist){ 
        distBuffer[bCount] = dist;
        bCount++;
        if(bCount == 2) interval_time = millis() - timer1;
        LEDarr.clearDisplay(0); 
        clrTimer = millis();
        last_time = millis();
      }       
//      }
      if(millis()> clrTimer + 2000){
        bCount =0;
      }
      String stmp="";
      for(int i=0; i< buffersize; i++){
          stmp += distBuffer[i];
//          Serial.print(distBuffer[i]);Serial.print(", ");
          stmp += ',';
      }
        
        Serial.print("distBuffer: ");
        Serial.println(stmp);
      
        postData = "ID=1"; 
        payload = "";
        http.begin("http://140.120.14.51/device/getdata.php");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
        httpCode = http.POST(postData); 
        payload = http.getString();    
//        Serial.print("payload  : ");
//        Serial.println(payload);  
        http.end(); 
        JSONVar myObject = JSON.parse(payload);
        
        if(int(myObject["flag2"])) {
          timer1 = millis();
          
          postData = "FLAG2=0";
          http.begin("http://140.120.14.51/device/update_flag2.php");  //--> Specify request destination
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
          httpCode = http.POST(postData); //--> Send the request
          payload = http.getString();     //--> Get the response payload
          http.end();
          postData = "";
        }
        
      if(bCount == buffersize){
        String s = "L2s";
        Serial.print("\ndistance sequence :[");
        for(int i=0; i< buffersize; i++){
          s += distBuffer[i];
//          Serial.print(distBuffer[i]);Serial.print(", ");
          s += '/';
        }
        bCount =0;
        Serial.print(s);
        Serial.println(']');
        String userlog = myObject["user_log"];
        userlog += "t";
        userlog += interval_time;
        userlog += s;
        userlog += ',';
        http.begin("http://140.120.14.51/device/update_userLog.php");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");       
        httpCode = http.POST("USER_LOG="+userlog);
        http.end(); 
       
        LEDarr.clearDisplay(0); 
        display_num(999);
        delay(1000);
        
      }
  }
}
