/* 
    See Feather Huzzah docs: https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi

    A lot of the Access Point config came from:
    https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/CaptivePortalAdvanced.ino
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h> //See: https://medium.com/@punnyarthabanerjee/esp8266-handling-data-in-json-7c6f62c9062e

#include <SimpleTimer.h>
#include "pages.h"
#include <Arduino.h>

#define REDLED 0
#define BLUELED 2
#define LED_ON LOW
#define LED_OFF HIGH
#define VBATPIN A0 
//AO requires an external voltage divider to measure battery voltage, this code is still here for reference

const char *softAP_ssid = "HuzzahWIFI";
const char *softAP_password = "password"; //No, this is not a critical password at all

/* Soft AP network parameters */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);
 
String webString = "";     // String to display
bool isLightOn = false;
SimpleTimer timer;

float batteryLevel(){
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Serial.print("VBat: " ); Serial.println(measuredvbat);
    return measuredvbat;
}

void logRequest() {
    // Get request details    
    String url = server.uri();
        
    // Log the request to serial
    Serial.print("req: "); 
    Serial.print(" "); 
    Serial.println(url);    
    
    int numArgs = server.args();
    Serial.print("Number of args: ");
    Serial.println(numArgs);
    for (int i=0; i< numArgs; i++){
      Serial.print(server.argName(i));
      Serial.print(" : ");
      Serial.println(server.arg(i));
    }
}

void handle_root() {
    logRequest();
    Serial.println("Root request RCVD");
    server.send(200, "text/html", pages::getIndexPage());
    delay(100);
}

void handle_state(){
    logRequest();
    Serial.println("state request RCVD"); 
    StaticJsonDocument<300> JSONData;
    // Use the object just like a javascript object or a python dictionary
    JSONData["batt"] = batteryLevel();
    JSONData["light"] = isLightOn;
    // You can add more fields
    char data[300];
    // Converts the JSON object to String and stores it in data variable
    serializeJson(JSONData,data);
    // Set status code as 200, content type as application/json and send the data
    server.send(200, "application/json", data);
    delay(100);
}

void handle_lighton(){ 
    logRequest();
    Serial.println("Light On RCVD"); 
    isLightOn = true;
    digitalWrite(BLUELED, LED_ON);
    server.send(200);            // send to someones browser when asked
    delay(100);
}

void handle_lightoff(){  
    logRequest();
    Serial.println("Light Off RCVD"); 
    isLightOn = false;
    digitalWrite(BLUELED, LED_OFF);
    server.send(200);            // send to someones browser when asked
    delay(100);
}

void setup(void){
    pinMode(REDLED, OUTPUT);
    pinMode(BLUELED, OUTPUT);

    digitalWrite(REDLED, LED_OFF); //On indicates error state during initialization
    digitalWrite(BLUELED, LED_ON); //On indicates Initialization Period

    // You can open the Arduino IDE Serial Monitor window to see what the code is doing
    Serial.begin(9600);  // Serial connection from ESP-01 via 3.3v console cable
    delay(1000);
    Serial.println("\n\r \n\rWorking to connect");

    //Create Access Point
    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(local_ip, gateway, subnet) ? " Ready" : " Failed!");
    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP(softAP_ssid, softAP_password) ? "  Ready" : " Failed!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    server.begin();
    Serial.println("HTTP server started");
  
    /* Setup the DNS server redirecting all the domains to the apIP */
    // dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    // dnsServer.start(DNS_PORT, "*", apIP);
    // delay(1000);
    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, handle_root);
    server.on("/state", HTTP_GET, handle_state);
    server.on("/lighton", HTTP_GET, handle_lighton);
    server.on("/lightoff", HTTP_GET, handle_lightoff);

    server.begin();
    Serial.println("HTTP server started");
    digitalWrite(BLUELED, LED_OFF); //On indicates Initialization Period

    timer.setInterval(1000, [](){
        digitalWrite(REDLED, LED_ON);
        //Serial.println("Status: " + String(WiFi.status()));
        timer.setTimeout(200, [](){
            digitalWrite(REDLED, LED_OFF);
        });
    });
}
 
void loop(void) {
    server.handleClient();
    // dnsServer.processNextRequest();
    timer.run(); 
} 