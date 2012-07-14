/*

@author thewknd
@date 7.2012

@copyright
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/


#include <kellerbus.h>

/**
  @brief sets up the hardwareserial interface to the transmitter.
  @constructor
  @param serialPort Hardwareserial port.
  @param _baudrate Baudrate for the hardwareserial port, usually 9600.
  @param _rts Ready To Send Pin.
  @param _timeout Communication timeout, in milliseconds, usually 100 (250 for DCX).
*/

CKellerBus::CKellerBus(HardwareSerial* serialPort, uint16_t _baudrate, uint8_t _rts, uint16_t _timeout)
{
  baudrate = _baudrate;
  timeout = _timeout;
  hwSerial = serialPort;
  RTS_PIN = _rts; 
  useHWSerial = true;
  
  pinMode(RTS_PIN,OUTPUT);
  digitalWrite(RTS_PIN,LOW);
}

/**
  @brief sets up the softserial interface to the transmitter.
  @constructor
  @param serialPort SoftwareSerial instance.
  @param _baudrate Baudrate for the SoftwareSerial port, usually 9600.
  @param _rts Ready To Send Pin.
  @param _timeout Communication timeout, in milliseconds, usually 100 (250 for DCX).

  *note*
  - Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
  - Not all pins on the Leonardo support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
*/

CKellerBus::CKellerBus(SoftwareSerial* serialPort, uint16_t _baudrate, uint8_t _rts, uint16_t _timeout)
{
  baudrate = _baudrate;
  timeout = _timeout;
  RTS_PIN = _rts;
  
  useHWSerial = false;

  swSerial = serialPort;
  
  pinMode(RTS_PIN,OUTPUT);
  digitalWrite(RTS_PIN,LOW);
}

/**
  @brief Device initialization. Wrapper for F48.
  @param _device device address.  
*/

void CKellerBus::initDevice(uint8_t _device) 
{
  device = _device;
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 48;

  TransferData(2,10);
  if (Error != RS_OK) {
    // 2nd try for sleeping dcx
    delay(30);
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 48;
    TransferData(2,10);
  } 
}

/**
  @brief Device initialization. Wrapper for F48.
  @param _device Device address.  
  @param _class Pointer for the device class. 
  @param _group Pointer for the device group.
  @param _year Pointer for the software version (year).
  @param _week Pointer for the software version (week).
  @param _buffer Buffer size.
  @param _state Device state.    
*/

void CKellerBus::initDevice(uint8_t _device, uint8_t* _class, uint8_t* _group, uint8_t* _year, uint8_t* _week, uint8_t* _buffer, uint8_t* _state) 
{
  device = _device;
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 48;

  TransferData(2,10);
  if (Error != RS_OK) {
    // 2nd try for sleeping dcx
    delay(30);
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 48;
    TransferData(2,10);
  }
  
  if (Error == RS_OK) {
    *_class = RxBuffer[2];
    *_group = RxBuffer[3];
    *_year = RxBuffer[4];
    *_week = RxBuffer[5];
    *_buffer = RxBuffer[6];
    *_state = RxBuffer[7];
  } 
}

/**
  @brief Returns the serialnumber.
  @return serialnumber 
*/

uint32_t CKellerBus::getSerialnumber(void) 
{
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 69;

  TransferData(2,8);
  
  if (Error == RS_OK) {
    // Serialnumber calculation, see protocol documentation
    return 256*65536*(uint32_t)RxBuffer[2] + 65536*(uint32_t)RxBuffer[3] + 256*(uint32_t)RxBuffer[4] + (uint32_t)RxBuffer[5];
  } else {
    return -1;
  }
}

/**
  @brief Returns the channel value. Wrapper function F73. 
  @param  Channel Index of the channel.

  _available values for "Channel"_

  - CH_0                        
  - CH_P1                       
  - CH_P2                       
  - CH_T                        
  - CH_TOB1                     
  - CH_TOB2                     
                 
  @return Channel value.
*/

float CKellerBus::readChannel(uint8_t Channel)
{
  uint8_t bteArr[4];
  float value;
  if ((Channel < MAX_CHANNELS) && (Channel >= 0) ) {

    // Prepare TxBuffer
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 73;
    TxBuffer[2] = Channel;

    TransferData(3,9);
    
    if(Error == RS_OK) {
      bteArr[0] = RxBuffer[5];
      bteArr[1] = RxBuffer[4];
      bteArr[2] = RxBuffer[3];
      bteArr[3] = RxBuffer[2];

      return *(float*)(&bteArr[0]);
      
    } else {
      return -1000;
    }
  } else {
    Error = SW_INVALIDPARAM;
    return -1000; 
  }
}  

