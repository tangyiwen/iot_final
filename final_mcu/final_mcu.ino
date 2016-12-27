#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Servo.h>


//copy
#define MOTOR 12 //motor

#define BUZ_PIN 16//buzzer

#define fsr_pin A0 //壓力

Servo myservo; //motor

const int greenPin = 4;
const int redPin = 0;
const int bluePin = 5;

const int melody = 5000;
const int noteDuration = 250;

const int irReceiver = 13;                // 紅外線接收器
const int irLed  = 14;                    // 紅外線發射器
const unsigned int frequency = 38000;    // 發射頻率(單位: Hz)

//copy

int r = 0,g = 0,b = 0;

/*const char* ssid = "wenz";
const char* password = "wenz0537";*/

const char* ssid = "Pluto";
const char* password = "pluto27351";

ESP8266WebServer server(80);

char led_status[25];
String webSite;
byte x = 0;
void buildWeb() {
  webSite+="<html>\n";
  webSite+="<head>\n";
  webSite+="<meta charset='UTF-8'/>\n";
  webSite+="<title>FINAL</title>\n";
  webSite+="<style>\n";
  webSite+="body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n";
  webSite+="div{width: 500px;  text-align: center;margin:50 auto;}\n";
  webSite+="</style>\n";
  webSite+="</head>\n";
  webSite+="<body>\n";
  webSite+="<div>\n";
  webSite+="<h1>TITLE</h1>\n";
  webSite+="<p id=\"status\">You can select a status ...</p>\n";
  webSite+="<form action=\"/form1\">\n";
  webSite+="<input type=\"radio\" id=\"on\" name=\"lock\" value=\"on\"><label for=\"on\"> LOCK ON  </label>\n";
  webSite+="<input type=\"radio\" id=\"off\" name=\"lock\" value=\"off\"><label for=\"off\"> LOCK OFF </label><br><br>\n";
  webSite+="<input type=\"submit\" value=\"submit\"></form>\n";
  webSite+="</div>\n";
  webSite+="</body>\n";
  webSite+="</html>\n";      
}

void handleRoot() {
  server.send ( 200, "text/html", webSite ); //send back client a HTML file
}

void handleLEDStatus() {
  if (server.arg("lock") == "off") {
  snprintf(led_status, 25, "Now, motor is OFF ...");
  myservo.write(0); 
  /*Wire.beginTransmission(8); // transmit to device #8
  Wire.write(1);              // sends one byte
  Wire.endTransmission();    // stop transmitting*/
  }else {
  snprintf(led_status, 25, "Now, motor is ON ...");
   myservo.write(90); 
  /*Wire.beginTransmission(8); // transmit to device #8
  Wire.write(0);              // sends one byte
  Wire.endTransmission();    // stop transmitting*/
  }
  server.send ( 200 , "text/html" , led_status );
}

void handleNotFound() {
  server.send ( 404, "text/html", "404 Error!" );  
}
void setup(void){
  buildWeb();
  pinMode(BUZ_PIN,OUTPUT);
  pinMode(fsr_pin,INPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(redPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  pinMode(irReceiver,INPUT);
  pinMode(irLed,OUTPUT);
  myservo.attach(MOTOR);
  digitalWrite(irLed,HIGH);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on ( "/", handleRoot );
  server.on("/form1",handleLEDStatus);
  server.onNotFound ( handleNotFound );
  Wire.begin(4,5);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  //copy
  int fsr_value = analogRead(fsr_pin); //壓力感測器的值
  int wealth_value = map(fsr_value, 0, 1023, 0, 3); // 從0~1023映射到0~3
  int ir_status = digitalRead(irReceiver);   // 讀取 irReceiver 的狀態
  Serial.print(fsr_value); 
  Serial.print("|"); 
  Serial.print(ir_status);              // 把 irReceiver 的狀態印到 Serial Port
  Serial.print("|"); 
  
  switch(wealth_value){ //隨壓力變化的OLED、LED
    case 0:{
        b = 255;
        Serial.println("0000"); 
      }
      break;
    case 1:{
        g = 255;
        Serial.println("0001"); 
      }
      break;
    case 2:{
       r = 255;
       Serial.println("0002"); 
      }
      break;
    case 3:{
       r = g = b = 255;
       Serial.println("0003"); 
      }
      break;
    default:{
       Serial.println("9999"); 
      }
   break;
  }

  analogWrite(greenPin,r);
  analogWrite(redPin,g);
  analogWrite(bluePin,b);
  
  if (ir_status == 1) { //偵測是否有硬幣投入(紅外線)，有蜂鳴器發聲 0=recieve
    tone(BUZ_PIN, melody, noteDuration);
    delay(50);
  }
  //copy
}
