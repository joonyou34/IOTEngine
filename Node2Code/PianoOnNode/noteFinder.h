struct NotePair {
  String noteName;
  int noteVal;

  NotePair(String noteName, int noteVal) : noteName(noteName), noteVal(noteVal) {}

  bool operator<(const NotePair& other) const {
    return noteName < other.noteName;
  }

  NotePair& operator=(const NotePair& other) {
    noteName = other.noteName;
    noteVal = other.noteVal;
    return *this;
  }
};

template<typename T>
void insertionSort(T arr[], int n) {
  for (int i = 1; i < n; i++) {
    T key = arr[i];
    int j = i - 1;

    // Move elements of arr[0..i-1] that are greater than the key
    // to one position ahead of their current position
    while (j >= 0 && key < arr[j]) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

const byte noteCt = 60;

NotePair noteMap[noteCt] = 
{
{"C2", 65},
{"CS2", 69},
{"D2", 73},
{"DS2", 78},
{"E2", 82},
{"F2", 87},
{"FS2", 93},
{"G2", 98},
{"GS2", 104},
{"A2", 110},
{"AS2", 117},
{"B2", 123},
{"C3", 131},
{"CS3", 139},
{"D3", 147},
{"DS3", 156},
{"E3", 165},
{"F3", 175},
{"FS3", 185},
{"G3", 196},
{"GS3", 208},
{"A3", 220},
{"AS3", 233},
{"B3", 247},
{"C4", 262},
{"CS4", 277},
{"D4", 294},
{"DS4", 311},
{"E4", 330},
{"F4", 349},
{"FS4", 370},
{"G4", 392},
{"GS4", 415},
{"A4", 440},
{"AS4", 466},
{"B4", 494},
{"C5", 523},
{"CS5", 554},
{"D5", 587},
{"DS5", 622},
{"E5", 659},
{"F5", 698},
{"FS5", 740},
{"G5", 784},
{"GS5", 831},
{"A5", 880},
{"AS5", 932},
{"B5", 988},
{"C6", 1047},
{"CS6", 1109},
{"D6", 1175},
{"DS6", 1245},
{"E6", 1319},
{"F6", 1397},
{"FS6", 1480},
{"G6", 1568},
{"GS6", 1661},
{"A6", 1760},
{"AS6", 1865},
{"B6", 1976}
};

void shenanigans() {
  insertionSort(noteMap, noteCt);
}

int getNote(String noteName) {
  int l = 0, r = noteCt-1;

  while(l <= r) {
    int mid = (l+r)>>1;
    if(noteMap[mid].noteName == noteName)
      return noteMap[mid].noteVal;
    
    if(noteMap[mid].noteName < noteName)
      l = mid+1;
    else
      r = mid-1;
  }
  return 0;
}