/**
  @brief Returns the scaling value. Wrapper function F30. 
  @param no Index of the scaling value.

  _available values for "no"_

  - OFFSET_P1               
  - GAIN_P1         
  - OFFSET_P2                   
  - GAIN_P2                     
  - OFFSET_ANALOG               
  - GAIN_ANALOG                 
  - OFFSET_CH0                  
  - GAIN_CH0                    
  - MIN_P1                      
  - MAX_P1                      
  - MIN_P2                      
  - MAX_P2                      
  - MIN_T                       
  - MAX_T                       
  - MIN_TOB1                    
  - MAX_TOB1                    
  - MIN_TOB2                    
  - MAX_TOB2                    
  - MIN_CH0                     
  - MAX_CH0                     
  - MIN_ANALOG                  
  - MAX_ANALOG   
                 
  @return Scaling value.
*/


float CKellerBus::readScalingValue(uint8_t no)
{
  uint8_t bteArr[4];
  float value;
  if ((no <= 95) && (no >= 53) ) {

    // Prepare TxBuffer
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 30;
    TxBuffer[2] = no;

    TransferData(3,8);
    
    if(Error == RS_OK) {
      bteArr[0] = RxBuffer[5];
      bteArr[1] = RxBuffer[4];
      bteArr[2] = RxBuffer[3];
      bteArr[3] = RxBuffer[2];
      return *(float*)(&bteArr[0]);
    } else {
      return -1;
    }
    
  } else {
    Error = SW_INVALIDPARAM;
    return -1;  
  }
}
  
/**
  @brief Sets the device address. 
  @param newAddress The new device address.
*/

void CKellerBus::writeDeviceAddress(uint8_t newAddress)
{
  if ((newAddress <= 250) && (newAddress >= 0) ) {

    // Prepare TxBuffer
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 66;
    TxBuffer[2] = newAddress;

    TransferData(3,5);
    
    if(Error == RS_OK) {
      device = RxBuffer[2];
    }
  } else {
    Error = SW_INVALIDPARAM;
  }
}  

/**
  @brief Read configuration, Wrapper function F100.
  @param CFG_P Pointer for the register CFG_P.
  @param CFG_T Pointer for the register CFG_T.
  @param CNT_T Pointer for the register CNT_T.
*/

void CKellerBus::readConfiguration(uint8_t* CFG_P, uint8_t* CFG_T, uint8_t* CNT_T)
{
  
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 100;
  TxBuffer[2] = 2;

  TransferData(3,9);
  
  if(Error == RS_OK) {
    *CFG_P = RxBuffer[2];
    *CFG_T = RxBuffer[3];
    *CNT_T = RxBuffer[6];
  }
} 

/**
  @brief Reads out the speciefied part from the record page
*/

int8_t CKellerBus::getRecordPageContent(uint16_t pageAddress, uint16_t offset, uint8_t* datatype,  uint8_t* channel, float* measurement, uint16_t* timegap, char* desc)
{
  uint8_t buf0, buf1, buf2, buf3, bteArr[4]; 

  F67(pageAddress, offset, 4, &buf0, &buf1, &buf2, &buf3);

  if(Error == RS_OK) {
    if((buf0 & 0xF0) != 0xF0) {
      *channel = (buf0 & 0xF0) >> 4;
      *timegap = buf0 & 0x0F;

      bteArr[0] = 0;
      bteArr[1] = buf3;
      bteArr[2] = buf2;
      bteArr[3] = buf1;
      *measurement = *(float*)(&bteArr[0]);  
      /*Serial.println("\n\rDBG VALUE");
      Serial.println(bteArr[0],BIN);
      Serial.println(bteArr[1],BIN);
      Serial.println(bteArr[2],BIN);
      Serial.println(bteArr[3],BIN);
      Serial.println(*(float*)(&bteArr[0]));*/

      *datatype = 0;

    } else if(buf0 == 0xF0) {
      *timegap = (256 * buf2) + buf3;

      /*Serial.println("\n\rDBG TimEGAP");
      Serial.println(buf1,BIN);
      Serial.println(buf2,BIN);
      Serial.println(buf3,BIN);*/

      *datatype = 1;

    } else if(buf0 == 0xF4) {
      desc[0] = buf1;
      desc[1] = buf2;
      desc[2] = buf3;
      *datatype = 2;

    } else if(buf0 == 0xFF) {
      *datatype = 3;
    } else {
      *datatype = 4;
    }

    return 1;
  } else {
    return -1;
  }

}


