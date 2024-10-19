#include "pitches.h"

const byte buzzer = D2;

struct Song {
  String notes, durations;
  int noteSpacing, delayBeforeRepeat;
  Song(String notes = "", String durations = "", int noteSpacing = 50, int delayBeforeRepeat = 50)
    : notes(notes), durations(durations){}
};

const byte songCapacity = 10;

Song songs[songCapacity] =
  {
    {"C4 D4 E4 G4 E4 D4 C4 0 G4 A4 G4 E4 D4 C4 ", "250 250 250 500 250 250 250 250 500 250 250 250 250 250 ", 50},
    {
      "E5 E5 0 E5 0 "
      "C5 E5 0 G5 0 G4 0 "
      "C5 0 G4 0 E4 0 "
      "A4 0 B4 0 AS4 A4 0 "
      "G4 E5 0 G5 0 A5 0 "
      "F5 G5 0 E5 0 C5 0 D5 B4 0 ",
      "125 125 25 125 50 "
      "125 125 25 125 200 125 200 "
      "125 125 125 125 125 125 "
      "125 25 125 50 125 125 125 "
      "125 125 25 125 50 150 50 "
      "125 125 50 125 50 125 75 125 125 125 ",
      50
    },
    {
      "E4 B3 C4 D4 C4 B3 A3 A3 C4 E4 D4 C4 B3 C4 D4 E4 C4 A3 A3 A3 B3 C4 D4 F4 A4 G4 F4 E4 C4 E4 D4 C4 B3 B3 C4 D4 E4 C4 A3 A3 ",
      "250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 125 125 250 ",
      350
    },
    {
      "E5 D5S E5 D5S E5 B4 D5 C5 A4 "
      "C4 E4 A4 B4 E4 G4S B4 C5 "
      "E4 E5 D5S E5 D5S E5 B4 D5 C5 A4 "
      "C4 E4 A4 B4 E4 C5 B4 A4 ",
      "250 250 250 250 250 250 250 250 500 "
      "250 250 250 250 250 250 250 500 "
      "250 250 250 250 250 250 250 250 500 "
      "250 250 250 250 250 250 250 500 ",
      75
    }
  };

bool onloop = false;
int songsNum = 4;
int songIdx = 0;
int noteIdx = -1, durationIdx = -1;

bool playingMusic = false;

unsigned long lastNoteTime = 0;
unsigned long lastNoteDelay = 0;

unsigned long stol(String s) {
  long ret = 0;
  for(int i = 0; i < s.length(); i++) {
    ret*=10;
    ret += s[i]-'0';
  }
  return ret;
}

String getNext(String& targ, int& idx) {
  if(idx+1 == targ.length())
    return "0";
  String ret = "";
  while(targ[++idx] != ' ')
    ret += targ[idx];
  
  return ret;
}

void playNextNote() {
  unsigned long duration = stol(getNext(songs[songIdx].durations, durationIdx));
  tone(buzzer, getNote(getNext(songs[songIdx].notes, noteIdx)), duration);
  lastNoteDelay = duration + songs[songIdx].noteSpacing;
  lastNoteTime = millis();
}

void playMusic() {
  if(millis() - lastNoteTime > lastNoteDelay) {
    if(noteIdx+1 != songs[songIdx].notes.length())
      playNextNote();
    else {
      noteIdx = -1, durationIdx = -1;
      lastNoteTime = millis();
      lastNoteDelay = songs[songIdx].delayBeforeRepeat;

      if(!onloop) {

        if(songIdx >= songsNum)
          songIdx = 0;
        else
          songIdx++;

      }
    }
  }
}