//v0.1

#include <MCP492X.h>
#include "HzVCVTable.h"

#define SWTCH_1             2                                                   //Manual Switch
#define SWTCH_2             3                                                   //Manual Switch
#define GATE_2              4                                                   //Gate I/O
#define PWM_1               5                                                   //PWM Out + Fixed Lowpass (!DOES NOT GO TO FULL ZERO)
#define GATE_1              6                                                   //Gate I/O
#define GATE_3              7                                                   //Gate I/O
#define SPI_CHIP_SELECT_DAC 8                                                   //DAC Pin Select for MCP4922

#define LOGPOT              A0                                                  //Log Pot (Corner)
#define LINPOT_1            A1                                                  //Lin Pot
#define LINPOT_2            A2                                                  //Lin Pot (Middle)
#define ULTRASON_TRIG       A5                                                  //Trigger pulse to ultrasonic sensor
#define ULTRASON_ECHO       A4                                                  //Echo from ultrasonic to Arduino

MCP492X myDac(SPI_CHIP_SELECT_DAC);

                                  //bpm to time_val: 60000/bpm

struct step {
  int note;                                                                     //0 - 4095 for 0 - 5V out
  byte CV;                                                                      //0 - 255 CV out via PWM & RC LowPass
  bool gate1;                                                                   //1 for Gate
  bool gate2;
  bool gate3;
  float gateDuty;                                                               //0 - 1 floating value = stepT/gateT
};

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

bool swtch_1_val = false;
bool swtch_2_val = false;
int linpot_1_val = 0;
int linpot_2_val = 0;
int logpot_val = 0;
int ultrason_duration;
int ultrason_distance;

const byte seq_length = 8;
byte seq_loop = seq_length;
int bpm = 110;
byte counter = 0;


step seq[seq_length];

void stepOut(step &p) {
  digitalWrite(GATE_1, p.gate1);
  digitalWrite(GATE_2, p.gate2);
  digitalWrite(GATE_3, p.gate3);
  if(p.gate2) {
    myDac.analogWrite(p.note);
    noteOn(0x90, p.note, 127);
  }
  analogWrite(PWM_1, p.CV);
}

void swtch1Update() {                                                           //General parameters
  linpot_1_val = analogRead(LINPOT_1);
  linpot_2_val = analogRead(LINPOT_2);
  logpot_val = analogRead(LOGPOT);

  seq_loop = linpot_1_val/128;
  if(seq_loop>7)  seq_loop=7;
  bpm = logpot_val + 20;
  for(int i=0; i<8; i++)
    seq[i].gate2 = bitRead(linpot_2_val, i);
}

void swtch2Update() {                                                           //Single step modifications

  linpot_1_val = analogRead(LINPOT_1);
  linpot_2_val = analogRead(LINPOT_2);
  logpot_val = analogRead(LOGPOT);

  byte modStep = linpot_1_val/128;
  if(modStep>7) modStep = 7;
  seq[modStep].CV = linpot_2_val / 4;
  seq[modStep].gateDuty = logpot_val/1023.f;
}

void setup() {
  Serial.begin(31250);
  myDac.begin();
  pinMode(GATE_1, OUTPUT);
  pinMode(GATE_2, OUTPUT);
  pinMode(GATE_3, OUTPUT);
  pinMode(PWM_1, OUTPUT);
  pinMode(SWTCH_1, INPUT);
  pinMode(SWTCH_2, INPUT);

  for(int i=0; i<8; i++) {
    seq[i].note = 0;
    seq[i].CV = 0;
    seq[i].gate1 = 0;
    seq[i].gate2 = 0;
    seq[i].gate3 = 0;
    seq[i].gateDuty = 0.5;
  }
  seq[0].note = MIDI::c2;
  seq[1].note = MIDI::ds2;
  seq[2].note = MIDI::c2;
  seq[3].note = MIDI::d2;
  seq[4].note = MIDI::f2;
  seq[5].note = MIDI::g2;
  seq[6].note = MIDI::f2;
  seq[7].note = MIDI::c3;

  seq[0].gate3 = HIGH;
}

void loop() {
  swtch_1_val = digitalRead(SWTCH_1);
  swtch_2_val = digitalRead(SWTCH_2);

  stepOut(seq[counter]);

  if(swtch_1_val==HIGH) {
    swtch1Update();
  } else if(swtch_2_val==HIGH) {
    swtch2Update();
  }

  delay(int(60000/bpm*seq[counter].gateDuty));
  noteOn(0x90, seq[counter].note, 0x00);
  delay(int(60000/bpm*(1-seq[counter].gateDuty)));
  counter++;
  counter = counter%seq_loop;
}
