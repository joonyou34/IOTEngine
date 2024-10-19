#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include "SoundSystem.h"
#include <limits.h>


const char* ssid = "Engine";
const char* APpassword = "engine123";

IPAddress staticIP(192, 168, 1, 69), gateway(192,168,1,1), subnet(255,255,255,0);

ESP8266WebServer server(80);

void startHotspot() {
  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ssid, APpassword);

  Serial.println("hotspot on");
}

bool engineState = false;

void handleGameEngine() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("player") && server.hasArg("button") && server.hasArg("state")) {
    String player = server.arg("player");
    String button = server.arg("button");
    String state = server.arg("state");

    String currentButton = player + button + state;

    Serial.print("/E");
    Serial.println(currentButton);

    server.send(200);
  }
  else {
    server.send(400, "text/plain", "No Input");
  }
}

void handleToggleEngine() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  engineState = !engineState;
  Serial.print("/P");
  Serial.println(engineState);

  server.send(200);
}



void handleNextTrack() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  songIdx++;
  songIdx%=songsNum;
  noteIdx = -1, durationIdx = -1;

  server.send(200);
}

void handleLoop() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  onloop = !onloop;

  server.send(200, "text/plain", String((char)('0'+onloop)));
}

void handleMusicState() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  playingMusic = !playingMusic;

  server.send(200, "text/plain", String((char)('0'+playingMusic)));
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  startHotspot();

  Serial.println("test");

  shenanigans();

  
  server.on("/toggleEngine", handleToggleEngine); 
  server.on("/gameEngine", handleGameEngine);
  server.on("/nextTrack", handleNextTrack);
  server.on("/musicState", handleMusicState); 
  server.on("/loop", handleLoop);

  server.begin();
}


void loop() {
  server.handleClient();

  if(playingMusic) {
    playMusic();
  }
  else
    noTone(buzzer);
}