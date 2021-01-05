
/*
 * USE PIN: DIGITAL 2 = relay IN
 *          DIGITAL 7 = Reciever RF
 *          DIGITAL 8 = LED = RF
 *          DIGITAL 9 = LED = Relay STATUS
 * IPAddress 192.168.1.182
 * 
*/


#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include <VirtualWire.h>

const int receive_pin = 7;
String message; 
String myString;

// RELAY SIRAM 
int siram; 
int mesinair = 0; 
int air;
byte mac[] = {0x00, 0x92, 0x3B, 0xC2, 0x88, 0x1D};
IPAddress ip(192, 168, 1, 182);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
EthernetServer server(80);

void setup()
{
    delay(1000);
    Serial.begin(9600); // Debugging only
    Serial.println("setup");
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
    pinMode(2,INPUT_PULLUP);
 
      // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip, myDns, gateway);

      // Start to listen
    server.begin();
    Ethernet.init(10);  // Most Arduino shields
    Serial.println(F("Server is ready."));
    Serial.print(F("Please connect to http://"));
    Serial.println(Ethernet.localIP());

  
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(500);  // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    // Wait for an incomming connection
  EthernetClient client = server.available();

  // Allocate a temporary JsonDocument
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> doc;

    int sensorVal = digitalRead(2);
    int airVal = digitalRead(7);
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;


if (vw_have_message()) 
  {
    vw_get_message(buf, &buflen); // Non-blocking
    Serial.println((char*)buf);
    digitalWrite(8,HIGH);
    // ON ->> 1 to json
    Serial.println("ON");
      air= 1;     
   //   delay(800);
    }
 
  if (sensorVal == LOW){
    digitalWrite(9,LOW);
    siram = 0;
  }else{
    siram = 1;
    digitalWrite(9,HIGH);
  }
   //Serial.println(mesinair);
   // delay(500);
 
  doc["mesin_air"] = air;
  doc["pompa_tanaman"] = siram;
  
  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

}


String getValue(String data,  int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
     //   if (data.charAt(i) == separator || i == maxIndex) {
     //       found++;
     //       strIndex[0] = strIndex[1] + 1;
     //       strIndex[1] = (i == maxIndex) ? i+1 : i;
     //   }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
