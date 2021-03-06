// ArduinoJson - arduinojson.org

// https://arduinojson.org/v6/example/http-server/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Wire.h>

 
volatile int NbTopsFan; //measuring the rising edges of the signal
int Calc;
int hallsensor = 2;    //The pin location of the sensor
float vol = 0.0,l_menit; 
//float vol = 0.0; 
//int l_menit;
unsigned long currentTime;
unsigned long cloopTime;



byte mac[] = {0x00, 0xAA, 0xBB, 0xEF, 0xAA, 0x1C};
//set network
IPAddress ip(192, 168, 1, 181);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);

EthernetServer server(80);

void rpm ()     //This is the function that the interupt calls
{
    NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
}


void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;
    // start the Ethernet connection and the server:

  // Initialize Ethernet libary
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }
//  byte newMac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xAA, 0x1C};
//  Ethernet.setMACAddress(newMac);  // change the MAC address

  Ethernet.begin(mac, ip, myDns, gateway);
  
  // Start to listen
  server.begin();

  pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
  attachInterrupt(0, rpm, RISING); //and the interrupt is attached
  currentTime = millis();
  cloopTime = currentTime;
}

void loop() {
    
  // Wait for an incomming connection
  EthernetClient client = server.available();

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


  currentTime = millis();
  NbTopsFan = 0;   //Set NbTops to 0 ready for calculations
  sei();      //Enables interrupts
  delay (1000);   //Wait 1 second
  cli();      //Disable interrupts
//    Calc = (NbTopsFan * 60 / 5.5); //(Pulse frequency x 60) / 5.5Q, = flow rate in L/hour
//    Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 5.5Q, = flow rate in L/hour
  l_menit = (NbTopsFan / 5.5); //(Pulse frequency x 60) / 5.5Q, = flow rate in L/hour

  // print PULSE / RPM    
  doc["pulse"] = NbTopsFan;
  // print liter/menit
  doc["liter_per_menit"] = l_menit;

  // print liter/jam
  l_menit = l_menit/60;
  doc["liter_per_jam"] = l_menit;
  //print VOLUME
  vol=vol+l_menit;
  doc["volume"] = vol;

  if (NbTopsFan == 0) {
      doc["status"] = "OFF";
  } else {
      doc["status"] = "ON";
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
