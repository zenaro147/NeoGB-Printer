#ifdef ENABLE_WEBSERVER

WebServer server(80);
int imgID=0;
#define DUMP_CHUNK_SIZE 90

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
  defaultHeaders();
  File file = FSYS.open(imgPath);
  size_t sent = server.streamFile(file, contentType);
  file.close();
}

void refreshWebData(){
  #ifdef USE_OLED
    oledStateChange(11); //Generating File List
  #endif
  imgID=0;
  Serial.println("WEB - Generating a new file List...");
  char path[] = "/www/ImgList.json"; 
    File file = FSYS.open(path,FILE_WRITE);
    if(!file){
      Serial.println("failed to open file for reading");
      return;
    }
    
    Serial.print("WEB - Getting image list");
    file.print("data='[{\"ImageFolder\":{");
  
    char thumbDir[31];
    char imgDir[31];
    char imgName[30];
    sprintf(thumbDir, "/www/thumb");
    
    File root = FSYS.open(thumbDir);
    if(root.isDirectory()){
      File imgFile = root.openNextFile();
      while(imgFile){
        imgID++;
        if(imgID > 1){
          file.print(",");
        }
        String imgFilePath = (String)imgFile.name();
        uint8_t subStrLen = imgFilePath.length();
        sprintf(imgName, "%s", imgFilePath.substring(subStrLen-9,subStrLen-4));
        file.print("\"");
        file.print(imgID);
        file.print("\":{\"ImageName\":\"");
        file.print(imgName);
        file.print("\",\"id\":");
        file.print(imgID);
        
        #ifdef BMP_OUTPUT
          sprintf(imgDir, "/output/bmp/%s.bmp", imgName);
          if(FSYS.exists(imgDir)){
            file.print(",\"bmp\":1");
          }else{
            file.print(",\"bmp\":0");
          }
        #endif
        
        #ifdef PNG_OUTPUT
          sprintf(imgDir, "/output/png/%s.png", imgName);
          if(FSYS.exists(imgDir)){
            file.print(",\"png\":1}");
          }else{
            file.print(",\"png\":0}");
          }
        #endif
        imgFile = root.openNextFile();
      }
    }
    Serial.println("... Done!");

    Serial.print("WEB - Getting folder infos");
    file.print("},\"FolderInfo\":{\"totImages\":");
    file.print(imgID);
    file.print(",\"haveDumps\":");
    sprintf(thumbDir, "/dumps");  
     
    if(get_dumps() > 0){
      file.print("1");
    }else{
      file.print("0");
    }
    Serial.println("... Done!");
    
    file.print("}}]'");
    file.close();
  
  Serial.println("WEB - New file generated! Refreshing page...");
  #ifdef USE_OLED
    oledStateChange(9); //Printer Idle as Server
  #endif
  defaultHeaders();
  server.send(200, "application/json", "{\"Status\":1}");  
}

//void handleGenericArgs() { //Handler ---- http://192.168.0.192/genericArgs?year=2000&month=01&day=20
//  String message = "Number of args received:";
//  message += server.args();            //Get number of parameters
//  message += "\n";                            //Add a new line
//  for (int i = 0; i < server.args(); i++) {
//    message += "Arg nº" + (String)i + " –> ";   //Include the current iteration value
//    message += server.argName(i) + ": ";     //Get the name of the parameter
//    message += server.arg(i) + "\n";              //Get the value of the parameter
//  } 
//  server.send(200, "text/plain", message);       //Response to the HTTP request
//}

//void handleSpecificArg() { //Handler ---- http://192.168.0.192/specificArgs?Temperature=10
//  String message = "";
//  if (server.arg("Temperature")== ""){     //Parameter not found
//    message = "Temperature Argument not found";
//  }else{     //Parameter found
//    message = "Temperature Argument = ";
//    message += server.arg("Temperature");     //Gets the value of the query parameter
//  }
//  server.send(200, "text/plain", message);          //Returns the HTTP response
//}

/**************************************************************************************************************
  Functions to allow Remote Acces using the HerrZatacke interface https://herrzatacke.github.io/gb-printer-web/
***************************************************************************************************************/
void getDumpsList(){
  String dumpList;
  bool sep = false;
  
  File dumpDir = FSYS.open("/www/thumb");

  File file = dumpDir.openNextFile();
  while(file){
    if (sep) {
      dumpList += ",";
    } else {
      sep = true;
    }
    dumpList += "\"/thumb/";
        
    char imgName[30];
    String imgFilePath = (String)file.name();
    uint8_t subStrLen = imgFilePath.length();
    dumpList += imgFilePath.substring(subStrLen-9,subStrLen);
    
    dumpList += "\"";
    file = dumpDir.openNextFile();
  }  
  
  char fs[100];
  sprintf(fs, "{\"total\":0,\"used\":0,\"available\":0,\"maximages\":0,\"dumpcount\":%d}", imgID);

  defaultHeaders();
  server.send(200, "application/json", "{\"fs\":" + String(fs) + ",\"dumps\":[" + dumpList + "]}");
}

void handleDump() {
  String path = "/dumps/" + server.pathArg(0); 

  File file = FSYS.open(path);
  if(!file || file.isDirectory()){
    Serial.println("failed to open file for reading");
    return;
  }
    
  if(file) {
    defaultHeaders();

    server.setContentLength(file.available() * 3);
    server.send(200, "text/plain");

    Serial.println(file.available());
    Serial.println(file.available() * 3);

    char converted[DUMP_CHUNK_SIZE];
    uint8_t index = 0;

    while (file.available()) {
      char c = file.read();

      converted[index] = nibbleToCharLUT[(c>>4)&0xF];
      converted[index + 1] = nibbleToCharLUT[(c>>0)&0xF];
      converted[index + 2] = ' ';
      index += 3;

      if (index >= DUMP_CHUNK_SIZE || file.available() == 0) {
        server.sendContent(converted, index);
        index = 0;
      }
    }
    file.close();
    return;
  }
}

void getEnv(){
  server.send(200, "application/json", "{\"version\":\"0\",\"maximages\":0,\"env\":\"esp8266\",\"fstype\":\"littlefs\",\"bootmode\":\"alternating\",\"oled\":false}");  
}

/**********************************************
  Basic WebServer Functions and URL definitions
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
  else if (filename.endsWith(".json")) return "application/json";
  return "text/plain"; //"application/octet-stream"
}

void webserver_setup() {
  refreshWebData();
  server.on("/refreshlist", refreshWebData);
  server.on(UriBraces("/delete/{}"), DeleteImage);
  server.on(UriBraces("/download/{}"), GetImage);
  server.on(UriBraces("/d/{}"), handleDump);

  server.on("/env.json", getEnv);
  server.on("/dumps/list", getDumpsList);
  server.on(UriBraces("/dumps/{}"), handleDump);

//  server.on("/genericArgs", handleGenericArgs); //Associate the handler function to the path
//  server.on("/specificArgs", handleSpecificArg);   //Associate the handler function to the path
 
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
