#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define NUMPIXELS 8

StaticJsonDocument<200> doc;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, D1, NEO_GRBW + NEO_KHZ800);

const char* ssid     = "COSMOS";
const char* password = "fafa2020";
char path[] = "/";
char host[] = "192.168.61.126";
const int port = 9999;


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
  if (client.connect("192.168.61.126", 9999)) {
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
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
}

  
void loop() {
  // put your main code here, to run repeatedly:
  
  if(client.connected()){
    String data;
    webSocketClient.getData(data);
    if(data.length() > 0) {
      Serial.print("Recv:\t"); Serial.println(data);
      deserializeJson(doc, data);
      int hum = dht.readHumidity();
      webSocketClient.sendData("CO2:",hum);
      if (!String(doc["deviceType"]).compareTo("cleaner")){
        if (!String(doc["mode"]).compareTo("on")) {
          Serial.println("Cleaner: On");
          webSocketClient.sendData("airCleaner On");
          int colorR = String(doc["colorR"]).toInt();
          int colorG = String(doc["colorG"]).toInt();
          int colorB = String(doc["colorB"]).toInt();
          strip.setPixelColor(0,  colorR, colorG, colorB, 0);
          strip.setPixelColor(1,  colorR, colorG, colorB, 0);
          strip.setPixelColor(2,  colorR, colorG, colorB, 0);
          strip.setPixelColor(3,  colorR, colorG, colorB, 0);
          strip.setPixelColor(4,  colorR, colorG, colorB, 0);
          strip.setPixelColor(5,  colorR, colorG, colorB, 0);
          strip.setPixelColor(6,  colorR, colorG, colorB, 0);
          strip.setPixelColor(7,  colorR, colorG, colorB, 0);
          strip.show();
          digitalWrite(D2, HIGH);
          digitalWrite(D3, LOW);
          analogWrite(D4, doc["motor"]);
        }
        else if(!String(doc["mode"]).compareTo("off")){
          Serial.println("Cleaner: Off");
          webSocketClient.sendData("airCleaner Off");
          for(int i=0; i<8; i++){
            strip.setPixelColor(i,0,0,0,0);
          }
          strip.clear();
          strip.show();
          digitalWrite(D4,0);
        }
      }
    }
  } // end-of-client.connected
  delay(3000);
}