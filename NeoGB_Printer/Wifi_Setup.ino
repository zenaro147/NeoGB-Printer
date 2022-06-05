#include "config.h"
bool hasNetworkSettings = true;
String ip = "";

void initWifi(){   
  hasNetworkSettings = loadWiFiConfig(); //Get Data from the conf.json

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

    //Get Time from NTP Server (Article about RTC/NTP for ESP32: http://suadica.com/dica.php?d=439&t=como-utilizar-relogio-rtc-interno-do-esp32)
    ntp.begin();                
    ntp.forceUpdate();       
    Serial.print("Updating Date and Time...");
    datetime = ntp.getEpochTime();
    Serial.print("NTP Unix: ");
    Serial.println(datetime);
    timeval tv;
    tv.tv_sec = datetime;
    settimeofday(&tv, NULL);
    time_t tt = time(NULL);
    data = *gmtime(&tt);
    strftime(formatted_datetime, 64, "%d/%m/%Y %H:%M:%S", &data);
    Serial.print("Date and Time updated: ");
    Serial.println(formatted_datetime);
   
  } else {
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(accesPointSSIDc, accesPointPasswordc);
    Serial.println("AccessPoint started");
  }
}

#ifdef ENABLE_WEBSERVER
void mdns_setup() {
  loadMdnsConfig();
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