/**
  @brief Reads the speciefied bytes from the record rom
  @param pageAddress Page address
  @param position Start position 0 .. 63
  @param cntBytes Amount of bytes to read 1 .. 4
  @param data0 First byte
  @param data1 Second byte
  @param data2 Third byte
  @param data3 Fourth byte
*/

void CKellerBus::F67(uint16_t pageAddress, uint8_t position, uint8_t cntBytes, uint8_t* data0, uint8_t* data1, uint8_t* data2, uint8_t* data3)
{
  // Prepare TxBuffer
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 67;
  TxBuffer[2] = highByte(pageAddress);
  TxBuffer[3] = lowByte(pageAddress);
  TxBuffer[4] = position;
  TxBuffer[5] = cntBytes;
  
  TransferData(6,4 + cntBytes); 
  
  if(Error == RS_OK) {
    *data0 = RxBuffer[2];
    *data1 = RxBuffer[3];  
    *data2 = RxBuffer[4];
    *data3 = RxBuffer[5]; 
  } 
} 

/**
  @brief --
  @return Start dedection bit
*/

int16_t CKellerBus::readStartDetection(uint16_t pageAddress)
{
  uint8_t buf0;

  F67(pageAddress,0,1,&buf0,0,0,0);

  if(Error == RS_OK) {
    return (buf0 & 0b10000000) >> 7; 
  } else {
    return -1;
  }
}

/**
  @brief --
  @return Start dedection bit
*/

int16_t CKellerBus::readOverflowCounter(uint16_t pageAddress)
{
  uint8_t buf0;

  F67(pageAddress,0,1,&buf0,0,0,0);

  if(Error == RS_OK) {
    return (buf0 & 0b01100000) >> 5; 
  } else {
    return -1;
  }
}

/**
  @brief Reads the time from the specified page
  @return Record time
*/

int16_t CKellerBus::readRecordPageStartPointer(uint16_t pageAddress)
{
  uint8_t buf0, buf1;

  F67(pageAddress,0,2,&buf0,&buf1,0,0);

  if(Error == RS_OK) {
    return (buf0 & 0b00011111) << 8 | buf1; 
  } else {
    return -1;
  }
}

/**
  @brief Reads the time from the specified page
  @return Record time
*/

uint32_t CKellerBus::readRecordPageTime(uint16_t pageAddress)
{
  uint32_t c1, c2, c3, c4; 
  uint8_t buf0, buf1, buf2, buf3;

  F67(pageAddress,2,4,&buf0,&buf1,&buf2,&buf3);

  if(Error == RS_OK) {
   
    c1 = 16777216UL * (uint32_t)buf0;
    c2 = 65536UL * (uint32_t)buf1;
    c3 = 256UL * (uint32_t)buf2;
    c4 = (uint32_t)buf3;

    return (946681200UL + c1 + c2 + c3 + c4 + 3600UL);
  
  } else {
    return -1;
  }
}

/**
  @brief Reads the speciefied bytes from the record configuration
  @param index Configuration index
*/

void CKellerBus::F92(uint8_t index, uint8_t* data0, uint8_t* data1, uint8_t* data2, uint8_t* data3, uint8_t* data4)
{
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 92;
  TxBuffer[2] = index;
  
  TransferData(3,9); 
  
  if(Error == RS_OK) {
    *data0 = RxBuffer[2];
    *data1 = RxBuffer[3];  
    *data2 = RxBuffer[4];
    *data3 = RxBuffer[5]; 
    *data4 = RxBuffer[6]; 
  }   
}

/**
  @brief Reads the speciefied bytes from the record configuration
  @param index Configuration index
*/

void CKellerBus::F93(uint8_t index, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 93;
  TxBuffer[2] = index;
  TxBuffer[3] = data0;
  TxBuffer[4] = data1;
  TxBuffer[5] = data2;
  TxBuffer[6] = data3;
  TxBuffer[7] = data4;
  
  TransferData(3,9);  
}

