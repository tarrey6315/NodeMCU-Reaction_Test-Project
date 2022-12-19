#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "LedControl.h"
#include "MyLedControl.h"




#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define default_delay 10;

const char* ssid = "SOCLAB";
const char* password = "1004soc1004";
//const char* ssid = "tien";
//const char* password = "pppppppp";

String postData = ""; 
String payload = "";
String website = "";

String str = "L1s10,L2,L3s10,l2s2,L4S2,R3";
int LEDseq[100];
int delayseq[100];
int seqNum = 1;
int repeatNum = 0;
int httpCode;
unsigned long last_time = 0;
int seqPTR = 0;
int seq = 0;
int export_flag = 1;
HTTPClient http; 
JSONVar myObject;    
WiFiClient client;   

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

void prepare(){
  display_num(999);delay(3000);
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
    website = "http://140.120.14.51/device/update_flag.php";
    access_dataBase(postData, website); 
    
    postData = "FLAG2=1";
    website = "http://140.120.14.51/device/update_flag2.php";
    access_dataBase(postData, website);  

    export_flag = 0;
    prepare();
}

void scriptStart(){
  Serial.print("millis= ");Serial.print(millis());
  Serial.print("\tlast= ");Serial.print(last_time);
  Serial.print("\tseqPTR= ");Serial.println(seqPTR);
  
  if(millis()- last_time >= delayseq[seqPTR]*1000 && delayseq[seqPTR]!= 0 && repeatNum != 0){
//    postData = "FLAG2=1";
//    website = "http://140.120.14.51/device/update_flag2.php";
//    access_dataBase(postData, website);
    seqPTR++;
    if(seqPTR == seqNum) LEDarray.clearDisplay(0);
    else display_num(LEDseq[seqPTR]);
    last_time = millis();
  }
  
    if((repeatNum==0)&& (seqPTR==0)){
      seqPTR=0; 
      export_flag ++;
      LEDarray.clearDisplay(0);   
      delay(1000);
  }
  else if( (repeatNum > 0) && (seqPTR == seqNum)){
    repeatNum --;
    seqPTR=0;
    if(repeatNum !=0) display_num(LEDseq[0]);
    else LEDarray.clearDisplay(0); 
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
  
  LEDarray.shutdown(0,false);  // 關閉省電模式
  LEDarray.setIntensity(0,0);  // 設定亮度為 0 (介於0~15之間)
  LEDarray.clearDisplay(0); 

  display_num(1);delay(100);
  display_num(2);delay(100);
  display_num(3);delay(100);
  display_num(4);delay(100);
  display_num(5);delay(100);
  display_num(6);delay(100);
  LEDarray.clearDisplay(0); 
  
 
}
void loop()
{
   //mysql server connecting
  if(WiFi.status()== WL_CONNECTED) {
    postData = "FLAG2=1";
    website = "http://140.120.14.51/device/getdata.php";
    access_dataBase(postData, website);  
    myObject = JSON.parse(payload);
    
    if (int(myObject["flag1"])){
      getInput(myObject["input"]);
    }
    scriptStart();
    
    if(export_flag == 1){
      postData = "";
      website = "http://140.120.14.51/device/export.php";
      access_dataBase(postData, website);
       
      delay(1000);
  }
    
   }
}
