// ArduinoJson - arduinojson.org

// https://arduinojson.org/v6/example/http-server/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11};
//set network
IPAddress ip(192, 168, 1, 180);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);

EthernetServer server(80);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();


void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;
    // start the Ethernet connection and the server:

  // Initialize HTU21D libary
//  if (!htu.begin()) {
//    Serial.println("Couldn't find sensor!");
//    while (1);
//  }
  
  // Initialize Ethernet libary
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }

  Ethernet.begin(mac, ip, myDns, gateway);

  // Start to listen
  server.begin();

  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());

  pinMode(8, OUTPUT);
}

void loop() {
 //   float temp = htu.readTemperature();
 //   float rel_hum = htu.readHumidity();
    
  // Wait for an incomming connection
  EthernetClient client = server.available();
delay(100);

  // Do we have a client?
  if (!client) return;

  Serial.println(F("New client"));

  // Read the request (we ignore the content in this example)
  while (client.available()) client.read();

  // Allocate a temporary JsonDocument
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<500> doc;
//  const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3);
//  DynamicJsonDocument doc(capacity);


  
// doc["temp"] = temp;
// doc["humadity"] = rel_hum;
 doc["test1"] = "adsasdada";

//==============================================================
// GET JSON TORN
  // Connect to HTTP server
  EthernetClient torn;
  torn.setTimeout(10000);
  if (!torn.connect("192.168.1.181", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }
  Serial.println(F("Connected!"));

  // Send HTTP request
  torn.println(F("GET /example.json HTTP/1.0"));
  torn.println(F("Host: 192.168.1.181"));
  torn.println(F("Connection: close"));
  if (torn.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }
    // Check HTTP status
//  char status[32] = {0};
//  torn.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
//  if (strcmp(status + 9, "200 OK") != 0) {
//    Serial.print(F("Unexpected response: "));
//    Serial.println(status);
//    return;
//  }

//  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!torn.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc2(capacity);
  // Parse JSON object
  DeserializationError error = deserializeJson(doc2, torn);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  String isi;
  isi = (doc2["status"].as<char*>());
  doc["status"] = isi;

  if (isi == "OFF") {
    digitalWrite(8, HIGH); 
   } else {
    digitalWrite(8, LOW); 
   }
  

  //Serial.print(F("Sending: "));
  serializeJson(doc, Serial);
  Serial.println();

  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  // Disconnect
  client.stop();
}
