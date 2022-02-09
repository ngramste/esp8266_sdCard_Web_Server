#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <SD.h>
#include <SDConfig.h>
#include <Adafruit_NeoPixel.h>

const int chipSelect = D4;
char configFile[] = "network.conf";

// How many NeoPixels on the stick?
#define NUM_PIXELS 234
#define DIN_PIN D2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DIN_PIN, NEO_GRB + NEO_KHZ800);

char* ssid;
char* password;
char* host_name;

int r = 128;
int g = 128;
int b = 128;

ESP8266WebServer server(80);

const int led = 13;

class MyHandler : public RequestHandler {
  bool canHandle(HTTPMethod method, String uri) {
    return uri.startsWith( "/" );
  }

  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {    
    String contentType;
    Serial.println(requestUri);
    if (requestUri.equals("/")) {
      requestUri = "/index.html";
    }
    
    if (requestUri.equals("/api")) {
      handleAPICall();
      return true;
    }    

    if (requestUri.endsWith(".html")) contentType = "text/html";
    else if (requestUri.endsWith(".css")) contentType = "text/css";
    else if (requestUri.endsWith(".js")) contentType = "text/javascript";
    else if (requestUri.endsWith(".ico")) contentType = "image/x-icon";
    else if (requestUri.endsWith(".png")) contentType = "image/png";
    else if (requestUri.endsWith(".jpg")) contentType = "image/jpg";
    else if (requestUri.endsWith(".jpeg")) contentType = "image/jpeg";
    else if (requestUri.endsWith(".gif")) contentType = "image/gif";
    else if (requestUri.endsWith(".svg")) contentType = "image/svg+xml";
    else if (requestUri.endsWith(".webp")) contentType = "image/webp";
    else if (requestUri.endsWith(".avif")) contentType = "image/avif";
    else if (requestUri.endsWith(".apng")) contentType = "image/apng";
          
    if (SD.exists(requestUri)){      
      Serial.print("Request: ");
      Serial.print(requestUri);
      Serial.print(" 200 ");
      Serial.println(contentType);
    
      File fd = SD.open(requestUri, "r");
      
      size_t filesize = fd.size(); //the size of the file in bytes     
      char string[filesize + 1];
      memset(string, 0, filesize + 1);
      
      fd.read((uint8_t *)string, sizeof(string));  
      fd.close();
      
      server.send(200, contentType, string);
      return true;
    } else {
      Serial.print("Request: ");
      Serial.print(requestUri);
      Serial.print(" 404 ");
      Serial.println(contentType);
      handleNotFound();
    }
  }

  void handleAPICall() {
    String response;
    if (server.method() == HTTP_GET) {
      server.send(400, "text/plain", "Bad request");
      return;
    }
    
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i).equals("r")) r = server.arg(i).toInt();
      if (server.argName(i).equals("g")) g = server.arg(i).toInt();
      if (server.argName(i).equals("b")) b = server.arg(i).toInt();
    }

    updateStripColor();

    response = String(r);
    response += ",";
    response += String(g);
    response += ",";
    response += String(b);
    
    Serial.println(response);
    server.send(200, "text/plain", response);
  }

  void updateStripColor() {
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
  }
  
  void handleNotFound() {
    digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
  
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  
    server.send(404, "text/plain", message);
    digitalWrite(led, 0);
  }

} myHandler;

bool initSDCard() {
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return false;
  }
  Serial.println("initialization done.");
  return true;
}

bool readConfig() {
  int maxLineLength = 127;
   SDConfig cfg;
  // Open the configuration file.
  if (!cfg.begin(configFile, maxLineLength)) {
    Serial.print("Failed to open configuration file: ");
    Serial.println(configFile);
    return false;
  }
  // Read each setting from the file.
  while (cfg.readNextSetting()) {
    if (cfg.nameIs("ssid")) {
      // Dynamically allocate a copy of the string.
      ssid = cfg.copyValue();
    } else if (cfg.nameIs("psk")) {
      // Dynamically allocate a copy of the string.
      password = cfg.copyValue();
    } else if (cfg.nameIs("hostname")) {
      // Dynamically allocate a copy of the string.
      host_name = cfg.copyValue();
    } else {
      // report unrecognized names.
      Serial.print("Unknown name in config: ");
      Serial.println(cfg.getName());
    }
  }
  // clean up
  cfg.end();
  return true;
}

bool initLEDs() {
  strip.begin();
  strip.show(); // Start with all pixels off
  return true;
}

bool initWifi() {  
  WiFi.hostname(host_name);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(host_name);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(74880);

  // Initialize the SD card
  if (!initSDCard()) return;
  
  // Read the config file
  if (!readConfig()) return;
  
  // Configure the wifi connection
  if (!initLEDs()) return;
  
  // Configure the wifi connection
  if (!initWifi()) return;

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.addHandler (&myHandler);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
