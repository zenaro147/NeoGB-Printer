#ifdef ENABLE_WEBSERVER
#include "config.h"
bool hasNetworkSettings = true;
String ip = "";

void initWifi(){   
  hasNetworkSettings = setupWifi(); //Get Data from the conf.json

  const char * accesPointSSIDc = accesPointSSID.c_str();
  const char * accesPointPasswordc = accesPointPassword.c_str();
  
  WiFi.disconnect(); 
  Serial.print("Connecting to wifi ");  
  WiFi.mode(WIFI_MODE_STA);  
  WiFi.begin(accesPointSSIDc, accesPointPasswordc);
  WiFi.setSleep(false);
  
  unsigned int connTimeout = millis() + WIFI_CONNECT_TIMEOUT;
  unsigned int connTick = 0;
  while (hasNetworkSettings && (WiFi.status() != WL_CONNECTED)) {
    delay(250);
    connTick++;
  
    unsigned int remain = connTimeout - millis();
    if (remain <= 0 || remain > WIFI_CONNECT_TIMEOUT) {
      Serial.println("WiFi Connection timeout");
      hasNetworkSettings = false;
    }
  
    if (connTick % 4 == 0) {
      Serial.print(".");
      //STATUS CONNECTING
    }
  }
  
  if (hasNetworkSettings) {
    Serial.print("\nConnected to ");
    Serial.print(WiFi.SSID());
    Serial.print(" with IP address: ");
    Serial.println(WiFi.localIP());
    return;
  } else {
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(accesPointSSIDc, accesPointPasswordc);
    Serial.println("AccessPoint started");
  }
}

void mdns_setup() {
  String protocol = F("http://");
  String localsrv = ".local";
  
  const char * mdnsNamec = mdnsName.c_str();
  if (!MDNS.begin(mdnsNamec)) {
    Serial.println("Error setting up MDNS responder!");
  }
  
  MDNS.addService("http", "tcp", 80);
  WiFi.hostname(mdnsName + localsrv);
  
  Serial.println("mDNS responder started");
  
  if (hasNetworkSettings) {
    ip = WiFi.localIP().toString();
  } else {
    ip = WiFi.softAPIP().toString();
  }
  
  Serial.println(protocol + ip);
  Serial.println(protocol + mdnsName);
  Serial.println(protocol + mdnsName + localsrv);
}
#endif
