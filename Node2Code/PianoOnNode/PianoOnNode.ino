#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <limits.h>
#include "pitches.h"

const char* ssid = "PianoNode";
const char* APpassword = "pianonode";

IPAddress staticIP(192, 168, 1, 69), gateway(192,168,1,1), subnet(255,255,255,0);
ESP8266WebServer server(80);
WebSocketsServer webSocketServer(81);

const int notes[24] = {NOTE_C4 ,NOTE_CS4,NOTE_D4 ,NOTE_DS4,NOTE_E4 ,NOTE_F4 ,NOTE_FS4,NOTE_G4 ,NOTE_GS4,NOTE_A4 ,NOTE_AS4,NOTE_B4,
                      NOTE_C5 ,NOTE_CS5,NOTE_D5 ,NOTE_DS5,NOTE_E5 ,NOTE_F5 ,NOTE_FS5,NOTE_G5 ,NOTE_GS5,NOTE_A5 ,NOTE_AS5,NOTE_B5};

const byte APPin = D7;
const byte buzzer = D2;
const byte recordPin = D1;
const byte playbackPin = D5;
const int bufferSize = 200;

byte curNote = -1;
bool recording = false;
bool playingBack = false;
bool recordingStarted = false;

int songLength = 0;
byte notesBuffer[bufferSize];
int durationsBuffer[bufferSize];
int playbackIdx = 0;

unsigned long delaySz = 0;
unsigned long lastNote = 0;

byte stob(const String s) {
  byte ret = 0;
  for(int i = 0; i < s.length(); i++) {
    ret = ret*10 + (s[i]-'0');
  }
  return ret;
}

void startHotspot() {
  WiFi.softAPConfig(staticIP, gateway, subnet);
  digitalWrite(APPin, WiFi.softAP(ssid, APpassword));
}

void handlePiano() {
  if (server.hasArg("index") && server.hasArg("type")) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200);
    
    if(playingBack) return;

    int index = stob(server.arg("index"));
    bool type = (server.arg("type") == "1");
    
    if(recordingStarted && songLength != bufferSize) {
      notesBuffer[songLength] = curNote;
      durationsBuffer[songLength] = millis() - lastNote;
      songLength++;
    }

    lastNote = millis();
    if(type) {
      curNote = index;
      tone(buzzer, notes[index]);
      if(recording)
        recordingStarted = true;
        
      return;
    }

    if(index == curNote) {
      curNote = -1;
      noTone(buzzer);
    }
  }
  else
    server.send(400);
}

void handleRecord() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if(playingBack) {
    server.send(400);
    return;
  }
  if(recording) {
    server.send(200, "text/plain", "0");
    recording = false;
    recordingStarted = false;
  }
  else {
    server.send(200, "text/plain", "1");
    recording = true;
    songLength = 0;
  }
  digitalWrite(recordPin, recording);
}

void handlePlayback() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if(recording) {
    server.send(400);
    return;
  }
  if(playingBack) {
    server.send(200, "text/plain", "0");
    playingBack = false;
    noTone(buzzer);
    webSocketServer.broadcastTXT("-1");
  }
  else {
    server.send(200, "text/plain", "1");
    playingBack = true;
    playbackIdx = 0;
    delaySz = 0;
  }
  digitalWrite(playbackPin, playingBack);
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String response = (recording ? "1" : "0");
  response += (playingBack ? "1" : "0");
  server.send(200, "text/plain", response);
}


void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(APPin, OUTPUT);
  pinMode(recordPin, OUTPUT);
  pinMode(playbackPin, OUTPUT);

  startHotspot();

  server.on("/", handleRoot);
  server.on("/piano", handlePiano);
  server.on("/record", handleRecord);
  server.on("/playback", handlePlayback);

  server.begin();
  webSocketServer.begin();
}

void playNext() {
  if(millis() - lastNote >= delaySz) {
    if(playbackIdx != songLength) {
      int note = notesBuffer[playbackIdx];
      webSocketServer.broadcastTXT(String(note).c_str());
      if(~note)
        tone(buzzer, notes[note]);
      else
        noTone(buzzer);
      delaySz = durationsBuffer[playbackIdx];
      lastNote = millis();
      playbackIdx++;
    }
    else {
      webSocketServer.broadcastTXT("-1");
      noTone(buzzer);
      lastNote = millis();
      delaySz = ULONG_MAX;
    }
  }
}


void loop() {
  server.handleClient();
  webSocketServer.loop();

  if(playingBack)
    playNext();
}
