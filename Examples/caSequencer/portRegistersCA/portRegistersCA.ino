#include <Firmata.h>

//TOTAL_PORTS =3 on Arduino Nano
//TOTAL_PINS =22 on Arduino Nano
byte previousPin[TOTAL_PORTS];
byte previousPort[TOTAL_PORTS];

void pullDigitalInput(void)
{
    previousPort[1] =PIND;
}

void pushDigitalOutput(byte port, byte data)
{
    if(previousPin[port] !=data)
    {
        Firmata.sendDigitalPort(port, data);
        previousPin[port] =data;
    }
}

void setup()
{
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION,
                            FIRMATA_FIRMWARE_MINOR_VERSION);
    Firmata.attach(DIGITAL_MESSAGE, pullDigitalInput);
    Firmata.begin(57600);

    for(int i =0; i <TOTAL_PORTS; i++)
        previousPin[i] =previousPort[i] =0;
    Serial.begin(57600);
    DDRD = DDRD | B11111100;
}

void loop()
{
    while(Firmata.available())
        Firmata.processInput();

    delay(200);
//    PORTD = B10101000;
    pushDigitalOutput(1, B10101000);
    pullDigitalInput();
    Serial.println(previousPort[1], BIN);
    delay(200);
//    PORTD = B01010100;
    pushDigitalOutput(1, B01010100);
    pullDigitalInput();
    Serial.println(previousPort[1], BIN);
}
