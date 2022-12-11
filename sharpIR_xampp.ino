#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)

const char* ssid = "SOCLAB";
const char* password = "1004soc1004";
//const char* ssid = "tien";
//const char* password = "pppppppp";

String postData = "";
String payload = "";
//char db[] = "device";
// Domain Name with full URL Path for HTTP POST Request
float volts;
int dist ;

unsigned long lastTime = 0;
unsigned long timerDelay = 500;
long samplesTaken = 0; //Counter for calculating the Hz or read rate
long unblockedValue; //Average IR at power up
long startTime; //Used to calculate measurement rate
int count;


WiFiClient client;            


void setup()
{
  Serial.begin(9600);
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
  
  delay(2000);

}
void loop()
{
   //mysql server connecting
  if(WiFi.status()== WL_CONNECTED) {
      HTTPClient http;  //--> Declare object of class HTTPClient.
      int httpCode;     //--> Variables for HTTP return code.
//      postData = "ID=esp32_01"; 
//      payload = "";
//      http.begin("http://140.120.14.51/device/getdata.php");  //--> Specify request destination
//      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
//      httpCode = http.POST(postData); //--> Send the request
//      payload = http.getString();     //--> Get the response payload
//    
//      Serial.print("httpCode : ");
//      Serial.println(httpCode); //--> Print HTTP return code
//      Serial.print("payload  : ");
//      Serial.println(payload);  //--> Print request response payload
//      http.end();  //--> Close connection
//      
//      Serial.println();
      volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
      dist = 29.988*pow(volts, -1.173); // worked out from datasheet graph
      postData += "DISTANCE=" + String(dist);  
      payload = "";
      // C:\xampp/htdocs/device
      http.begin("http://140.120.14.51/device/update_1.php");  //--> Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
      httpCode = http.POST(postData); //--> Send the request
      payload = http.getString();     //--> Get the response payload
      postData = "";
      
//      Serial.print("httpCode : ");
//      Serial.println(httpCode); //--> Print HTTP return code
//      Serial.print("payload  : ");
//      Serial.println(payload);  //--> Print request response payload
      http.end();
      Serial.print(dist);
      Serial.println("CM\n");
      delay(1000);
   }
//   // 5v
//  float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
//  int dist = 29.988*pow(volts, -1.173); // worked out from datasheet graph
//  delay(500); // slow down serial port 
//  if (dist <= 30){
//      //insert, change database name and values by string and char[]
//    char INSERT_SQL[] = "INSERT INTO device.datalog (distance) VALUES ('dist')";
//    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
//    //execute
//    cur_mem->execute(INSERT_SQL);
//    delete cur_mem;
//    Serial.println("Data Saved.");
//    Serial.print(dist);
//    Serial.println("cm");// print the distance
//  }
//  else{
//      Serial.println("out of range");
//    }
//  lastTime = millis();
 
}
