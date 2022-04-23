/***********************************************
  Create an empty JSON if the file don't exist
************************************************/
void createEmptyConfig() {
  Serial.println("Preparing empty conf.json. \nYou can configure WiFi-Settings via the web interface.");
  File confFileEmpty = FSYS.open("/www/conf.json", FILE_WRITE);
  confFileEmpty.println("{}");
  confFileEmpty.close();
}

/***************************************************************
  Set the WiFi settings to initiate the Server during the boot
****************************************************************/
bool setupWifi() {
  StaticJsonDocument<1023> conf;  
  File confFile = FSYS.open("/www/conf.json");

  if (confFile) { 
    DeserializationError error = deserializeJson(conf, confFile.readString());
    confFile.close();

    if (!error) {
      if (conf.containsKey("mdns")) {
        mdnsName = String(conf["mdns"].as<String>());
        if (mdnsName == "null" || mdnsName == ""){
          Serial.println("No MDNS settings configured - using default");
          mdnsName = DEFAULT_MDNS_NAME;
        }
      }
      if (conf.containsKey("network")) {
        accesPointSSID = String(conf["network"]["ssid"].as<String>());
        accesPointPassword = String(conf["network"]["psk"].as<String>());        
        if(accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == ""){
          Serial.println("No WiFi settings configured - getting the Access Point Settings...");
          if (conf.containsKey("ap")) {
            accesPointSSID = String(conf["ap"]["ssid"].as<String>());
            accesPointPassword = String(conf["ap"]["psk"].as<String>());
          }
          if(accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == ""){
            Serial.println("No AccessPoint settings configured - using default");
            accesPointSSID = DEFAULT_AP_SSID;
            accesPointPassword = DEFAULT_AP_PSK;
          } 
          return false;
        }
        return true;
      } else {
        if (conf.containsKey("ap")) {
          Serial.println("No WiFi settings configured - getting the Access Point Settings...");
          accesPointSSID = String(conf["ap"]["ssid"].as<String>());
          accesPointPassword = String(conf["ap"]["psk"].as<String>());
          if(accesPointSSID == "null" || accesPointSSID == "" || accesPointPassword == "null" || accesPointPassword == ""){
            Serial.println("No AccessPoint settings configured - using default");
            accesPointSSID = DEFAULT_AP_SSID;
            accesPointPassword = DEFAULT_AP_PSK;
          }
        } else {
          Serial.println("No AccessPoint settings configured - using default");
          accesPointSSID = DEFAULT_AP_SSID;
          accesPointPassword = DEFAULT_AP_PSK;
        }
        return false;
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


/****************************************************************
  Set the Image settings to initiate the Server during the boot
*****************************************************************/
void setupImages() {
  StaticJsonDocument<1023> conf;  
  File confFile = FSYS.open("/www/conf.json");

  if (confFile) { 
    DeserializationError error = deserializeJson(conf, confFile.readString());
    confFile.close();

    if (!error) {
      if (conf.containsKey("bmpimage")) {
        outputAsBMP = conf["bmpimage"]["output"];
        scaleBMP = conf["bmpimage"]["scale"];  
        if(outputAsBMP == NULL || outputAsBMP == 0 || scaleBMP == NULL || scaleBMP == 0){
          Serial.println("No BMP settings configured - using default");
          outputAsBMP = BMP_OUTPUT;
          scaleBMP = BMP_UPSCALE_FACTOR;
        }
      } 
      
      if (conf.containsKey("pngimage")) {
        outputAsPNG = conf["pngimage"]["output"];
        scalePNG = conf["pngimage"]["scale"];  
        if(outputAsPNG == NULL || outputAsPNG == 0 || scalePNG == NULL || scalePNG == 0){
          Serial.println("No PNG settings configured - using default");
          outputAsPNG = PNG_OUTPUT;
          scalePNG = PNG_UPSCALE_FACTOR;
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
      //Read network settings
      if (conf.containsKey("mdns")) {
        JsonVariant mdns = conf["mdns"].as<JsonVariant>();
      }
      if (conf.containsKey("network")) {
        JsonVariant network = conf["network"].as<JsonVariant>();
        network.remove("psk");
      }
      if (conf.containsKey("ap")) {
        JsonVariant ap = conf["ap"].as<JsonVariant>();
        ap.remove("psk");
      }

      //Read image settings
      if (conf.containsKey("bmpimage")) {
        JsonVariant bmpconf = conf["bmpimage"].as<JsonVariant>();
      }
      if (conf.containsKey("pngimage")) {
        JsonVariant pngconf = conf["pngimage"].as<JsonVariant>();
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

// Write-Update Config File
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
  
  //Set network settings
  if (confUpdate.containsKey("mdns")) {
    conf["mdns"] = confUpdate["mdns"].as<String>();
  }
  if (confUpdate.containsKey("ap")) {
    if (confUpdate["ap"].containsKey("ssid") && confUpdate["ap"].containsKey("psk")) {
      conf.remove("ap");
      JsonObject ap = conf.createNestedObject("ap");
      ap["ssid"] = confUpdate["ap"]["ssid"];
      ap["psk"] = confUpdate["ap"]["psk"];
    }
  }
  if (confUpdate.containsKey("network")) {
    if (confUpdate["network"].containsKey("ssid") && confUpdate["network"].containsKey("psk")) {
      conf.remove("network");
      JsonObject network = conf.createNestedObject("network");
      network["ssid"] = confUpdate["network"]["ssid"];
      network["psk"] = confUpdate["network"]["psk"];
    }
  }

  //Set image settings  
  if (confUpdate.containsKey("bmpimage")) {
    if (confUpdate["bmpimage"].containsKey("output") && confUpdate["bmpimage"].containsKey("scale")) {
      conf.remove("bmpimage");
      JsonObject bmpimage = conf.createNestedObject("bmpimage");
      bmpimage["output"] = confUpdate["bmpimage"]["output"];
      bmpimage["scale"] = confUpdate["bmpimage"]["scale"];
    }
  }  
  if (confUpdate.containsKey("pngimage")) {
    if (confUpdate["pngimage"].containsKey("output") && confUpdate["pngimage"].containsKey("scale")) {
      conf.remove("pngimage");
      JsonObject pngimage = conf.createNestedObject("pngimage");
      pngimage["output"] = confUpdate["pngimage"]["output"];
      pngimage["scale"] = confUpdate["pngimage"]["scale"];
    }
  }
  
  File confFileUpdated = FSYS.open("/www/conf.json", FILE_WRITE);
  serializeJson(conf, confFileUpdated);
  confFileUpdated.close();
  conf.clear();
  confUpdate.clear();
  return "\"ok\"";
}