/**
  @brief Returns the device time
  @return Device time in seconds since 1.1 1970 (unix time)
*/

uint32_t CKellerBus::readDeviceTime(void)
{
  uint32_t c1, c2, c3, c4; 
  uint8_t buf0, buf1, buf2, buf3;

  F92(3,&buf0,&buf1,&buf2,&buf3,0);
  
  if(Error == RS_OK) {
   
    c1 = 16777216UL * (uint32_t)buf0;
    c2 = 65536UL * (uint32_t)buf1;
    c3 = 256UL * (uint32_t)buf2;
    c4 = (uint32_t)buf3;
     
    return (946681200UL + c1 + c2 + c3 + c4 + 3600UL);
  
  } else {
    return -1;
  }
} 

/**
  @brief Returns the record start time
  @return Record start time in seconds since 1.1 1970 (unix time)
*/

uint32_t CKellerBus::readRecordStartTime(void)
{
  uint32_t c1, c2, c3, c4;
  uint8_t buf0, buf1, buf2, buf3;

  F92(4,&buf0,&buf1,&buf2,&buf3,0);
  
  if(Error == RS_OK) {
   
    c1 = 16777216UL * (uint32_t)buf0;
    c2 = 65536UL * (uint32_t)buf1;
    c3 = 256UL * (uint32_t)buf2;
    c4 = (uint32_t)buf3;
     
    return (946681200UL + c1 + c2 + c3 + c4 + 3600UL);
  
  } else {
    return -1;
  }
} 

/**
  @brief Returns the actual record page address
  @return The  address of actual record page
*/

int16_t CKellerBus::readActualPageAddress(void)
{
  uint8_t buf0,buf1;

  F92(1,0,0,0,&buf0,&buf1);
  
  if(Error == RS_OK) {    
    return buf0 << 8 | buf1;
  } else {
    return -1;
  }
} 

/**
  @brief Returns the battery capacity.
  @return Battery caoacity in percent.
*/

int8_t CKellerBus::readBatCapacity(void)
{
  uint8_t buf0;

  F92(8,0,0,0,0,&buf0);
  
  if(Error == RS_OK) {
    return buf0; 
  } else {
    return -1;
  }
} 

/**
  @brief Returns the first record page address
  @return The first record page address
*/

int16_t CKellerBus::readRecRomFirstPagePhysik(void)
{
  uint8_t buf0,buf1;

  F92(2,&buf0,&buf1,0,0,0);
 
  if(Error == RS_OK) {
    return buf0<<8 | buf1;   
  } else {
    return -1;
  }
} 

/**
  @brief Returns the last record page address
  @return The last record page address 
*/

int16_t CKellerBus::readRecRomLastPagePhysik(void)
{
  uint8_t buf0,buf1;

  F92(2,0,0,&buf0,&buf1,0);
  
  if(Error == RS_OK) {    
    return buf0<<8 | buf1; 
  } else {
    return -1;
  }
} 

/**
  @brief Returns FUNC
  @return FUNC
*/

int16_t CKellerBus::readFUNC(void)
{
  uint8_t buf0;

  F92(0,&buf0,0,0,0,0);
  
  if(Error == RS_OK) {    
    return buf0;
  } else {
    return -1;
  }
} 

/**
  @brief Returns REC_CTRL
  @return REC_CTRL
*/

int16_t CKellerBus::readRECCTRL(void)
{
  uint8_t buf0;

  F92(1,0,&buf0,0,0,0);
  
  if(Error == RS_OK) {    
    return buf0;
  } else {
    return -1;
  }
} 

/**
  @brief Returns the record CFG
  @return Record CFG
*/

int16_t CKellerBus::readRECCFG(void)
{
  uint8_t buf0;

  F92(1,&buf0,0,0,0,0);
  
  if(Error == RS_OK) {    
    return buf0;
  } else {
    return -1;
  }
} 

/**
  @brief Returns EE_CTRL
  @return EE_CTRL
*/

int16_t CKellerBus::readEECTRL(void)
{
  uint8_t buf0;

  F92(1,0,0,&buf0,0,0);
  
  if(Error == RS_OK) {    
    return buf0;
  } else {
    return -1;
  }
} 

/**
  @brief Sets the device time.
  @param _day Days of the new date.
  @param _month Months of the new date.
  @param _year Years of the new date.
  @param _hour Hours of the new time.
  @param _minute Minutes of the new time.
  @param _second Seconds of the new time.
*/

