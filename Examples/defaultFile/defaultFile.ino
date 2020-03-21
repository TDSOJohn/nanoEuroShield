#include <MCP492X.h>

#define SWTCH_1             2     //Manual Switch
#define SWTCH_2             3     //Manual Switch
#define GATE_2              4     //Gate I/O
#define PWM_1               5     //PWM Out + Fixed Lowpass (!DOES NOT GO TO FULL ZERO)
#define GATE_1              6     //Gate I/O
#define GATE_3              7     //Gate I/O
#define SPI_CHIP_SELECT_DAC 8     //DAC Pin Select for MCP4922

#define LOGPOT              A0    //Log Pot (Corner)
#define LINPOT_1            A1    //Lin Pot
#define LINPOT_2            A2    //Lin Pot (Middle)

MCP492X myDac(SPI_CHIP_SELECT_DAC);

int linpot_1_val = 0;
int linpot_2_val = 0;
int logpot_val = 0;

bool swtch_1_val = 0;
bool swtch_2_val = 0;

void setup() {
  myDac.begin();
  Serial.begin(9600);
  pinMode(GATE_1, OUTPUT);
  pinMode(GATE_2, OUTPUT);
  pinMode(GATE_3, OUTPUT);
  pinMode(PWM_1, OUTPUT);
  pinMode(SWTCH_1, INPUT);
  pinMode(SWTCH_2, INPUT);
}

void loop() {

}
