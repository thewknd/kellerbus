/*
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <Arduino.h>

#ifndef _H_KELLERBUS
#define _H_KELLERBUS

#define COMM_ERR_BAD_CRC           -4
#define COMM_OK                     0

#define COMM_TX_MAX                 20
#define COMM_RX_MAX                 260

#define MAX_CHANNELS                32
#define MAX_UNITS                   15

class CKellerBus
{
  private:
    HardwareSerial *Comm;
    unsigned long Baudrate;
    byte RTS_PIN;
    byte Timeout;
    
    
    byte TxBuffer[COMM_TX_MAX];
    byte RxBuffer[COMM_TX_MAX + COMM_RX_MAX];
    
    byte Open();
    byte Close();
    
    byte TransferData(byte nTX, byte nRX);
    
    byte cClass;
    byte cGroup;
    byte cYear;
    byte cWeek;
    byte cBuffer;
    byte cState;
    
    unsigned long Serialnumber;
    
    byte cDevice;
    
    float ch0;
    float chP1;
    float chP2;
    float chTOB1;
    float chTOB2;
    float chT;
    
    
  public:
    
    CKellerBus(HardwareSerial* mComm, unsigned long pBaudrate, byte RTS);
    
    byte initDevice();
    byte initDevice(byte Device);
    byte readChannel(byte Channel);
    byte readSerialnumber();
     
    byte getClass();
    byte getGroup();
    byte getYear();
    byte getWeek();
    byte getBuffer();
    byte getState();
    byte getDevice();
    unsigned long getSerialnumber();
    
    float getCH0();
    float getP1();
    float getP2();
    float getTOB1();
    float getTOB2();
    float getT();    
};
#endif
