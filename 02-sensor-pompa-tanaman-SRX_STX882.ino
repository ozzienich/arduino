
/*
 * USE PIN: DIGITAL 2 = relay IN
 *          DIGITAL 5 = Reciever RF
 *          DIGITAL 8 = LED = RF
 *          DIGITAL 9 = LED = Relay STATUS
 * IPAddress 192.168.1.182
 * 
*/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

byte mac[] = {0x00, 0x92, 0x3B, 0xC2, 0x88, 0x1D};
IPAddress ip(192, 168, 1, 182);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
EthernetServer server(80);
int siram; 
int air;

void setup()
{
    Serial.begin(9600);  // Debugging only
    mySwitch.enableReceive(1);  // Receiver on interrupt 1 => that is pin #3

    pinMode(6,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(2,INPUT_PULLUP);
      // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip, myDns, gateway);
      // Start to listen
    server.begin();
    Ethernet.init(10);  // Most Arduino shields
    Serial.println(F("Server is ready."));
    Serial.print(F("Please connect to http://"));
    Serial.println(Ethernet.localIP());
    Serial.print( mySwitch.getReceivedValue() );
    
}

void loop()
{
    digitalWrite(6,LOW);
    int air = 0;
    int sensorVal = digitalRead(2);

// Wait for an incomming connection
    EthernetClient client = server.available();
    StaticJsonDocument<200> doc;
    
//    uint8_t buf[3];
//    uint8_t buflen = sizeof(buf);
  if (mySwitch.available()) {
    digitalWrite(6,HIGH);
    air = 1; 
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );
    mySwitch.resetAvailable();
  }

   delay(500);
    if (sensorVal == LOW){
      digitalWrite(5,LOW);
      siram = 0;
    }else{
      siram = 1;
      digitalWrite(5,HIGH);
    }

   doc["pompa_air"] = air;
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
