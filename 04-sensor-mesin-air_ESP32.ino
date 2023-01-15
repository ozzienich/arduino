#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>


/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/solved-reconnect-esp32-to-wifi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


// Replace with your network credentials (STATION)
const char* ssid = "kebonbinatang.org";
const char* password = "1234567a";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

IPAddress local_IP(192, 168, 1, 183);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


WiFiServer server(80);

void initWiFi() {
   // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);


  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

    server.begin();

}

void oz(){
    	
  StaticJsonDocument<100> testDocument;
  testDocument["sensorType"] = "Temperature";
  testDocument["value"] = 10;
 
  char buffer[100];
 
  serializeJson(testDocument, buffer);
 
  Serial.println(buffer);
}

void loop() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
WiFiClient client = server.available();
 if (client) {
   StaticJsonDocument<100> testDocument;
  testDocument["pompa_air"] =1;

            // Write response headers
          client.println(F("HTTP/1.0 200 OK"));
          client.println(F("Content-Type: application/json"));
          client.println(F("Connection: close"));
          client.print(F("Content-Length: "));
          client.println(measureJsonPretty(testDocument));
          client.println();

          // Write JSON document
          serializeJsonPretty(testDocument, client);
 }



}