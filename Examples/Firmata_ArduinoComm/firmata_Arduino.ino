#include <Wire.h>
#include <Firmata.h>

#define I2C_WRITE                   B00000000
#define I2C_READ                    B00001000
#define I2C_READ_CONTINUOUSLY       B00010000
#define I2C_STOP_READING            B00011000
#define I2C_READ_WRITE_MODE_MASK    B00011000
#define I2C_10BIT_ADDRESS_MODE_MASK B00100000
#define I2C_END_TX_MASK             B01000000
#define I2C_STOP_TX                 1
#define I2C_RESTART_TX              0
#define I2C_MAX_QUERIES             8   //Max i2c queries stored
#define I2C_REGISTER_NOT_SPECIFIED  -1

//The minimum interval for sampling analog input
#define MINIMUM_SAMPLING_INTERVAL   1

#ifdef FIRMATA_SERIAL_FEATURE
SerialFirmata serialFeature;
#endif

//Analog inputs
int analogInputsToReport =0;            //bitwise array to store pin reporting

//Digital input ports
byte reportPINs[TOTAL_PORTS];
byte previousPINs[TOTAL_PORTS];

//Pins configuration
byte portConfigInputs[TOTAL_PORTS];

//Time variables
unsigned long currentMillis;
unsigned long previousMillis;
unsigned int samplingInterval =19;      //ms between main loop cycles

struct i2c_device_info
{
    byte addr;
    int  reg;
    byte bytes;
    byte stopTX;
};

i2c_device_info query[I2C_MAX_QUERIES]; //i2c data array

byte i2cRxData[64];
bool isI2CEnabled =false;
signed char queryIndex = -1;
unsigned int i2cReadDelayTime =0;       //Delay time between i2c read request and Wire.requestfrom()

bool isResetting =false;

void wireWrite(byte data)
{
    #if ARDUINO >= 100
        Wire.write((byte)data);
    #else
        Wire.send(data);
    #endif
}

byte wireRead(void)
{
    #if ARDUINO >= 100
        return Wire.read();
    #else
        return Wire.receive();
    #endif
}

void checkDigitalInputs(void)
{

}

void enableI2CPins()
{
    byte i;
    for(i =0; i <TOTAL_PINS; i++)
    {
        if(IS_PIN_I2C(i))
            setPinModeCallBack(i, PIN_MODE_I2C);
    }
}

void disableI2CPins()
{

}

void setup()
{
    //Set firmware name and version, using the sketch's filename, minus the ".ino":
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);

    //Attach functions to various incoming message types
    Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
    Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
    Firmata.attach(REPORT_ANALOG, reportAnalogCallback);
    Firmata.attach(REPORT_DIGITAL, reportDigitalCallback);
    Firmata.attach(SET_PIN_MODE, setPinModeCallback);
    Firmata.attach(SET_DIGITAL_PIN_VALUE, setPinValueCallback);
    Firmata.attach(START_SYSEX, sysexCallback);
    Firmata.attach(SYSTEM_RESET, systemResetCallback);

    Firmata.begin(57600);       //Start the library and override default baud rate
    while(!Serial) { ; }        //Wait for Serial port to connect

    systemResetCallBack();      //reset to default config
}

void loop()
{
    byte pin, analogPin;

    checkDigitalInputs();

    while(Firmata.available())
        Firmata.processInput();
}
