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

  String pathWithGz = path + ".gz";
  File file1 = FSYS.open(path);
  File file2 = FSYS.open(pathWithGz);
  
  if(file1 || file2) {
    String contentType = getContentType(path);

    if(file2) {
      path += ".gz";
    }
    
    file1.close();
    file2.close();  
    
    File file = FSYS.open(path);
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
  return "text/plain";
}

void webserver_setup() {
//  server.on("/dumps/clear", clearDumps);
//  server.on("/dumps/list", getDumpsList);
//  server.on("/wificonfig/get", getConfig);
//  server.on("/wificonfig/set", setConfig);
//  server.on("/env.json", getEnv);
//
//  server.on(UriBraces("/dumps/{}"), handleDump);
//
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