void CKellerBus::writeDeviceTime(uint8_t _day, uint8_t _month, uint16_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second)
{
  uint32_t since2000,c0,c1,c2,c3;
  
  setTime(_hour, _minute, _second, _day, _month, _year); // hr - min - sec - day - month - year
  
  since2000 = now() - 946681200UL;
  
  c0 = since2000 / (uint32_t)pow(2UL,24UL);
  since2000 = since2000 % (uint32_t)pow(2UL,24UL);
  c1 = since2000 / (uint32_t)pow(2UL,16UL);
  since2000 = since2000 % (uint32_t)pow(2UL,16UL);
  c2 = since2000 / (uint32_t)pow(2UL,8UL);
  since2000 = since2000 % (uint32_t)pow(2UL,8UL);
  c3 = since2000;
  
  F93(3, c0, c1, c2, c3, 0); 
} 

/**
  @brief Transmits the TXBuffer to the device. 
  @param nTX Amount of bytes for transmission, without the checksum.
  @param nRX Amount of bytes for the response, with the checksum.
*/

void CKellerBus::TransferData(byte nTX, byte nRX) 
{
  uint16_t Crc, p, now, startTimeout; 
  uint16_t b = 0; // counts the incoming bytes

  Error = RS_OK;
  // Clear RxBuffer
  for(b = 0; b < COMM_TX_MAX + COMM_RX_MAX; b++) {
    RxBuffer[b] = 0;
  }
  b = 0;

  // generate checksum  
  Crc = checksum.CRC16(TxBuffer,nTX);
  TxBuffer[nTX]= (Crc>>8)&0xFF; 
  TxBuffer[nTX+1]= Crc&0xFF;  

  if (KB_DEBUG) {
    Serial.print("TX:");
    for(p = 0; p < nTX + 2; p++) {
      Serial.print(TxBuffer[p],DEC);
      Serial.print ("'") ;
    }
  }

  // Open the RS485 connection
  open();

  digitalWrite(RTS_PIN,HIGH);
  delay (1);

  if(useHWSerial) {
    if(hwSerial->write(TxBuffer,(int)(nTX + 2)) != (nTX + 2)) {
      Error = RS_TXERROR;
    }

    while (!(UCSR1A & (1 << UDRE1))) {
      UCSR1A |= 1 << TXC1;
    }
    while (!(UCSR1A & (1 << TXC1)));
  }


  if(!useHWSerial) {
    if(swSerial->write(TxBuffer,(int)(nTX + 2)) != (nTX + 2)) {
      Error = RS_TXERROR;
    }
  }
  
  digitalWrite(RTS_PIN,LOW);   

  if (KB_DEBUG) Serial.print(" -RX:");

  b = 0;  
  startTimeout = millis();
  do {
    
    // hardwareSerial as serialPort
    if(useHWSerial) {
      if (hwSerial->available() > 0) {
        // incoming byte
        RxBuffer[b] = hwSerial->read();
        if (KB_DEBUG) Serial.print(RxBuffer[b],DEC); 

        if (b == 0){

          // first step, check device address
          if(device == 250) {
            if ((RxBuffer[b] >= 1) && (RxBuffer[b] <= 250)) {
              // device address is valid
              b++;
            }
          } else if(RxBuffer[b] == device )  {
            // rx buffer and tx buffer have the same device address -> ok
            b++;    
          } else {
            // wrong device address
            if (KB_DEBUG) Serial.print(" -ERROR: DEVICE ADDRESS-");
          }
        } else if(b == 1) {
          // second step, check for function code
          // handles exception errors (communication protocol: 3.3.2.2 / Exception errors )

          if(RxBuffer[b] != (0x80 | TxBuffer[b])) { 

            if( RxBuffer[b] == TxBuffer[b]) {
              // the transmitted and the received function code are the same 
              b++;
            } else {
              // function code was wrong, go back to the first step
              b = 0;
              if (KB_DEBUG) Serial.print(" -ERROR: FUNCTION CODE-");
            }
          } else {
            // exception error flag is set
            b++;
            nRX = 3;
            if (KB_DEBUG) Serial.print(" -EXCEPTION- ");
          }
        } else {
         // step > 2
          b++;
        }

        if (KB_DEBUG) Serial.print("'");      

        startTimeout = millis();  
      }
    }

    // softwareSerial as serialPort
    if(!useHWSerial) {
      if (swSerial->available() > 0) {
        // incoming byte
        RxBuffer[b] = swSerial->read();
        if (KB_DEBUG) Serial.print(RxBuffer[b],DEC); 

        if (b == 0){

          // first step, check device address
          if(device == 250) {
            if ((RxBuffer[b] >= 1) && (RxBuffer[b] <= 250)) {
              // device address is valid
              b++;
            }
          } else if(RxBuffer[b] == device )  {
            // rx buffer and tx buffer have the same device address -> ok
            b++;    
          } else {
            // wrong device address
            if (KB_DEBUG) Serial.print(" -ERROR: DEVICE ADDRESS-");
          }
        } else if(b == 1) {
          // second step, check for function code
          // handles exception errors (communication protocol: 3.3.2.2 / Exception errors )

          if(RxBuffer[b] != (0x80 | TxBuffer[b])) { 

            if( RxBuffer[b] == TxBuffer[b]) {
              // the transmitted and the received function code are the same 
              b++;
            } else {
              // function code was wrong, go back to the first step
              b = 0;
              if (KB_DEBUG) Serial.print(" -ERROR: FUNCTION CODE-");
            }
          } else {
            // exception error flag is set
            b++;
            nRX = 3;
            if (KB_DEBUG) Serial.print(" -EXCEPTION- ");
          }
        } else {
         // step > 2
          b++;
        }
        if (KB_DEBUG) Serial.print("'");      

        startTimeout = millis();  
      }
    }      
    now = millis();    
  } while((b < nRX) && (now - startTimeout <= timeout) && (Error == RS_OK)); // timeout 

  if (now - startTimeout > timeout) { // checks fo timeout error
    Error = RS_TIMEOUT;
    if (KB_DEBUG) {
      Serial.print("\r\nb:");
      Serial.println(b);
      Serial.print("nRX:");
      Serial.println(nRX);
      Serial.print("diff:");
      Serial.println(now - startTimeout);
      Serial.print("Timeout:");
      Serial.println(timeout);
    }
  }

  if(Error == RS_OK) {
    // check the checksum
    Crc = checksum.CRC16(RxBuffer,b-2);

    if (KB_DEBUG) {
      Serial.print("\r\nCRC: HB:"); 
      Serial.print(highByte(Crc));
      Serial.print(" LB:");
      Serial.print(lowByte(Crc));
      Serial.println("");
    }

    if((highByte(Crc) != RxBuffer[b-2]) || (lowByte(Crc) != RxBuffer[b-1])) {
      // the calculated checksum differs from the received checksum
      Error = RS_BADCRC;
      if (KB_DEBUG)Serial.println("*** BAD CRC");
    } 
  }

  // if the exception flag was set
  if((RxBuffer[1] & 0x80)) {
    switch (RxBuffer[2]) {
    case 1 : 
      Error = DEVICE_NONFUNCTION; 
      break;
    case 2 : 
      Error = DEVICE_INCPARAMETERS; 
      break;
    case 3 : 
      Error = DEVICE_ERRONEOUSDATA; 
      break;
    case 32: 
      Error = DEVICE_INIT; 
      break;
    default: 
      Error = RS_ERROR; 
      break;
    }
  }  

  // Close the serialPort
  close();
  if (KB_DEBUG) Serial.print("Fehler:");
  if (KB_DEBUG) Serial.println(Error);
}

