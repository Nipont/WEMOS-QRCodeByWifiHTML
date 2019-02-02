/* *********************************************************************************
 * ESP8266 QRcode
 * dependency library :
 *   ESP8266 Oled Driver for SSD1306 display by Daniel Eichborn, Fabrice Weinberg
 *
 * SDA --> D6
 * SCL --> D7
***********************************************************************************/

#include <qrcode.h>
#include <SSD1306.h>
#include <WiFi.h>


#define WIFI_AP_NAME "WEMOS"
#define WIFI_AP_PASS NULL


//SSD1306  display(0x3c, D6, D7);
SSD1306  display(0x3c, 5, 4);
QRcode qrcode (&display);
String ipAddress;

WiFiServer server(80);

void setup() {

    Serial.begin(115200);



    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASS);

    ipAddress=WiFi.softAPIP().toString();
    Serial.print("IP address: ");
    Serial.println(ipAddress);

    server.begin();

    display.init();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);


    
    // enable debug qrcode
    qrcode.debug();

    // Initialize QRcode display using library
    qrcode.init();

    display.clear();
    display.display();
    display.drawString(0, 0, "Scan QR Code"); 
    display.drawString(0,10,"to connect AP");
    display.display();
    delay(5000);
    // create qrcode
    qrcode.create("WIFI:S:WEMOS;;");

    Serial.println();
    Serial.print("Waiting for AP get connect...");
    while (WiFi.softAPgetStationNum()<1){}
    
    Serial.println();
    Serial.println("AP Connected!");
    int value = 0;

     
    display.clear();
    display.drawString(0, 0, "Scan QR Code"); 
    display.drawString(0,10,"to open setting page.");
    display.display();
    delay(5000);
    
    qrcode.create("http://"+ipAddress);

}



void loop() {

  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,

    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:

            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<title>QRCode By Wifi HTML</title>");
            client.println("</head>");
            client.println("<body>");
            client.print("<H1>Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.</H1><br>");
            client.print("<H1>Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.</H1><br>");
            client.println("</body>");
            client.println("</html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
          display.clear();
          display.drawString(0, 0, "LED ON");
          display.display();
        }
        
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
          display.clear();
          display.drawString(0, 0, "LED OFF");
          display.display();
        }

        if (currentLine.endsWith("GET / ")) {

          qrcode.create("http://"+ipAddress);
        }


      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
