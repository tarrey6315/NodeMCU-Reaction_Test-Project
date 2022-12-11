#include <Wire.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "LedControl.h"
LedControl LEDarr=LedControl(12,13,15,1); // LedControl(DIN,CLK,CS,DIVCEnum)->(D6,D7,D8)
#define buffersize 10
#define detect_dist  10
void display_num(int n){
  byte num4[8] = {                                   //  4
        0b00100000,0b01110001,0b11111011,0b01111111,
        0b00111111,0b00011111,0b00111111,0b01111111};
  byte num_all[8] = {
        0b11111111,0b11111111,0b11111111,0b11111111,
        0b11111111,0b11111111,0b11111111,0b11111111};
  switch(n){
    case 4:
       for(int j=0;j<8;j++) LEDarr.setColumn(0,j,num4[j]); break;
    case 999:
       for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_all[j]); break;
  }
}

#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
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

unsigned long last_time = 0;
WiFiClient client;            

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  // WIFI connecting

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
  display_num(4);
//  LEDarr.setChar(0,3,false);
  delay(2000);

}
void loop(){
  if(WiFi.status()== WL_CONNECTED) {
      display_num(4);
      HTTPClient http;  //--> Declare object of class HTTPClient.
      int httpCode;     //--> Variables for HTTP return code. 
      postData = "ID=1";
      volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
     
      dist = 29.988 * pow(volts, -1.173); // 10cm-80cm, 1080
//      dist = 12.08 * pow(volts , -1.058);// 4cm-30cm,430
//      dist =  60.374 * pow(volts , -1.16);// 20cm-150cm,20150
      
      

      postData += "&DISTANCE_4=" + String(dist);  
      payload = "";
      http.begin("http://140.120.14.51/device/update_4.php");  //--> Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
      httpCode = http.POST(postData); //--> Send the request
      payload = http.getString();     //--> Get the response payload
      postData = "";
      
//      if (dist <=5) display_num(999);
//      else display_num(1);
      http.end();
      
      if (millis() > last_time + 50){
        if(dist <= detect_dist){
          distBuffer[bCount] = dist;
          bCount++;
          LEDarr.clearDisplay(0); 
          display_num(99);
          clrTimer = millis();
          last_time = millis();
          delay(13);   
        }    
//        Serial.print(dist); Serial.print("CM, ");
      }
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
      
      
      if(bCount == buffersize){
        String s = "L4s";
        Serial.print("\ndistance sequence :[");
        for(int i=0; i< buffersize; i++){
          s += distBuffer[i];
//          Serial.print(distBuffer[i]);Serial.print(", ");
          s += ',';
        }
        bCount =0;
        Serial.print(s);
        Serial.println(']');
        
        postData = "ID=1"; 
        payload = "";
        http.begin("http://140.120.14.51/device/getdata.php");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
        httpCode = http.POST(postData); 
        payload = http.getString();    
        Serial.print("payload  : ");
        Serial.println(payload);  
        http.end(); 
       
        JSONVar myObject = JSON.parse(payload);
        String userlog = myObject["user_log"];
        userlog += s;
        http.begin("http://140.120.14.51/device/update_userLog.php");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");       
        httpCode = http.POST("USER_LOG="+userlog);
        http.end(); 
        
        LEDarr.clearDisplay(0); 
        display_num(999);       
        delay(1500);
        
      }  
  }
}
