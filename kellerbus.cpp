#include "kellerbus.h"

// kellerbus.cpp

CKellerBus::CKellerBus(HardwareSerial* mComm, unsigned long pBaudrate,unsigned char RTS){
  
  Baudrate = pBaudrate;
  Timeout = 100;
  Comm = mComm;
  RTS_PIN = RTS;
  
  pinMode(RTS_PIN,OUTPUT);
  digitalWrite(RTS_PIN,LOW);
  
  cClass = -1;
  cGroup = -1;
  cYear = -1;
  cWeek = -1;
  cBuffer = -1;
  cState = -1;
  ch0 = -1;
  chP1 = -1;
  chP2 = -1;
  chTOB1 = -1;
  chTOB2 = -1;
  chT = -1;
}
unsigned short CKellerBus::Open()
{
  Comm->begin(Baudrate);
  delay(2);  
  
  return 1;
}
unsigned short CKellerBus::initDevice(unsigned char Device = 250) 
{
  unsigned short ret;
  Open();
  cDevice = Device;
  TxBuffer[0] = cDevice;
  TxBuffer[1] = 0b01111111 & 48;
  
  if(TransferData(2,10) == COMM_OK) {
    cClass = RxBuffer[2];
    cGroup = RxBuffer[3];
    cYear = RxBuffer[4];
    cWeek = RxBuffer[5];
    cBuffer = RxBuffer[6];
    cState = RxBuffer[7];
    ret = COMM_OK;
  } else {
    ret = COMM_ERR_BAD_CRC;
  }
  Close();
  return ret;
}
unsigned short CKellerBus::initDevice() 
{
  unsigned short ret;
  Open();
  TxBuffer[0] = cDevice;
  TxBuffer[1] = 0b01111111 & 48;
  
  if(TransferData(2,10) == COMM_OK) {
    cClass = RxBuffer[2];
    cGroup = RxBuffer[3];
    cYear = RxBuffer[4];
    cWeek = RxBuffer[5];
    cBuffer = RxBuffer[6];
    cState = RxBuffer[7];
    ret = COMM_OK;
  } else {
      // 2nd try for sleeping dcx
      delay(5);
      TxBuffer[0] = cDevice;
      TxBuffer[1] = 0b01111111 & 48;
      if(TransferData(2,10) == COMM_OK) {
      cClass = RxBuffer[2];
      cGroup = RxBuffer[3];
      cYear = RxBuffer[4];
      cWeek = RxBuffer[5];
      cBuffer = RxBuffer[6];
      cState = RxBuffer[7];
      ret = COMM_OK;
    } else {
      ret = COMM_ERR_BAD_CRC;
    }
  }
  Close();
  return ret;
}
unsigned short CKellerBus::readChannel(unsigned char Channel)
{
  unsigned char bteArr[4];
  float value;
  unsigned short ret;
  
  Open();
  
  TxBuffer[0] = cDevice;
  TxBuffer[1] = 0b01111111 & 73;
  TxBuffer[2] = Channel;
  if(TransferData(3,9) == COMM_OK) {
    
    bteArr[0] = RxBuffer[5];
    bteArr[1] = RxBuffer[4];
    bteArr[2] = RxBuffer[3];
    bteArr[3] = RxBuffer[2];
     
    value = *(float*)(&bteArr[0]);
    
    switch(Channel) {
      case 0: ch0 = value; break;
      case 1: chP1 = value; break;  
      case 2: chP2 = value; break;  
      case 4: chTOB1 = value; break;  
      case 5: chTOB2 = value; break;  
      case 3: chT = value; break;  
      default:break;
      ret = COMM_OK;
    }

  } else {
    ret = COMM_ERR_BAD_CRC;
  }
  Close();
  return ret;
}
int CKellerBus::TransferData(unsigned short nTX, unsigned short nRX) 
{
  unsigned int Crc; 
  unsigned char n, m, x,CRC_H,CRC_L,delay_cnt;
  int ret;
  unsigned long b=0;
  
  // Clear RxBuffer;
  for(b = 0; b < COMM_TX_MAX + COMM_RX_MAX; b++) {
    RxBuffer[b] = 0;
  }
  b = 0;
  
  // initialisation CRC16
  Crc= 0xFFFF; 
  m= nTX; 
  x= 0; 
  
  // loop over all bits 
  while(m>0) {
    Crc^= TxBuffer[x]; 
    for(n=0; n<8; n++) {
      if(Crc&1) {
        Crc>>= 1; 
        Crc^= 0xA001;
      } else {
        Crc>>= 1;
      }
    } 
    m--; 
    x++;
  } // result 
  CRC_H= (Crc>>8)&0xFF; 
  CRC_L= Crc&0xFF;  
  // End CRC16
  
  
  TxBuffer[nTX] = CRC_H;
  TxBuffer[nTX+1] = CRC_L;
  
  digitalWrite(RTS_PIN,HIGH);
  delay(3);
  Comm->write(TxBuffer,nTX + 2);
  delay(2);
  digitalWrite(RTS_PIN,LOW);  
    
  delay_cnt = 0;
    
  do {
    if (Comm->available() > 0) {
      RxBuffer[b] = Comm->read(); 
      b++;
    }      
    delay(1);  
    delay_cnt += 1;   
  } while(delay_cnt <= Timeout); // timeout max 105ms
  delay(2);
  
  if(b == nRX) {
    ret = COMM_OK;             
  } else {
    ret = COMM_ERR_BAD_CRC;   
  }

  return ret;
}

unsigned char CKellerBus::getClass() {
  return cClass;
}
unsigned char CKellerBus::getGroup() {
  return cGroup;
}
unsigned char CKellerBus::getYear() {
  return cYear;
}
unsigned char CKellerBus::getWeek() {
  return cWeek;
}
unsigned char CKellerBus::getBuffer() {
  return cBuffer;
}
unsigned char CKellerBus::getState() {
  return cState;
}
unsigned char CKellerBus::getDevice() {
  return cDevice;
}
float CKellerBus::getCH0() {
  return ch0;
}
float CKellerBus::getP1() {
  return chP1;
}
float CKellerBus::getP2() {
  return chP2;
}
float CKellerBus::getTOB1() {
  return chTOB1;
}
float CKellerBus::getTOB2() {
  return chTOB2;
}
float CKellerBus::getT() {
  return chT;
}

unsigned short CKellerBus::Close()
{
  Comm->end();
  delay(2);  
  
  return 1;
}
