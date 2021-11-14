#ifdef ENABLE_WEBSERVER

WebServer server(80);

void defaultHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
}

void send404() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/html", "<html><body><h1>404 - Not Found</h1><p>You probably forgot to upload the additional data.</p></body></html>");
}

bool handleFileRead(String path) {
  path = "/www" + path;

  if (path.endsWith("/")) {
    path += "index.html";
  }
  
  File file = FSYS.open(path);
  if(file) {
    String contentType = getContentType(path);
    defaultHeaders();
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }

  Serial.print(path);
  Serial.println(" - Not Found");
  return false;
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain"; //"application/octet-stream"
}


void parseDumps(){
  if(totalDumps > 0){
    ConvertFilesBMP();
    defaultHeaders();
    server.send(200, "application/json", "{\"Status: \"Done\"}");  
  }else{
    defaultHeaders();
    server.send(200, "application/json", "{\"Status: \"Failed\"}");  
  }
}

void refreshWebData(){
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
      
      #if defined(BMP_OUTPUT) || (!defined(BMP_OUTPUT) && !defined(PNG_OUTPUT))
        sprintf(imgDir, "/output/bmp/%s.bmp", String(imgName).substring(0, 5));
        if(FSYS.exists(imgDir)){
          file.print("\",\"bmp\":1");
        }else{
          file.print("\",\"bmp\":0");
        }
      #else
        file.print("\",\"bmp\":0");
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
    defaultHeaders();
    server.send(200, "application/json", "{\"Status: \"Failed\"}");  
    return;
  }
  file.print("}}]'");
  file.close();
  
  defaultHeaders();
  server.send(200, "application/json", "{\"Status: \"Done\"}");  
}

void webserver_setup() {
  server.on("/parseDumps", parseDumps);
  server.on("/refreshlist", refreshWebData);
//  server.on("/wificonfig/get", getConfig);
//  server.on("/wificonfig/set", setConfig);
//  server.on("/env.json", getEnv);

//  server.on(UriBraces("/dumps/{}"), handleDump);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      send404();
    }
  });
  server.begin();
  Serial.println(F("Server started"));
}

void webserver_loop() {
  server.handleClient();
}
#endif