/**
  @brief Returns the value of channel CH0.
  @return Value of channel CH0.
*/

float CKellerBus::getCH0(void) 
{
  return readChannel(CH_0);
}

/**
  @brief Returns the value of channel P1.
  @param unit Pressure unit.
  @return Value of channel P1.
  @see For available values for "unit", see pressureConversion().
*/

float CKellerBus::getP1(uint8_t unit) 
{ 
  return pressureConversion(readChannel(CH_P1),unit);
}


/**
  @brief Returns the value of channel P2.
  @param unit Pressure unit.  
  @return Value of channel P2. 
  @see For available values for "unit", see pressureConversion().
*/

float CKellerBus::getP2(uint8_t unit) 
{
  return pressureConversion(readChannel(CH_P2),unit);
}


/**
  @brief Returns the value of channel TOB1.
  @param unit Temperature unit.
  @return Value of channel TOB1.
  @see For available values for "unit", see temperatureConversion().
*/

float CKellerBus::getTOB1(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB1),unit);
}

/**
  @brief Returns the value of channel TOB2.
  @param unit Temperature unit.
  @return Value of channel TOB2.
  @see For available values for "unit", see temperatureConversion().
*/

float CKellerBus::getTOB2(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB2),unit);
}

/**
  @brief Returns the value of channel T.
  @param unit Temperature unit.
  @return Value of channel T.
  @see For available values for "unit", see temperatureConversion().
*/

