#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <string.h>
#include <ArduinoJson.h>

StaticJsonDocument<200> doc;

const char* ssid     = "COSMOS";
const char* password = "fafa2020";
char path[] = "/";
char host[] = "192.168.161.126";
const int port = 9002;
int sourcePin = D5;
int ledPin = D3;
boolean switchStatus = false;


// Use WiFiClient class to create TCP connections
WiFiClient client;

WebSocketClient webSocketClient;



void setup() {
  pinMode(sourcePin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, switchStatus);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  if (client.connect("192.168.161.126", port)) {
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
}

  
void loop() {
  // put your main code here, to run repeatedly:
  
  if(client.connected()){
    String data;
    webSocketClient.getData(data);
    if(data.length() > 0) {
      Serial.print("Recv:\t"); Serial.println(data);
      deserializeJson(doc, data);
      if (!String(doc["deviceType"]).compareTo("led")){
        if (!String(doc["mode"]).compareTo("on")) {
          Serial.println("LED: On");
          switchStatus = HIGH;
        }
        else if(!String(doc["mode"]).compareTo("Off")){
          Serial.println("LED: On");
          switchStatus = LOW;
        }
      digitalWrite(ledPin, switchStatus);
    } //
  }// end of if-client.connected
  
  if(digitalRead(sourcePin) == LOW) {
    switchStatus = !switchStatus;
    digitalWrite(ledPin, switchStatus);
    if (switchStatus) {
      webSocketClient.sendData("Switch On");
    }
    else {
      webSocketClient.sendData("Switch Off");
    }
  delay(3000);
  }
}
}