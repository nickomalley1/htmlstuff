
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

int device_address=0x00;  //GPAK5 I2C address


//////// Edit This Section /////////


/*
const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PASSWORD";

IPAddress ip(192, 168, 43, 210);
*/
const char *ssid = "Apart 26";
const char *password = "apart2615";

IPAddress ip(192, 168, 88, 210);
///////////////////////////////////

String website;
int i = 0;

ESP8266WebServer server ( 80 );
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


int controlGPAKadr(int adr) {
  Wire.beginTransmission(adr);
  return Wire.endTransmission();
}
void writeOne(void) {
  int I2C_adr;
 

  if (!server.hasArg("nvmData") || !server.hasArg("adr") || !server.hasArg("RegAdr")) {
    server.send(200, "text/plain", "Error Missing arguments");
    return;
  }
  I2C_adr = server.arg("adr").toInt();
  if (controlGPAKadr(I2C_adr) != 0) {
    server.send(200, "text/plain", "Error Could not connect GPAK");
    return;
  }

  device_address=I2C_adr;
  writeI2C(byte(server.arg("RegAdr").toInt()), byte(server.arg("nvmData").toInt()));

  server.send ( 200, "text/html", "Success Value successfully written to register" );


}
void readOne(void) {
  int I2C_adr;
  String response = "";
  

  if (!server.hasArg("adr") || !server.hasArg("RegAdr")) {
    server.send(200, "text/plain", "Error Missing arguments");
    return;
  }
  I2C_adr = server.arg("adr").toInt();
  if (controlGPAKadr(I2C_adr) != 0) {
    server.send(200, "text/plain", "Error Could not connect GPAK");
    return;
  }

  device_address=I2C_adr;
  response += String(readI2C(byte(server.arg("RegAdr").toInt())));


  server.send ( 200, "text/html", response );
}

void writeAll(void) {
  int I2C_adr;
  

  if (!server.hasArg("nvmData%5B%5D") || !server.hasArg("adr")) {
    server.send(200, "text/plain", "Error Missing arguments");
    return;
  }
  I2C_adr = server.arg("adr").toInt();
  if (controlGPAKadr(I2C_adr) != 0) {
    server.send(200, "text/plain", "Error Could not connect GPAK");
    return;
  }

  device_address=I2C_adr;
  for (i = 0; i < 256; i++) {
    writeI2C(byte(i), byte(server.arg(i).toInt()));
  }

  server.send ( 200, "text/html", "Success Values successfully written to registers " );

}

void readAll(void) {
  int I2C_adr;
  String response = "";


  if (!server.hasArg("adr")) {
    server.send(200, "text/plain", "Error Missing arguments");
    return;
  }
  I2C_adr = server.arg("adr").toInt();
  if (controlGPAKadr(I2C_adr) != 0) {
    server.send(200, "text/plain", "Error Could not connect GPAK");
    return;
  }

  device_address=I2C_adr;
  for (i = 0; i < 255; i++) {
   

    response += String(readI2C(byte(i))) + "-";
  }
  response += String(readI2C(byte(255)));
  
  server.send ( 200, "text/html", response );
}

void multipleRead(void){
  int I2C_adr;
  String response = "";
  
  int num_reg=0;
  if (!server.hasArg("adr")||!server.hasArg("num_reg")) {
    server.send(200, "text/plain", "Error Missing arguments");
    return;
  }
  I2C_adr = server.arg("adr").toInt();
  if (controlGPAKadr(I2C_adr) != 0) {
    server.send(200, "text/plain", "Error Could not connect GPAK");
    return;
  }
  num_reg=server.arg("num_reg").toInt();
  device_address=I2C_adr;
  for (i = 0; i <num_reg-1 ; i++) {
    

    response += String(readI2C(byte(server.arg(i).toInt()))) + "-";
  }
  response += String(readI2C(byte(server.arg(num_reg-1).toInt())));
  
  server.send ( 200, "text/html", response );
}


void checkDevice(void) {
  int I2C_adr[16] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120};
  String response = "";
  int x = 0;
  for (i = 0; i < 16; i++) {
    if (controlGPAKadr(I2C_adr[i]) == 0) {
      x++;
      response += String(I2C_adr[i]) + "-";
    }
  }
  if (x == 0)
    server.send ( 200, "text/html", "Error No devices found." );
  else
    server.send ( 200, "text/html", response );
}

void handleRoot() {

  website = "<html>\n";
  website += " <head></head>\n";
  website += " <body></body>\n";
  website += "  <script src=\"https://code.jquery.com/jquery-3.1.1.js\"></script>\n";
  website += "  <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>";
  website += "     <script type=\"text/javascript\">\n";
  website += "         $(function () {\n";
  website += "            $.get(\"https://raw.githubusercontent.com/mbalci/GPAK5_Tracking/master/head.html\", function (data) {\n";
  website += "               $(\"head\").html(data);\n";
  website += "           });\n";
  website += "            $.get(\"https://raw.githubusercontent.com/mbalci/GPAK5_Tracking/master/body.html\", function (data) {\n";
  website += "               $(\"body\").html(data);\n";
  website += "           });\n";
  website += "      });\n";
  website += "   </script>\n";

  website += "</html>";


  server.send ( 200, "text/html", website );

}

void handleNotFound() {
  server.send ( 404, "text/plain", "not found" );

}
void writeI2C(byte byte_address, byte data) {
  Wire.beginTransmission(device_address);
  Wire.write(byte_address);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t readI2C(byte byte_address) {
  uint8_t value;
  Wire.beginTransmission(device_address);
  Wire.write(byte_address);
  Wire.endTransmission();
  
  Wire.requestFrom(device_address, 1);
  while(Wire.available()) {
    value = Wire.read();
  }
  return value;
}
void setup ( void ) {
  
  WiFi.config(ip, gateway, subnet);

  Serial.begin ( 9600 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  ////// EDIT THIS SECTION ////////////
  // Change this to the Wire.begin(0,2) for Generic esp8266
  
  Wire.begin();  

  ////////////////////////////////////
  
  server.on("/", handleRoot);
  server.on("/writeAll", writeAll);
  server.on ( "/readAll", readAll);
  server.on ( "/readOne", readOne);
  server.on ( "/writeOne", writeOne);
  server.on ( "/checkDevice", checkDevice);
  server.on("/multipleRead", multipleRead);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  server.handleClient();
}


