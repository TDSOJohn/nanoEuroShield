// v0.1

int linPotPin = A0;                                                             //Lin Pot
int latchPin = 8;                                                               //ST_CP
int dataPin = 11;                                                               //DS
int clockPin = 12;                                                              //SH_CP

byte ca_seq_old = 0;
byte ca_seq_cur = 0;
int linpot_val = 0;

const byte seq_length = 8;
byte seq_loop = seq_length;
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
  Serial.println();
  Serial.println(dataIn, BIN);
  bool p, q, r;

  for(int i=6; i>0; i--) {
    p = (dataIn & (1<<(i-1)));
    r = (dataIn & (1<<(i+1)));
    if((p || r) && ( !(p && r))) {
      dataOut += 1;
      Serial.print(" dataOut +=1 at ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(dataOut, BIN);
      }
    if(dataOut) {
      dataOut <<= 1;
      Serial.print(" Shifting at ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(dataOut, BIN);
    }
  }
  Serial.println();
  Serial.println(dataOut, BIN);
  return dataOut;
}

void shiftOut(byte dataOut) {
  int i=0;
  int pinState;

  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);

  for (i=7; i>=0; i--)  {
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
