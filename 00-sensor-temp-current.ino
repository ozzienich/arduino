

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>


  // Initialize Ethernet library
byte mac[] = {0x00, 0x92, 0x3B, 0xC2, 0x88, 0x82};
IPAddress ip(192, 168, 1, 180);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255,255,255,0); //subnet mask
EthernetServer server(80);
// Connect to HTTP server
EthernetClient net_torn;
String  txt_status,txt_pulse,txt_liter_menit, txt_liter_jam, txt_volume, air_ngucur;

// ===================================================================
void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;

  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip,subnet, myDns, gateway);
  // Start to listen
  server.begin();
  
//  delay(1000);
  pinMode(8, OUTPUT);
  Serial.println(F("Connecting..."));

}
//==================================================================================
void loop() {
  delay(1000);
 // Serial.println(F("Connecting..."));
  getTORN();

  EthernetClient net_torn = server.available();
   if (net_torn) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (net_torn.connected()) {
      if (net_torn.available()) {
        char c = net_torn.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          StaticJsonDocument<500> sensor_garasi;

           sensor_garasi["air_pompa"] = "OFF";          
           sensor_garasi["air_status"] = txt_status;
           sensor_garasi["air_pulse"] = txt_pulse;
           sensor_garasi["air_liter_per_menit"] = txt_liter_menit;
           sensor_garasi["air_liter_per_jam"] = txt_liter_jam;
           sensor_garasi["air_volume"] = txt_volume;
         

          
          Serial.print(F("Sending: "));
          serializeJson(sensor_garasi, Serial);
          Serial.println();

          // Write response headers
          net_torn.println(F("HTTP/1.0 200 OK"));
          net_torn.println(F("Content-Type: application/json"));
          net_torn.println(F("Connection: close"));
          net_torn.print(F("Content-Length: "));
          net_torn.println(measureJsonPretty(sensor_garasi));
          net_torn.println();

          // Write JSON document
          serializeJsonPretty(sensor_garasi, net_torn);
            break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    net_torn.stop();
    Serial.println("client disconnected");
   }
}

//======================================================================================================
void getTORN(){
 // Connect to HTTP server
  //EthernetClient net_torn;

  net_torn.setTimeout(10000);
  if (!net_torn.connect("192.168.1.181", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }


  // Send HTTP request
  net_torn.println(F("GET /example.json HTTP/1.0"));
  net_torn.println(F("Host: 192.168.1.181"));
  net_torn.println(F("Connection: close"));
  if (net_torn.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  net_torn.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!net_torn.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
//  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(6) + 60;
//  DynamicJsonDocument doc(capacity);
  StaticJsonDocument<200> sensor_torn;


  // Parse JSON object
  DeserializationError error = deserializeJson(sensor_torn, net_torn);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  Serial.println(F("Response:"));
  txt_status= (sensor_torn["status"].as<char*>());
  Serial.println(txt_status);
  txt_pulse =  (sensor_torn["pulse"].as<long>());
  Serial.println(txt_pulse);
  txt_liter_menit = (sensor_torn["liter_per_menit"].as<float>());
  Serial.println(txt_liter_menit);
  txt_liter_jam = (sensor_torn["liter_per_jam"].as<float>());
  Serial.println(txt_liter_jam);
  txt_volume = (sensor_torn["volume"].as<float>());
  Serial.println(txt_volume);



// LED ACTIVE
  String air_ngucur;
  air_ngucur = (sensor_torn["status"].as<char*>());
  //sensor_garasi["status"] = "OFF";

  if (air_ngucur == "ON") {
    digitalWrite(8, HIGH); 
   } else {
    digitalWrite(8, LOW); 
   }
  // Disconnect
  net_torn.stop();
  

}
