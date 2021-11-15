#ifdef ENABLE_WEBSERVER

AsyncWebServer server(80);

void ParseDumps(){
  if(totalDumps > 0){
    ConvertFilesBMP();
  }
}

void RefreshWebData(){
  String path = "/www/ImgList.json"; 
  File file = FSYS.open(path,FILE_WRITE);

  if(!file){
    Serial.println("failed to open file for reading");
    return;
  }

  file.print("data='[{\"FolderInfo\":{\"totImages\":");
  file.print(String(totalImages));
  file.print(",\"haveDumps\":");
  if(totalDumps > 0){
    file.print("1");
  }else{
    file.print("0");
  }
  file.print("},\"ImageFolder\":{");

  char thumbDir[31];
  char imgDir[31];
  char imgName[10];
  int imgID=1;
  File root;
  File imgFile;
  sprintf(thumbDir, "/www/thumb");
  root = FSYS.open(thumbDir);
  if(root.isDirectory()){
    imgFile = root.openNextFile();
    while(imgFile){
      if(imgID > 1){
        file.print(",");
      }
      sprintf(imgName, imgFile.name());
      file.print("\"");
      file.print(imgID);
      file.print("\":{\"ImageName\":\"");
      file.print(String(imgName).substring(0, 5));
      file.print("\",\"id\":");
      file.print(imgID);
      
      #if defined(BMP_OUTPUT) || (!defined(BMP_OUTPUT) && !defined(PNG_OUTPUT))
        sprintf(imgDir, "/output/bmp/%s.bmp", String(imgName).substring(0, 5));
        if(FSYS.exists(imgDir)){
          file.print(",\"bmp\":1");
        }else{
          file.print(",\"bmp\":0");
        }
      #else
        file.print(",\"bmp\":0");
      #endif
      
      #ifdef PNG_OUTPUT
        sprintf(imgDir, "/output/png/%s.png", String(imgName).substring(0, 5));
        if(FSYS.exists(imgDir)){
          file.print(",\"png\":1}");
        }else{
          file.print(",\"png\":0}");
        }
      #else
        file.print(",\"png\":0}");
      #endif
      
      imgID++;
      imgFile = root.openNextFile();
    }
  }else{
    return;
  }
  file.print("}}]'");
  file.close();
}




bool handleFileRead(String path) {
  path = "/www" + path;
  if (path.endsWith("/")) {
    path += "index.html";
  }
  if(FSYS.exists(path)){
    return true;
  }else{
    Serial.print(path);
    Serial.println(" - Not Found");
    return false;
  }
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".bmp")) return "image/bmp";
  return "text/plain"; //"application/octet-stream"
}

void DeleteImage(String img){
  char pathimg [25];
  sprintf(pathimg, "/www/thumb/%s.png", img);
  if(FSYS.remove(pathimg)){
    Serial.printf("%s Deleted. \n",pathimg);
  }else{
    Serial.printf("Error deleting %s \n",pathimg);
  }
  #ifdef BMP_OUTPUT
    sprintf(pathimg, "/output/bmp/%s.bmp", img);
    if(FSYS.remove(pathimg)){
      Serial.printf("%s Deleted. \n",pathimg);
    }else{
      Serial.printf("Error deleting %s \n",pathimg);
    }
  #endif
  #ifdef PNG_OUTPUT
    sprintf(pathimg, "/output/png/%s.png", img);
    if(FSYS.remove(pathimg)){
      Serial.printf("%s Deleted. \n",pathimg);
    }else{
      Serial.printf("Error deleting %s \n",pathimg);
    }
  #endif
}

void webserver_setup() {
  server.on("/parseDumps", HTTP_GET, [](AsyncWebServerRequest *request){
    ParseDumps();
    request->send(200, "text/plain", "{\"Status\":0}");
  });
  server.on("/refreshlist", HTTP_GET, [](AsyncWebServerRequest *request){
    RefreshWebData();
    request->send(200, "text/plain", "{\"Status\":0}");
  });

  server.on("/download", HTTP_GET, [] (AsyncWebServerRequest *request) {
    const char* PARAM_INPUT_1 = "id";
    const char* PARAM_INPUT_2 = "format";
    String inputMessage1;
    String inputMessage2;
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)){
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      //Do something
      String imgPath = "/output/"+inputMessage2+"/"+inputMessage1+"."+inputMessage2;
      request->send(SD, imgPath, "image/bmp");
    }else{
      request->send(200, "text/plain", "Missing Parameters");
    }
  });
  
  server.on("/delete", HTTP_GET, [] (AsyncWebServerRequest *request) {
    const char* PARAM_INPUT_1 = "id";
    String inputMessage1;
    String inputMessage2;
    if (request->hasParam(PARAM_INPUT_1)){
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      DeleteImage(inputMessage1);
      request->send(200, "text/plain", "{\"Status\":1}");
    }else{
      request->send(200, "text/plain", "{\"Status\":0}");
    }
  });

  server.onNotFound([](AsyncWebServerRequest *request){
      if(!handleFileRead(String(request->url()))){
        request->send(404, "text/html", "<html><body><h1>404 - Not Found</h1><p>You probably forgot to upload the additional data.</p></body></html>");
      }else{
        String reqPatch = "/www"+String(request->url());
        if (reqPatch.endsWith("/")){
          reqPatch += "index.html";
        }
        request->send(SD, reqPatch, getContentType(reqPatch));
      }
  });
  
  server.begin();
  Serial.println(F("Server started"));
}

void webserver_loop() {
}
#endif
