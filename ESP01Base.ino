/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESP01 vu du dessus antenne à gauche                                                                             //
//                                                                                                                 //
//   io3-Rx   VCC                                                                                                  //
//   io0      RST   (RST et CH_EN doivent être HIGH, io0 doit être HIGH au Boot sinon ESP passe en Mode Flachage)  //                                                                           //
//   io2      CH_EN                                                                                                //
//   GND      io1-Tx                                                                                               //
//                                                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#define OTAName                    "ESP01Base"
#define WifiSSID                   "TBSOFT"
#define WifiPass                   "TB04011966"
#define IP                         56

#define io0                        0 // -> Doit être Pulled-Up par une resistance externe au Boot si elle est cablée sur autre chose (sinon Mode flash Boot)
#define io1                        1 // Tx
#define io2                        2 // Correspond a OnBoardLed
#define io3                        3 // Rx
#define tx                         1
#define rx                         3 

float param                        = 0;

// Class instances
ESP8266WebServer server(80); 

void loop() { server.handleClient(); ArduinoOTA.handle(); }
void root() { 
  if (server.arg("param" )!="") { param = server.arg("param").toFloat(); }   
  sendJsonResponse(); 
}
void sendJsonResponse() { 
  setHeaders(); server.send(200, "application/json", "{ "
  "\"api\": \"ESP01\", "
  "\"param\": " + String(param, 2) + " }"); 
}
void cors()             { setHeaders(); server.send(200, "text/plain", "" ); }
void handleNotFound()   { if (server.method() == HTTP_OPTIONS) { setHeaders(); server.send(204); } else server.send(404, "text/plain", ""); }
void redirectToRoot()   { server.sendHeader("Location", "/",true); server.send(302, "text/html",""); }
void setHeaders() {
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");  
  server.sendHeader("Access-Control-Allow-Origin","*");      
}
void setup() {  
  pinMode(tx, FUNCTION_3); // -> transforme tx et rx en GPIO
  pinMode(rx, FUNCTION_3); // -> transforme tx et rx en GPIO
  
  pinMode(io0, INPUT); 
  pinMode(io1, OUTPUT);   
  pinMode(io2, OUTPUT); 
  pinMode(io3, INPUT); 

  WiFi.config(IPAddress(192, 168, 0, IP), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
  WiFi.hostname(OTAName); WiFi.mode(WIFI_STA); WiFi.begin(WifiSSID, WifiPass);
  while (WiFi.status() != WL_CONNECTED) { delay(250); }

  ArduinoOTA.setHostname(OTAName); ArduinoOTA.begin();  

  server.on("/",               root); 
  server.on("/", HTTP_OPTIONS, cors);
  server.onNotFound(handleNotFound);
  server.begin();  
}

