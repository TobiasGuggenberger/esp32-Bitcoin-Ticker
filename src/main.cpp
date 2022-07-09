#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
#include <ArduinoJson.h>

// Programmteile laden
#include <btclogo.h>
#include <btckurs.h>
#include <uhrzeit.h>


WebServer Server;

AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4


/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
//void callback                (char*, byte*, unsigned int);
void loop                      ();
void rootPage                  ();
void Zeit_Datum                ();
void Zeit_Uhrzeit              ();
void btc_kurs                  ();


/////////////////////////////////////////////////////////////////////////// Intervall der Steuerung
unsigned long startSCHLEIFE_btckurs = 0;
unsigned long intervSCHLEIFE_btckurs = 10000; 

unsigned long startSCHLEIFE_zeit = 0;
unsigned long intervSCHLEIFE_zeit = 2500; 


/////////////////////////////////////////////////////////////////////////// WiFi Root Page 
void rootPage() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Bitcoin Ticker</h2>"
    "<p style=\"text-align:center;\">esp32 Wifi Autoconnect</p>"
    "<p></p><p style=\"padding-top:15px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";
  Server.send(200, "text/html", content);
}

/////////////////////////////////////////////////////////////////////////// Setup
void setup() {
  Serial.begin(115200);
  Serial.println();

 //OTA Setup für Firmware
  ArduinoOTA.setHostname("24KanalRelaisWohnzimmer");
  ArduinoOTA.setPassword("7n6WkRpZtxtkykyMUx329");
  ArduinoOTA.begin();  

  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.
  Config.autoReconnect = true;
  Config.hostName = "esp32-01";
  Portal.config(Config);

  // Behavior a root path of ESP8266WebServer.
  Server.on("/", rootPage);

  Serial.println("Creating portal and trying to connect...");
  // Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println(WiFi.getHostname());
  }

  // ntp Server init
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

/////////////////////////////////////////////////////////////////////////// Loop
void loop() {
  // Wifi Portal starten
  Portal.handleClient();

  // OTA Handle starten
  ArduinoOTA.handle();  

      ///////////////////////////////////////////////////////////////////////// BTC Kurs abfragen
      if (millis() - startSCHLEIFE_btckurs > intervSCHLEIFE_btckurs) {
          startSCHLEIFE_btckurs = millis();   // aktuelle Zeit abspeichern
          // BTC Kurs abfragen
          btc_kurs();
        }

      ///////////////////////////////////////////////////////////////////////// ZEIT abfragen
      if (millis() - startSCHLEIFE_zeit > intervSCHLEIFE_zeit) {
          startSCHLEIFE_zeit = millis();   // aktuelle Zeit abspeichern
          // BTC Kurs abfragen
          Zeit_Datum();
        }


}