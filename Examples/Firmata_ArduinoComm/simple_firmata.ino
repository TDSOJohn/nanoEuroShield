#include <Firmata.h>

byte analogPin;
byte previousPIN[TOTAL_PORTS];
byte previousPORT[TOTAL_PORTS];

//Digital output function, only sends data when it changes
void digitalOutputPort(byte portNumber, byte portValue)
{
    if(previousPIN[portNumber] !=portValue)
    {
        Firmata.sendDigitalPort(portNumber, portValue);
        previousPIN[portNumber] =portValue;
    }
}

void setPinModeCallback(byte pin, int mode)
{
    if(IS_PIN_DIGITAL(pin))
        pinMode(PIN_TO_DIGITAL(pin), mode);
}

void digitalWriteCallback(byte port, int value)
{
    byte i;
    byte currentPinValue, previousPinValue;

    if(port <TOTAL_PORTS && value !=previousPORT[port])
    {
        for(i =0; i <8; i++)
        {
            currentPinValue =(byte)value &(1 <<i);
            previousPinValue =previousPORT[port] &(1 <<i);
            if(currentPinValue !=previousPinValue)
                digitalWrite(i +(port *8), currentPinValue);
        }
        previousPORT[port] =value;
    }
}
void analogWriteCallback(byte pin, int value)
{
    pinMode(pin, OUTPUT);
    analogWrite(pin, value);
}

void setup()
{
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
    Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
    Firmata.attach(SET_PIN_MODE, setPinModeCallBack);
//    Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
    Firmata.begin(57600);
}

void loop()
{
    byte i;

    while(Firmata.available())
        Firmata.processInput();

    for(i =0; i <TOTAL_PORTS; i++)
        digitalOutputPort(i, readPort(i, 0xFF));
/*
    for(analogPin =0; analogPin <TOTAL_ANALOG_PINS; analogPin++)
        Firmata.sendAnalog(analogPin, analogRead(analogPin));
*/
}
