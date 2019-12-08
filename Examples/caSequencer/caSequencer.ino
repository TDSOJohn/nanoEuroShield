// v0.1.1

const byte linPotPin = A0;                                                             //Lin Pot
const byte latchPin = 8;                                                               //ST_CP
const byte dataPin = 11;                                                               //DS
const byte clockPin = 12;                                                              //SH_CP

byte ca_seq_old = 0;
byte ca_seq_cur = 0;
int linpot_val = 0;

int bpm = 110;
byte outputNormalizer[8] = {0, 1, 2, 4, 5, 6, 3, 7};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  Serial.begin(9600);

  digitalWrite(latchPin, 0);
  ca_seq_old = rand() %255;
  ca_seq_cur = ca_seq_old;                                                      //0b00001000
  shiftOut(ca_seq_cur);
  digitalWrite(latchPin, 1);
}

void loop() {
  linpot_val = analogRead(linPotPin);
  bpm = linpot_val + 20;                                                        //Bpm range = 20...275
  ca_seq_old = ca_seq_cur;
  ca_seq_cur = sequenceUpdate(ca_seq_old);
  digitalWrite(latchPin, 0);
  shiftOut(ca_seq_cur);
  digitalWrite(latchPin, 1);
  delay(int(60000/bpm));
}

byte sequenceUpdate(byte dataIn) {
  byte dataOut = 0;
  bool p, q, r;

  for(int i=7; i>=0; i--) {
    if(!i || i==7) {
      p = (dataIn & (1<<7));
      q = (dataIn & (1));
    } else {
      p = (dataIn & (1<<(i-1)));
      r = (dataIn & (1<<(i+1)));
    }
    if((p || r) && ( !(p && r))) {
      dataOut += 1;
    }
    if(dataOut)
      dataOut <<= 1;
  }
  Serial.println();
  Serial.println(dataOut, BIN);
  return dataOut;
}

void shiftOut(byte dataOut) {
  int pinState;

  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);

  for (int i=7; i>=0; i--)  {
    digitalWrite(clockPin, 0);

    if ( dataOut & (1<<(outputNormalizer[i])) ) {
      pinState= 1;
    }
    else {
      pinState= 0;
    }
    digitalWrite(dataPin, pinState);
    digitalWrite(clockPin, 1);
    digitalWrite(dataPin, 0);
  }

  digitalWrite(clockPin, 0);
}
