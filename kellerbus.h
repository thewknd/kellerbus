#include "WProgram.h"

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
    unsigned char RTS_PIN;
    unsigned short Timeout;
    
    
    unsigned char TxBuffer[COMM_TX_MAX];
    unsigned char RxBuffer[COMM_TX_MAX + COMM_RX_MAX];
    
    unsigned short Open();
    unsigned short Close();
    
    int TransferData(unsigned short nTX, unsigned short nRX);
    
    unsigned char cClass;
    unsigned char cGroup;
    unsigned char cYear;
    unsigned char cWeek;
    unsigned char cBuffer;
    unsigned char cState;
    
    unsigned long Serialnumber;
    
    unsigned char cDevice;
    
    float ch0;
    float chP1;
    float chP2;
    float chTOB1;
    float chTOB2;
    float chT;
    
    
  public:
    
    CKellerBus(HardwareSerial* mComm, unsigned long pBaudrate, unsigned char RTS);
    
    unsigned short initDevice();
    unsigned short initDevice(unsigned char Device);
    unsigned short readChannel(unsigned char Channel);
    unsigned short readSerialnumber();
     
    unsigned char getClass();
    unsigned char getGroup();
    unsigned char getYear();
    unsigned char getWeek();
    unsigned char getBuffer();
    unsigned char getState();
    unsigned char getDevice();
    unsigned long getSerialnumber();
    
    float getCH0();
    float getP1();
    float getP2();
    float getTOB1();
    float getTOB2();
    float getT();    
};
#endif
