#ifdef ENABLE_WEBSERVER

WebServer server(80);

//void parseDumps(){
//  if(totalDumps > 0){
//    ConvertFilesBMP();
//    defaultHeaders();
//    server.send(200, "application/json", "{\"Status: \"Done\"}");  
//  }else{
//    defaultHeaders();
//    server.send(200, "application/json", "{\"Status: \"Failed\"}");  
//  }
//}

void DeleteImage(){
  String img = server.pathArg(0);
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
void GetImage(){
  String imgPath = server.pathArg(0);
  if (imgPath.endsWith(".bmp")){
    imgPath = "/output/bmp/" + imgPath;
  }
  if (imgPath.endsWith(".png")){
    imgPath = "/output/png/" + imgPath;
  }
  String contentType = getContentType(imgPath);
  //defaultHeaders();
  File file = FSYS.open(imgPath);
  size_t sent = server.streamFile(file, contentType);
  file.close();
}

void refreshWebData(){
  char path[] = "/www/ImgList.json"; 
    File file = FSYS.open(path,FILE_WRITE);
    if(!file){
      Serial.println("failed to open file for reading");
      return;
    }
  
    file.print("data='[{\"FolderInfo\":{\"totImages\":");
    
    Serial.print("Total Images: ");
    Serial.println(totalImages);
    
    file.print(String(totalImages));
    file.print(",\"haveDumps\":");
    Serial.print("haveDumps: ");
    if(totalDumps > 0){
      file.print("1");
      Serial.println("1");
    }else{
      file.print("0");
      Serial.println("0");
    }
    file.print("},\"ImageFolder\":{");
  
    char thumbDir[31];
    char imgDir[31];
    char imgName[10];
    int imgID=1;
    sprintf(thumbDir, "/www/thumb");
    
    Serial.print("Thumb Dir: ");
    Serial.println(thumbDir);
    
    File root = FSYS.open(thumbDir);
    if(root.isDirectory()){
      File imgFile = root.openNextFile();
      while(imgFile){
        if(imgID > 1){
          file.print(",");
        }
        sprintf(imgName, imgFile.name());
        
        Serial.print("Img File: ");    
        Serial.println(imgFile);    
        Serial.print("imgName: ");
        Serial.println(imgName);
        Serial.print("imgID: ");
        Serial.println(String(imgName).substring(0, 5));   
        
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
    }
    file.print("}}]'");
    file.close();
  
  defaultHeaders();
  server.send(200, "application/json", "{\"Status\":1}");  
}
void getEnv(){
  server.send(200, "application/json", "{\"version\":\"0\",\"maximages\":0,\"env\":\"esp8266\",\"fstype\":\"littlefs\",\"bootmode\":\"alternating\",\"oled\":false}");  
}

/**************************************************************************************************************
  Functions to the Remote Acces using the Herr Zatacke interface https://herrzatacke.github.io/gb-printer-web/
***************************************************************************************************************/
void getDumpsList(){
//  String fileName = "";
//  String fileShort = "";
//
//  // get number of files in /d/
//  unsigned int dumpcount = 0;
//
//  String out;
//  String dumpList;
//  bool sep = false;
//
//  uint64_t total = 0;
//  total = FSYS.totalBytes();
//  uint64_t used = 0;
//  used = FSYS.usedBytes();  
//  uint64_t avail = total - used;
//  
//  File dumpDir = FSYS.open("/d");
//
//
//  File file = dumpDir.openNextFile();
//  while(file){
//    dumpcount++;
//    if (sep) {
//      dumpList += ",";
//    } else {
//      sep = true;
//    }    
//    dumpList += "\"";
//    dumpList += "/dumps/";
//    dumpList += file.name();
//    dumpList += "\"";
//    file = dumpDir.openNextFile();
//  }  
//  
//  char fs[100];
//  sprintf(fs, "{\"total\":%llu,\"used\":%llu,\"available\":%llu,\"maximages\":%d,\"dumpcount\":%d}", total, used, avail, MAX_IMAGES, dumpcount);
//
//  defaultHeaders();
//  server.send(200, "application/json", "{\"fs\":" + String(fs) + ",\"dumps\":[" + dumpList + "]}");
  Serial.println("Enter Get Dump List");
  send404();
}

void handleDump() {
//  String path = "/d/" + server.pathArg(0); 
//
//  File file = FSYS.open(path); //check what print here
//  Serial.println(path);
//  if(!file || file.isDirectory()){
//    Serial.println("failed to open file for reading");
//    return;
//  }
//    
//  if(file) {
//    defaultHeaders();
//
//    server.setContentLength(file.available() * 3);
//    server.send(200, "text/plain");
//
//    Serial.println(file.available());
//    Serial.println(file.available() * 3);
//
//    const char nibbleToCharLUT[] = "0123456789ABCDEF";
//
//    char converted[DUMP_CHUNK_SIZE];
//    uint8_t index = 0;
//
//    while (file.available()) {
//      char c = file.read();
//
//      converted[index] = nibbleToCharLUT[(c>>4)&0xF];
//      converted[index + 1] = nibbleToCharLUT[(c>>0)&0xF];
//      converted[index + 2] = ' ';
//      index += 3;
//
//      if (index >= DUMP_CHUNK_SIZE || file.available() == 0) {
//        Serial.println(index + 3);
//        server.sendContent(converted, index);
//        index = 0;
//      }
//    }
//    file.close();
//    return;
//  }
  Serial.println("Enter Handle Dump");
  send404();
}

/**********************************************
  Main WebServer Functions and URL definitions
***********************************************/
void defaultHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
}
void send404() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/html", "<html><body><h1>404 - Not Found</h1></body></html>");
}

bool handleFileRead(String path) {
  path = "/www" + path;
  if (path.endsWith("/")) {
    path += "index.html";
  }
  if(FSYS.exists(path)){
    String contentType = getContentType(path);
    
    File file = FSYS.open(path);
    defaultHeaders();
    size_t sent = server.streamFile(file, contentType);
    file.close();
    
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
  server.on("/refreshlist", refreshWebData);
  server.on(UriBraces("/delete/{}"), DeleteImage);
  server.on(UriBraces("/download/{}"), GetImage);

  server.on("/env.json", getEnv);
  server.on("/dumps/list", getDumpsList);
  server.on(UriBraces("/dumps/{}"), handleDump);
  

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
