/***********************************************
  Create an empty JSON if the file don't exist
************************************************/
void createEmptyConfig() {
  Serial.println("Preparing empty conf.json. \nYou can configure WiFi-Settings via the web interface.");
  File confFileEmpty = FSYS.open("/www/conf.json", FILE_WRITE);
  confFileEmpty.println("{}");
  confFileEmpty.close();
}

/***********************************************
  Set the WiFi settings to initiate the Server
************************************************/
bool setupWifi() {
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

        accesPointSSID = String(conf["NetworkSettings"]["network"]["ssid"].as<String>());
        accesPointPassword = String(conf["NetworkSettings"]["network"]["psk"].as<String>());
        //Get Access Point Config in case the WiFi Network Config don't exist.
        if (accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == "") {
          accesPointSSID = String(conf["NetworkSettings"]["ap"]["ssid"].as<String>());
          accesPointPassword = String(conf["NetworkSettings"]["ap"]["psk"].as<String>());
          Serial.println("No WiFi settings configured - getting the Access Point Settings...");

          //Set the SSID and Password as default config (check the config.h file) in case the Access Point Config don't exist.
          if (accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == "") {
            Serial.println("No AccessPoint settings configured - using default");
            accesPointSSID = DEFAULT_AP_SSID;
            accesPointPassword = DEFAULT_AP_PSK;
          }
        } else {
          return true;
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
  return false;
}

/******************************
  Edit settings via WebServer
*******************************/
String JsonErrorResponse (String msg) {
  return "{\"error\":\"" + msg + "\"}";
}

//Read config file
String GetConfigFile() {
  StaticJsonDocument<1023> conf;  
  File confFile = FSYS.open("/www/conf.json");

  if (confFile) {
    DeserializationError error = deserializeJson(conf, confFile.readString());
    confFile.close();
    if (!error) {
      //Read network wettings
      if (conf.containsKey("NetworkSettings")) {
        JsonVariant mdns = conf["NetworkSettings"]["mdns"].as<JsonVariant>();
        JsonVariant network = conf["NetworkSettings"]["network"].as<JsonVariant>();
        network.remove("psk");
        JsonVariant ap = conf["NetworkSettings"]["ap"].as<JsonVariant>();
        ap.remove("psk");
      }

      //Read image wettings
      if (conf.containsKey("ImageSettings")) {
        JsonVariant bmpconf = conf["ImageSettings"]["bmpimage"].as<JsonVariant>();
        JsonVariant pngconf = conf["ImageSettings"]["pngimage"].as<JsonVariant>();
      }
    } else {
      return JsonErrorResponse("JSON deserialization error");
    }
  } else {
    return JsonErrorResponse("No conf file");
  }

  String out;
  serializeJson(conf, out);
  conf.clear();
  return out;
}

//Write/Update Config File
String SetConfigFile(String body) {
  StaticJsonDocument<1023> confUpdate;
  StaticJsonDocument<1023> conf;
  DeserializationError error = deserializeJson(confUpdate, body);

  if (error) {
    return JsonErrorResponse("cannot parse request body");
  }

  File confFile = FSYS.open("/www/conf.json");

  if (confFile) {
    deserializeJson(conf, confFile.readString()); // no need to check for an error - file will be re-written anyways
    confFile.close();
  }

  if (confUpdate.containsKey("mdns")) {
    conf["mdns"] = confUpdate["mdns"].as<String>();
  }

  if (confUpdate.containsKey("ap")) {
    if (
      confUpdate["ap"].containsKey("ssid") &&
      confUpdate["ap"].containsKey("psk")
    ) {
      conf.remove("ap");
      JsonObject ap = conf.createNestedObject("ap");
      ap["ssid"] = confUpdate["ap"]["ssid"];
      ap["psk"] = confUpdate["ap"]["psk"];
    }
  }

  if (confUpdate.containsKey("networks")) {
    if (!conf.containsKey("networks")) {
      conf.createNestedArray("networks");
    }

    JsonArray networks = conf["networks"].as<JsonArray>();
    JsonArray networksUpdate = confUpdate["networks"].as<JsonArray>();

  }

  File confFileUpdated = FSYS.open("/www/conf.json", FILE_WRITE);
  serializeJson(conf, confFileUpdated);
  confFileUpdated.close();
  conf.clear();
  confUpdate.clear();
  return "\"ok\"";
}
