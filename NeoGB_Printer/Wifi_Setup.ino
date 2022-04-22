#ifdef ENABLE_WEBSERVER
#include "config.h"
bool hasNetworkSettings = true;
String ip = "";


void createEmptyConfig() {
  Serial.println("Preparing empty conf.json. \nYou can configure WiFi-Settings via the web interface.");
  File confFileEmpty = FSYS.open("/www/conf.json", FILE_WRITE);
  confFileEmpty.println("{}");
  confFileEmpty.close();
}

void setupWifi() {
  StaticJsonDocument<1023> conf;  
  File confFile = FSYS.open("/www/conf.json");

  if (confFile) { 
    DeserializationError error = deserializeJson(conf, confFile.readString());
    confFile.close();

    if (!error) {
      if (conf.containsKey("mdns")) {
        if (String(conf["mdns"].as<String>()) != "") {
          mdnsName = String(conf["mdns"].as<String>());
        }
      }

      if (conf.containsKey("NetworkSettings")) { 
        mdnsName = String(conf["NetworkSettings"]["mdns"].as<String>());
        //Set the SSID and Passwork as default in case the mdns don't exist
        if (mdnsName == "null" || mdnsName == "") {
          accesPointPassword = DEFAULT_MDNS_NAME;
          Serial.println("No MDNS settings configured - using default");
        }

        hasNetworkSettings = true;
        accesPointSSID = String(conf["NetworkSettings"]["network"]["ssid"].as<String>());
        accesPointPassword = String(conf["NetworkSettings"]["network"]["psk"].as<String>());
        //Get Access Point Config in case the WiFi Network Config don't exist.
        if (accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == "") {
          hasNetworkSettings = false;
          accesPointSSID = String(conf["NetworkSettings"]["ap"]["ssid"].as<String>());
          accesPointPassword = String(conf["NetworkSettings"]["ap"]["psk"].as<String>());
          Serial.println("No WiFi settings configured - getting the Access Point Settings...");

          //Set the SSID and Password as default config (check the config.h file) in case the Access Point Config don't exist.
          if (accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == "") {
            Serial.println("No AccessPoint settings configured - using default");
            accesPointSSID = DEFAULT_AP_SSID;
            accesPointPassword = DEFAULT_AP_PSK;
          }
        }
      }          
    } else {
      Serial.println("Error parsing conf.json");
      Serial.println(error.c_str());
      createEmptyConfig();
    }
  } else {
    Serial.println("Could not open conf.json");
    createEmptyConfig();
  }
  conf.clear();
}



void initWifi(){  
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
