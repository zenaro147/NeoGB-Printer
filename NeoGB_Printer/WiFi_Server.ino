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
    file.print("true");
  }else{
    file.print("false");
  }
  file.print("},\"ImageFolder\":{}");
  //"1":{"thumb":"thumb/00001.png","bmp":"output/bmp/00001.bmp","png":"output/png/00001.png"},
  
  file.print("}]'"); //"}}]'"
  file.close();
  
  defaultHeaders();
  server.send(200, "application/json", "{\"Status: \"Done\"}");  
}

void webserver_setup() {
  refreshWebData();
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
