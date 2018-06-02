#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include "NeoPatterns.h"

#define ESP_LED 13 // ESP module LED pin
#define SIG_PIN 5 // LED strip signal pin - D1 on board (D4-2/D5-14/D1-5)
#define LEN_NUM 60 // number of LEDs on strip
#define SERIAL_FREQ 115200 // Serial port frequency

const char *apSsid = "LED_STRIP_NOMAP";
const char *apPassword = "ledstrippassword";
const char *OTAName = "ESP8266";
const char *OTAPassword = "esp8266";
const char* mdnsName = "strip";

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);     // http server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81
File fsUploadFile; // temporary storage for uploaded file
void stripCallback();
NeoPatterns strip(LEN_NUM, SIG_PIN, NEO_GRB + NEO_KHZ400, &stripCallback);

void stripCallback()
{
  strip.Reverse();
}

void blinkLED(int timeout)
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(timeout);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2 * timeout);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  strip.begin();

  // Start the serial communication to send messages to the computer
  Serial.begin(SERIAL_FREQ);
  delay(10);
  Serial.println("\r\n");

  startWiFi();
  startOTA();
  startSPIFFS();
  startWebSocket();
  startMDNS();
  startServer();

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  webSocket.loop();
  server.handleClient();
  ArduinoOTA.handle();
  strip.Update();

}

// Setup functions
void startWiFi()
{
  WiFi.softAP(apSsid, apPassword); // Start the AP
  Serial.print("Access Point \"");
  Serial.print(apSsid);
  Serial.println("\" started\r\n");

  // Adding some APs to connect
  wifiMulti.addAP("AP", "password");

  Serial.println("Connecting");
  // Wait for the Wi-Fi to connect
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");

  // If the ESP is connected to an AP, print out AP name and received IP
  if (WiFi.softAPgetStationNum() == 0) {
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());
  } else {
    Serial.print("Station connected to ");
    Serial.print(apSsid);
    Serial.print(" AP");
  }
  Serial.println("\r\n");
}

// TODO implement OTA
void startOTA()
{}

void startSPIFFS()
{
  SPIFFS.begin();
  Serial.println("SPIFFS started. Contents:");
  { // list all content
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

void startWebSocket()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
}

void startMDNS()
{
  MDNS.begin(mdnsName);
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer()
{
  server.on("/edit.html",  HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

// server handlers
void handleNotFound()
{
  if (!handleFileRead(server.uri()))
    server.send(404, "text/plain", "404: File Not Found");
}

bool handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";           // send index.html if folder was requested
  String contentType = getContentType(path);              // get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;
}

void handleFileUpload()
{}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght)
{
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
    {
      IPAddress client_ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n",
                    num, client_ip[0], client_ip[1], client_ip[2],
                    client_ip[3], payload);
    }
    break;
  case WStype_TEXT:
    Serial.printf("[%u] get text: %s\n", num, payload);
    if (payload[0] == '#') { //Just glow with selected color
      Serial.printf("GLOW with ");
      strip.Glow(getColorFromPayload(payload));
    } else if (payload[0] == 'F') { //Fade effect
      Serial.printf("FADE with ");
      strip.Fade(getColorFromPayload(payload), strip.Color(0, 0, 0), 50, 100);
    } else if (payload[0] == 'R') { // Rainbow effect
      Serial.printf("RAINBOW");
      strip.RainbowCycle(60);
    } else if (payload[0] == 'N') { // Shutdown LED
      Serial.printf("Turn LED off");
      strip.Glow(strip.Color(0, 0, 0));
    }
    break;
  }
  blinkLED(5);
}

/* Helpers */
/* Convert sizes in bytes to KB and MB*/
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
    return String(bytes) + "B";
  else if (bytes < (1024 * 1024))
    return String(bytes / 1024.0) + "KB";
  else if (bytes < (1024 * 1024 * 1024))
    return String(bytes / 1024.0 / 1024.0) + "MB";
}

/* Return content type */
String getContentType(String filename)
{
  // return MIME type based on file extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

/* Extract color from websocket payload */
uint32_t getColorFromPayload(uint8_t* payload)
{
  uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
  int r = ((rgb >> 20) & 0x3FF);            // 10 bits per color, so R: bits 20-29
  int g = ((rgb >> 10) & 0x3FF);            // G: bits 10-19
  int b = rgb & 0x3FF;                      // B: bits  0-9
  Serial.printf("extracted R:%d G:%d B:%d\n", r, g ,b);
  return strip.Color(r, g, b);
}

