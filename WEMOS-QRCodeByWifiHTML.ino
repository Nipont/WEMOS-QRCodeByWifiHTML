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
String qrText="Hello";

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
/*
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
*/
}



void loop() {

  if (WiFi.softAPgetStationNum()<1){
    Serial.print("Waiting for AP get connect...");
    display.clear();
    display.display();
    display.drawString(0, 0, "Scan QR Code"); 
    display.drawString(0,10,"to connect AP");
    display.display();
    delay(5000);
    // create qrcode
    qrcode.create("WIFI:S:WEMOS;;");
    while (WiFi.softAPgetStationNum()<1){}
    Serial.println();
    Serial.println("AP Connected!");
   
    display.clear();
    display.drawString(0, 0, "Scan QR Code"); 
    display.drawString(0,10,"to open setting page.");
    display.display();
    delay(5000);
    
    qrcode.create("http://"+ipAddress);
  }

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
            client.println("  <meta charset=\"UTF-8\">");
            client.println("  <title>QRCode By Wifi HTML</title>");
            client.println("</head>");
            client.println("<body>");
            client.println("  <form action=\"/\">");
            client.println("    <H1>QR Text : </H1><br/>");
            client.println("    <textarea name=\"QRText\" cols=\"30\" rows=\"4\">"+qrText+"</textarea><br/>");
            client.println("    <br/><br/><input type=\"submit\" value=\"Submit\">");
            client.println("  </form>");
            client.println("</body>");
            client.println("</html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            
            int pos=-1;
            int pos2=-1;
            pos = currentLine.indexOf("GET /?QRText=");
            if (pos > -1){
              pos2=currentLine.indexOf("HTTP/1.1");
              qrText=currentLine.substring(pos+13, pos2-1);
              Serial.println("qrText = "+qrText);
              display.clear();
              display.drawString(0, 0, "qrText="+qrText);
              display.display();
              delay(2000);
              qrcode.create(qrText);
            }
            
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    Serial.println();

  }
  
}

int url_decode(char *encoded_str, char *decoded_str) {
   
    // While we're not at the end of the string (current character not NULL)
    while (*encoded_str) {
        // Check to see if the current character is a %
        if (*encoded_str == '%') {
    
            // Grab the next two characters and move encoded_str forwards
            encoded_str++;
            char high = *encoded_str;
            encoded_str++;
            char low = *encoded_str;
    
            // Convert ASCII 0-9A-F to a value 0-15
            if (high &gt; 0x39) high -= 7;
            high &amp;= 0x0f;
    
            // Same again for the low byte:
            if (low &gt; 0x39) low -= 7;
            low &amp;= 0x0f;
    
            // Combine the two into a single byte and store in decoded_str:
            *decoded_str = (high &lt;&lt; 4) | low;
        } else {
            // All other characters copy verbatim
            *decoded_str = *encoded_str;
        }
    
        // Move both pointers to the next character:
        encoded_str++;
        decoded_str++;
    }
    // Terminate the new string with a NULL character to trim it off
    *decoded_str = 0;
}
