#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include "DHT.h"
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                  // Create an instance

#define DHTPIN 2  
#define DHTTYPE DHT21 
DHT dht(DHTPIN, DHTTYPE);

byte mac[] = {0x00, 0x92, 0x3B, 0xC2, 0x88, 0x82};
IPAddress ip(192, 168, 1, 180);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255,255,255,0); //subnet mask
EthernetServer server(80);
EthernetClient client;
double  t_status, t_pulse, t_pompa_air, t_pompa_tanaman;
float t_liter_menit, t_liter_jam,t_volume;
// ===================================================================

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip,subnet, myDns, gateway);
  server.begin();
  dht.begin();
  // LED Kran
   pinMode(8, OUTPUT);
  // LED Pompa AIR
   pinMode(7, OUTPUT);
  // LED Pompa Pohon
   pinMode(6, OUTPUT);
  emon1.current(1, 111.1);             
  Serial.println(F("starting..."));
}
//==================================================================================

void loop() {
  delay(1000);
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
  // Read Humidity
    float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);
    
 getTORN();
 getPOMPA();
  EthernetClient client = server.available();
   if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
          StaticJsonDocument<500> sensor_garasi;
           sensor_garasi["pompa_air"] = t_pompa_air;          
           sensor_garasi["pompa_tanaman"] = t_pompa_tanaman;          
           sensor_garasi["air_status"] = t_status;
           sensor_garasi["air_pulse"] = t_pulse;
           sensor_garasi["air_liter_per_menit"] = t_liter_menit;
           sensor_garasi["air_liter_per_jam"] = t_liter_jam;
           sensor_garasi["air_volume"] = t_volume;
           sensor_garasi["garasi_temp_C"] = t;
           sensor_garasi["garasi_humidity"] = h;
           sensor_garasi["garasi_temp_F"] = f;
           sensor_garasi["garasi_heat"] = hi;
           sensor_garasi["listrik_usage"] = Irms*220.0;
           sensor_garasi["listrik_irms"] = Irms;
      

          // Write response headers
          client.println(F("HTTP/1.0 200 OK"));
          client.println(F("Content-Type: application/json"));
          client.println(F("Connection: close"));
          client.print(F("Content-Length: "));
          client.println(measureJsonPretty(sensor_garasi));
          client.println();

          // Write JSON document
          serializeJsonPretty(sensor_garasi, client);
            break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
   }
}
//======================================================================================================

void getPOMPA(){
  client.setTimeout(10000);
  if (!client.connect("192.168.1.182", 80)) {
    return;
  }

  // Send HTTP request
  client.println(F("GET /example.json HTTP/1.0"));   
  client.println(F("Host: 192.168.1.182"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(6) + 60;
  StaticJsonDocument<200> sensor_pompa;

  // Parse JSON object
  DeserializationError error = deserializeJson(sensor_pompa, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    return;
  }

  // Extract values
  t_pompa_air = sensor_pompa["pompa_air"].as<long>();
  t_pompa_tanaman =  sensor_pompa["pompa_tanaman"].as<long>();
// LED ACTIVE

  if (t_pompa_tanaman == 1) {
    digitalWrite(7, HIGH); 
   } else {
    digitalWrite(7, LOW); 
   }

//LED MESIN AIR
  if (t_pompa_air == 1) {
    digitalWrite(6, HIGH); 
   } else {
    digitalWrite(6, LOW); 
   }

  // Disconnect
  client.stop();
}
//======================================================================================================
void getTORN(){
  client.setTimeout(10000);
  if (!client.connect("192.168.1.181", 80)) {
    return;
  }

  // Send HTTP request
  client.println(F("GET /example.json HTTP/1.0"));   
  client.println(F("Host: 192.168.1.181"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(6) + 60;
  //  DynamicJsonDocument doc(capacity);
  StaticJsonDocument<200> sensor_torn;

  // Parse JSON object
  DeserializationError error = deserializeJson(sensor_torn, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    return;
  }

  // Extract values
 // Serial.println(F("Response:"));
  t_status = sensor_torn["status"].as<long>();
 // Serial.println(txt_status);
  t_pulse =  (sensor_torn["pulse"].as<long>());
//  Serial.println(txt_pulse);
//  txt_liter_menit = (sensor_torn["liter_per_menit"].as<char>());
  t_liter_menit = sensor_torn["liter_per_menit"].as<float>();
//  Serial.println(txt_liter_menit);
  t_liter_jam = sensor_torn["liter_per_jam"].as<float>();
//  txt_liter_jam = (sensor_torn["liter_per_jam"].as<char>());
//  Serial.println(txt_liter_jam);
  t_volume = sensor_torn["volume"].as<float>();
//  txt_volume = (sensor_torn["volume"].as<char>());
//  Serial.println(txt_volume);

// LED ACTIVE
  String air_ngucur;
  if (t_status == 1) {
    digitalWrite(8, HIGH); 
   } else {
    digitalWrite(8, LOW); 
   }
  // Disconnect
  client.stop();
  

}
