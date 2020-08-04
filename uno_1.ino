/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                  // Create an instance

#define DHTPIN 2  
#define DHTTYPE DHT21 
DHT dht(DHTPIN, DHTTYPE);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  emon1.current(1, 111.1);             // Current: input pin, calibration.
    

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  dht.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
    // Wait a few seconds between measurements.
  delay(2000);


  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only

  
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
       //====================== HTML ============================
          // send a standard http response header
       //   client.println("HTTP/1.1 200 OK");
       //   client.println("Content-Type: application/json");
       //   client.println("Connection: close");  // the connection will be closed after completion of the response
       //   client.println("Refresh: 5");  // refresh the page automatically every 5 sec
       //   client.println();
       //   client.println("<!DOCTYPE HTML>");
       //   client.println("<html>");
       //   client.println("Humidity: "); 
       //   client.println(h);
       //   client.println(" %\t");
       //   client.println("Temperature: "); 
       //   client.println(t);
       //   client.println(" *C ");
       //   client.println(f);
       //   client.println(" *F\t");
       //   client.println("Heat index: ");
       //   client.println(hi);
       //   client.println(" *F");
       //   client.println("</html>");
       //==================== JSON ===================
//          client.print("{\"data\":{\"humidity\":\""+h+"\",\"temp_C\":\""+t+"\",\"temp_F\":\""+f+"\",\"heat\":\""+hi+"\"}}");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println();
          client.print("{\"humidity\":\"");
          client.print(h);
          client.print("\",\"temp_C\":\"");
          client.print(t);
          client.print("\",\"temp_F\":\"");
          client.print(f);
          client.print("\",\"heat\":\"");
          client.print(hi);
          client.print("\",\"power_usage\":\"");
          client.print(Irms*220.0);
          client.print("\",\"irms\":\"");
          client.print(Irms);
          client.print("\"}");


          
          
       


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
    client.stop();
    Serial.println("client disconnected");
  }
}
