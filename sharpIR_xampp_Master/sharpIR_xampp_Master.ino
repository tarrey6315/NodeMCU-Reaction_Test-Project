#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "LedControl.h"



LedControl LEDarr=LedControl(12,13,15,1); // LedControl(DIN,CLK,CS,DIVCEnum)->(D6,D7,D8)
#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define default_delay 10;

//const char* ssid = "SOCLAB";
//const char* password = "1004soc1004";
const char* ssid = "tien";
const char* password = "pppppppp";

String postData = ""; 
String payload = "";
      
String str = "L1s10,L2,L3s10,l2s2,L4S2,R3";
int LEDseq[100];
int delayseq[100];
int seqNum = 1;
int repeatNum = 0;
int httpCode;
unsigned long last_time = 0;
int seqPTR = 0;
int seq = 0;
byte export_flag = 0;
HTTPClient http; 
JSONVar myObject;    
WiFiClient client;   
/*arrow number
  1   6  
  2   5
  3   4*/
  
void display_num(int n){
  byte num1[8] = {                                    // 1
        0b11111110,0b11111100,0b11111000,0b11111100,
        0b11111110,0b11011111,0b10001110,0b00000100};
  byte num2[8] = {                                    // 2
        0b00010000,0b00110000,0b01111111,0b11111111,
        0b01111111,0b00110000,0b00010000,0b00000000};
  byte num3[8] = {                                   //  3
        0b00000100,0b10001110,0b11011111,0b11111110,
        0b11111100,0b11111000,0b11111100,0b11111110};
  byte num4[8] = {                                   //  4
        0b00100000,0b01110001,0b11111011,0b01111111,
        0b00111111,0b00011111,0b00111111,0b01111111};
  byte num5[8] = {                                  //  5
         0b00001000,0b00001100,0b11111110,0b11111111,
         0b11111110,0b00001100,0b00001000,0b00000000};
  byte num6[8] = {                                  //  6
        0b01111111,0b00111111,0b00011111,0b00111111,
        0b01111111,0b11111011,0b01110001,0b00100000};
 byte num_little[8] = {
        0b00000000,0b00000000,0b00000000,0b00011000,
        0b00011000,0b00000000,0b00000000,0b00000000};
  byte num_some[8] = {
      0b00000000,0b00000000,0b00111100,0b00111100,
       0b00111100,0b00111100,0b00000000,0b00000000};
 byte num_all[8] = {
        0b11111111,0b11111111,0b11111111,0b11111111,
        0b11111111,0b11111111,0b11111111,0b11111111};

  switch(n){
    case 1:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num1[j]); break;
    case 2:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num2[j]); break;
    case 3:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num3[j]); break;
    case 4:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num4[j]); break;
    case 5:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num5[j]); break;
    case 6:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num6[j]); break;
    case 9:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_little[j]); break;
    case 99:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_some[j]); break;
    case 999:
     for(int j=0;j<8;j++) LEDarr.setRow(0,j,num_all[j]); break;
  }
}
void prepare(){
  display_num(999);delay(1000);
//  display_num(99);delay(1000);
//  display_num(9);delay(1000);
  display_num(LEDseq[0]);
  
  Serial.print("LEDseq=[");
  for(int i=0;i<seqNum; i++){
    Serial.print(LEDseq[i]);Serial.print(',');
  }
  Serial.println("]");
  
  Serial.print("delayseq=[");
  for(int i=0;i<seqNum+1; i++){
    Serial.print(delayseq[i]);Serial.print(',');
  }
  Serial.println("]");
  Serial.print("seqNum= ");Serial.println(seqNum);
  last_time = millis();
}
  
void getInput(String data){
    seqNum = 0;
    repeatNum = -1;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex ; i++) {
       if (data.charAt(i) == ',' || i == maxIndex) {
          seqNum++;
          strIndex[0] = strIndex[1] + 1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
          String sub = data.substring(strIndex[0], strIndex[1]);
          
          if (sub.charAt(2)) {
            LEDseq[seqNum-1] = sub.substring(1,2).toInt();
            delayseq[seqNum-1] = sub.substring(3,sub.length()+1).toInt();
          }
          else if(sub.charAt(0) == 'r' ||sub.charAt(0) == 'R'){
            repeatNum = sub.substring(1,sub.length()+1).toInt(); 
             
            seqNum--;
            delayseq[seqNum] = delayseq[seqNum-1];   
          }
          else{
            LEDseq[seqNum-1] = sub.substring(1,2).toInt();
            delayseq[seqNum-1] = default_delay;
          }
      }
    }
 
    postData = "FLAG1=0";
    http.begin("http://140.120.14.51/device/update_flag.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    http.end();
    postData = "";

    postData = "FLAG2=1";
    http.begin("http://140.120.14.51/device/update_flag2.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
    http.end();
    postData = "";
    export_flag = 0;
    prepare();
}

void scriptStart(){
  Serial.print("millis= ");Serial.print(millis());
  Serial.print("\tlast= ");Serial.print(last_time);
  Serial.print("\tseqPTR= ");Serial.println(seqPTR);
  
  if(millis()- last_time >= delayseq[seqPTR]*1000 && delayseq[seqPTR]!= 0 && repeatNum != 0){
//    postData = "FLAG2=1";
//    http.begin("http://140.120.14.51/device/update_flag2.php");  //--> Specify request destination
//    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
//    httpCode = http.POST(postData); //--> Send the request
//    payload = http.getString();     //--> Get the response payload
//    http.end();
//    postData = "";
    
    seqPTR++;
    if(seqPTR == seqNum) LEDarr.clearDisplay(0);
    else display_num(LEDseq[seqPTR]);
    last_time = millis();
  }
  
    if((repeatNum==0)&& (seqPTR==0)){
      seqPTR=0; 
      export_flag ++;
      LEDarr.clearDisplay(0);   
      delay(1000);
  }
  else if( (repeatNum > 0) && (seqPTR == seqNum)){
    repeatNum --;
    seqPTR=0;
    if(repeatNum !=0) display_num(LEDseq[0]);
    else LEDarr.clearDisplay(0); 
  }
  
  else if((repeatNum < 0) && (seqPTR == seqNum)){
    seqPTR=0;  
    display_num(LEDseq[0]);
  }
}


void setup()
{
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

  display_num(1);delay(100);
  display_num(2);delay(100);
  display_num(3);delay(100);
  display_num(4);delay(100);
  display_num(5);delay(100);
  display_num(6);delay(100);
  LEDarr.clearDisplay(0); 
  
 
}
void loop()
{
   //mysql server connecting
  if(WiFi.status()== WL_CONNECTED) {
      postData = "ID=1"; 
      payload = "";
      http.begin("http://140.120.14.51/device/getdata.php");  //--> Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
      int httpCode = http.POST(postData); //--> Send the request
      payload = http.getString();     //--> Get the response payload
//      Serial.print("payload  : ");
//      Serial.println(payload);  //--> Print request response payload
      http.end();  //--> Close connection
      myObject = JSON.parse(payload);
    if (int(myObject["flag1"])){
      getInput(myObject["input"]);
    }
    scriptStart();
    
    if(export_flag == 1){
      http.begin("http://140.120.14.51/device/export.php");  //--> Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
      httpCode = http.POST(postData); //--> Send the request
      payload = http.getString();     //--> Get the response payload
      http.end();
      delay(1000);
  }
    
   }
}
