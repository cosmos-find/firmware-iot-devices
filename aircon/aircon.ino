#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <string.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#define PIN 16
#define NUMPIXELS 8
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
StaticJsonDocument<200> doc;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, D0, NEO_GRB + NEO_KHZ800);

const char* ssid     = "COSMOS";
const char* password = "fafa2020";
char path[] = "/";
char host[] = "192.168.161.126";
const int port = 9002;

// Use WiFiClient class to create TCP connections
WiFiClient client;

WebSocketClient webSocketClient;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) continue;
  delay(10);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // wi-fi 연결
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(5000);

  // 서버 연결
  if (client.connect("192.168.161.126", 9002)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }
  // 웹소켓 연결
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }

  dht.begin();
  strip.begin();
  strip.show();
  lcd.begin();
  lcd.backlight();
}

  
void loop() {
  // put your main code here, to run repeatedly:
  int tem = dht.readTemperature();
  if(client.connected()){
    String data;
    webSocketClient.getData(data);
    if(data.length() > 0) {
      Serial.print("Recv:\t"); Serial.println(data);
      deserializeJson(doc, data);
      if (!String(doc["deviceType"]).compareTo("aircon")) {
        if (!String(doc["status"]).compareTo("On")) {
          int power = String(doc["power"]).toInt();
          int tem = dht.readTemperature();
          int TT = String(doc["targetTemp"]).toInt();
          if (!String(doc["mode"]).compareTo("Cooling")) {
            Serial.println("aircon: On");
            webSocketClient.sendData("aircon On, mode:Cooling");
            webSocketClient.sendData("CT: ",tem);
            lcd.backlight();
            lcd.setCursor(1,0);
            lcd.print("Mode: Cooling");
            lcd.setCursor(1,1);
            lcd.print("CT:");
            lcd.setCursor(5,1);
            lcd.print(tem);
            lcd.setCursor(8,1);
            lcd.print("TT:");
            lcd.setCursor(12,1);
            lcd.print(TT);
            for(int i=0; i<8; i++){
            strip.setPixelColor(i,0,0,power);
            }
            strip.show();
          }
          if (!String(doc["mode"]).compareTo("Heating")) {
            Serial.println("aircon: On");
            webSocketClient.sendData("aircon On, mode:Heating");
            webSocketClient.sendData("CT: ",tem); 
            lcd.backlight();
            lcd.setCursor(1,0);
            lcd.print("Mode: Heating");
            lcd.setCursor(1,1);
            lcd.print("CT:");
            lcd.setCursor(5,1);
            lcd.print(tem);
            lcd.setCursor(8,1);
            lcd.print("TT:");
            lcd.setCursor(12,1);
            lcd.print(TT);
            for(int i=0; i<8; i++){
            strip.setPixelColor(i,power,0,0);
            }
            strip.show();
          }
        }
        if (!String(doc["status"]).compareTo("Off")){
          webSocketClient.sendData("aircon Off");
          lcd.clear();
          lcd.noBacklight();
          for(int i=0; i<8; i++){
            strip.setPixelColor(i,0,0,0);
          }
          strip.clear();
          strip.show();
        }
      }
    }
  }      
}