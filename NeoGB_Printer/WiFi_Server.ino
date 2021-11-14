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

void DeleteImage(){
  
}
void GetImage(){
  
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


void webserver_setup() {
//  server.on("/parseDumps", ParseDumps);
//  server.on("/refreshlist", RefreshWebData);
//  server.on("/delete", DeleteImage);
//  server.on("/download", GetImage);

  server.on("/parseDumps", HTTP_GET, [](AsyncWebServerRequest *request){
    ParseDumps();
    request->send(200, "text/plain", "OK");
  });
  server.on("/refreshlist", HTTP_GET, [](AsyncWebServerRequest *request){
    RefreshWebData();
    request->send(200, "text/plain", "OK");
  });

  server.onNotFound([](AsyncWebServerRequest *request){
      if(!handleFileRead(String(request->url()))){
        request->send(404, "text/html", "<html><body><h1>404 - Not Found</h1><p>You probably forgot to upload the additional data.</p></body></html>");
      }else{
        String reqPatch = "/www"+String(request->url());
        if (reqPatch.endsWith("/")) {
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
