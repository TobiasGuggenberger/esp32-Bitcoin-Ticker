#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>


WebServer Server;

AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4


// Setup für Uhr
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
//void callback                (char*, byte*, unsigned int);
void loop                      ();
void rootPage                  ();
void Zeit_Datum                ();
void Zeit_Uhrzeit              ();
void btc_kurs                  ();

/////////////////////////////////////////////////////////////////////////// ZEIT
void Zeit_Datum()
{
  tm local;
  getLocalTime(&local);
  
  Serial.println(&local, "%d.%m.%y");
}

void Zeit_Uhrzeit()
{
  tm local;
  getLocalTime(&local);
  
  Serial.println(&local, "%H:%M");
}


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


/////////////////////////////////////////////////////////////////////////// BTC Kurs 
void btc_kurs(){

   client->setInsecure();
  HTTPClient https;

  //if (https.begin(*client, "https://min-api.cryptocompare.com/data/pricemulti?fsyms=XRP&tsyms=ZAR&e=Luno&extraParams=your_app_name")) {  // HTTPS
  if (https.begin(*client, "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC&tsyms=USD")) {  // HTTPS
    int httpCode = https.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      Serial.println(httpCode);
      // file found at server?
      //if (httpCode == HTTP_CODE_OK) {
      if (httpCode == 200) {
        String payload = https.getString();

        const size_t capacity = 2 * JSON_OBJECT_SIZE(1) + 10;
        DynamicJsonDocument doc(capacity);

        String json = payload;

        //const char* json = "{\"XRP\":{\"ZAR\":4.20}}";

        deserializeJson(doc, json);

        String BTC_USD = doc["BTC"]["USD"]; 
        
        Serial.println("BTC Wert ausgeben");
        /*
      if (BTC_old_kurs < BTC_USD) {

      tft.drawBitmap(0, 0, bitcoinLogo, 128, 64, GREEN);
      tft.setCursor(24,78);
      tft.setTextColor(GREEN,BLACK);
      tft.setTextSize(2);
      tft.print("$"+BTC_USD.substring(0, 5));

      }
      else
      {
      tft.drawBitmap(0, 0, bitcoinLogo, 128, 64, RED);
      tft.setCursor(24,78);
      tft.setTextColor(RED,BLACK);
      tft.setTextSize(2);
      tft.print("$"+BTC_USD.substring(0, 5));
      }

      BTC_old_kurs = BTC_USD;
*/
      }
    } else {

    }

  } else {

  }

}


/////////////////////////////////////////////////////////////////////////// Setup
void setup() {
  Serial.begin(115200);
  Serial.println();

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

delay(80000);
 
btc_kurs();

}