float CKellerBus::getT(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_T),unit);
}

/**
  @brief Open the hardwareserial port
*/

void CKellerBus::open(void)
{
  if(useHWSerial) {
    hwSerial->begin(baudrate);
  } else {
    swSerial->begin(baudrate);
  }  
}

/**
  @brief Close the hardwareserial port
*/

void CKellerBus::close(void)
{
  if(useHWSerial) {
    hwSerial->end();
  } 
}

/**
  @brief Converts the pressure value.
  @param sValue pressure in bar
  @param unit Target pressure unit.
   
  _available values for "unit"_

  - P_BAR    conversion to Bar
  - P_MBAR   conversion to mBar
  - P_PSI    conversion to Psi
  - P_PA     conversion to Pascal
  - P_HPA    conversion to hPascal
  - P_KPA    conversion to kPascal
  - P_MPA    conversion to MPascal
  - P_MH2O   conversion to mH2O
  - P_MWS    conversion to mWs Meter Wassersaeule
  - P_MWG    conversion to mWg meters, water gauge
  - P_INHG   conversion to inHg
  - P_MMHG   conversion to mmHg
  - P_INH2O  conversion to inH2O
  - P_TORR   conversion to Torr
  - P_FTH2O  conversion to ftH2O
  
  @return Converted pressure value.
*/

float CKellerBus::pressureConversion(float sValue, uint8_t targetUnit) 
{ 
  float pValue;

  switch (targetUnit) {

  case P_BAR :
  default :
    pValue = sValue;
    break;

  case 1 : // mBar and hPa          
    pValue = sValue * 1000;
    break;

  case P_PSI :
    pValue = sValue * 14.5037738;
    break;

  case P_PA :
    pValue = sValue * 100000;
    break;

  case P_KPA :
    pValue = sValue * 100;
    break;

  case P_MPA :
    pValue = sValue / 10;
    break;

  case 6 : // mH2O, mWs, m.Wg
    pValue = sValue * 10.1972;
    break;

  case P_INHG :
    pValue = sValue * 29.5300;
    break;

  case 8 : // Torr and mmHg
    pValue = sValue * 750.061683;
    break;

  case P_INH2O :
    pValue = sValue * 401.463;
    break;

  case P_FTH2O :
    pValue = sValue * 33.4553;
    break;  
  }

  return pValue;
}

/**
  @brief Converts the temperature value.
  @param sValue Temperature in deg celsius
  @param unit Target temperature unit.
  
  _available values for "unit"_

  - T_DEGC  conversion to °C Celsius
  - T_DEGK  conversion to °K Kelvin
  - T_DEGF  conversion to °F Fahreinheit
  - T_DEGR  conversion to °R Rankine
  
  @return Converted temperature value.
*/ 

float CKellerBus::temperatureConversion(float sValue, uint8_t targetUnit) 
{ 
  float tValue;

  switch (targetUnit) {

  case T_DEGC :
  default :
    tValue = sValue;
    break;

  case T_DEGK :         
    tValue = sValue + 273.15;
    break;

  case T_DEGF :
    tValue = (sValue * 1.8) + 32;
    break;

  case T_DEGR :
    tValue = (sValue + 273.15) * 1.8;
    break;
  }

  return tValue;
}

/**
  @brief Reads the last error code.
  @return Error code.
*/ 

int8_t CKellerBus::getError(void)
{
  return Error;
}

/**
  @brief Sets the transmission timeout.
  @param _timeout The new timeout in milliseconds.
*/  

void CKellerBus::setTimeout(uint16_t _timeout)
{
  timeout = _timeout;
}

/**
  @brief Returns the transmission timeout.
  @return Transmission timeout in milliseconds.
*/ 

uint16_t CKellerBus::getTimeout(void)
{
  return timeout;
}

/**
  @brief Returns the device address
  @return Device address.
*/ 

uint8_t CKellerBus::getDeviceAddress(void)
{
  return device;
}
