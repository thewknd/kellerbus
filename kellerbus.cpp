/*
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <kellerbus.h>


/*
  Constructor: CKellerBus
  
  sets up the interface to the transmitter
  
  Parameters:
  _comm - Hardwareserial port.
  _baudrate - Baudrate for the hardwareserial port, usually 9600.
  _rts - Ready To Send Pin.
  _timeout - Communication timeout, in milliseconds, usually 100 (250 for DCX).
*/

CKellerBus::CKellerBus(HardwareSerial* _comm, uint16_t _baudrate, uint8_t _rts, uint16_t _timeout)
{
  baudrate = _baudrate;
  timeout = _timeout;
  Comm = _comm;
  RTS_PIN = _rts;

  pinMode(RTS_PIN,OUTPUT);
  digitalWrite(RTS_PIN,LOW);
}

/*
  Function: initDevice
  
  Device initialization. Wrapper for F48.
  
  Parameters:
  _device - device address.  
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

/*
  Function: initDevice
  
  Device initialization. Wrapper for F48.
  
  Parameters:
  _device - Device address.  
  _class - Pointer for the device class. 
  _group - Pointer for the device group.
  _year - Pointer for the software version (year).
  _week - Pointer for the software version (week).
  _buffer - Buffer size.
  _state - Device state.    
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

/*
  Function: getSerialnumber
  
  Returns the serialnumber.
  
  Returns:
  serialnumber 
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
      return -1;
    }
  } else {
    Error = SW_INVALIDPARAM;
    return -1000; 
  }
}  

/*
  Function: readScalingValue
  
  Returns the scaling value. Wrapper function F30.
  
  Parameters:
  no - Index of the scaling value.

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
                 
  Returns:
  Scaling value.
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
  
/*
  Function: writeDeviceAddress
  
  Sets the device address. 
  
  Parameters:
  newAddress - The new device address.
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

/*
  Function: readConfiguration
  
  wrapper function F100, read configuration
  
  Parameters:
  CFG_P - Pointer for the register CFG_P.
  CFG_T - Pointer for the register CFG_T.
  CNT_T - Pointer for the register CNT_T.
*/

void CKellerBus::readConfiguration(uint8_t* CFG_P, uint8_t* CFG_T, uint8_t* CNT_T)
{
  // Prepare TxBuffer
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

/*
  Function: readDeviceTime
  
  Returns the device time
  
  Returns:
  Device time in seconds since 1.1 1970 (unix time)
*/

time_t CKellerBus::readDeviceTime(void)
{
  uint32_t c1,c2,c3,c4;
   
  // Prepare TxBuffer
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 92;
  TxBuffer[2] = 3;
  
  TransferData(3,9); 
  
  if(Error == RS_OK) {
   
    c1 = (uint32_t)pow(2UL,24UL) * (uint32_t)RxBuffer[2];
    c2 = (uint32_t)pow(2UL,16UL) * (uint32_t)RxBuffer[3];
    c3 = (uint32_t)pow(2UL,8UL) * (uint32_t)RxBuffer[4];
    c4 = (uint32_t)RxBuffer[5];
     
    setTime(0, 0, 0, 1, 1, 2000); // hr - min - sec - day - month - year
    
    adjustTime(c1 + c2 + c3 + c4);
    return now();
  
  } else {
    return -1;
  }
} 

/*
  Function: readBatCapacity
  
  Returns the battery capacity.
  
  Returns:
  Battery caoacity in percent.
*/

int8_t CKellerBus::readBatCapacity(void)
{
  // Prepare TxBuffer
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 92;
  TxBuffer[2] = 8;
  
  TransferData(3,9); 
  
  if(Error == RS_OK) {
    return RxBuffer[6]; 
  } else {
    return -1;
  }
} 

/*
  Function: writeDeviceTime
  
  Sets the device time.
  
  Parameters:
  _day - Days of the new date.
  _month - Months of the new date.
  _year - Years of the new date.
  _hour - Hours of the new time.
  _minute - Minutes of the new time.
  _second - Seconds of the new time.
*/

void CKellerBus::writeDeviceTime(uint8_t _day, uint8_t _month, uint16_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second)
{
  uint32_t since2000,c1,c2,c3,c4;
  
  setTime(0, 0, 0, 1, 1, 2000); // hr - min - sec - day - month - year
  since2000 = now();
  setTime(_hour, _minute, _second, _day, _month, _year); // hr - min - sec - day - month - year
  
  since2000 = now() - since2000;
  
  c1 = since2000 / (uint32_t)pow(2UL,24UL);
  since2000 = since2000 % (uint32_t)pow(2UL,24UL);
  c2 = since2000 / (uint32_t)pow(2UL,16UL);
  since2000 = since2000 % (uint32_t)pow(2UL,16UL);
  c3 = since2000 / (uint32_t)pow(2UL,8UL);
  since2000 = since2000 % (uint32_t)pow(2UL,8UL);
  c4 = since2000;
  
  // Prepare TxBuffer
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 93;
  TxBuffer[2] = 3;
  TxBuffer[3] = c1;
  TxBuffer[4] = c2;
  TxBuffer[5] = c3;
  TxBuffer[6] = c4;
  TxBuffer[7] = 0; // not used
  
  TransferData(8,5);  
} 

/*
  Function: TransferData
  
  Transmits the TXBuffer to the device. 
  
  Parameters:
  nTX - Amount of bytes for transmission, without the checksum.
  nRX - Amount of bytes for the response, with the checksum.
*/

void CKellerBus::TransferData(byte nTX, byte nRX) 
{
  uint16_t Crc, p, now, startTimeout, b=0;
  uint8_t crcBuff[COMM_TX_MAX + COMM_RX_MAX];

  Error = RS_OK;

  // Clear RxBuffer;
  for(b = 0; b < COMM_TX_MAX + COMM_RX_MAX; b++) {
    RxBuffer[b] = 0;
    crcBuff[b] = 0;
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

  // Open HWSerial
  open();

  digitalWrite(RTS_PIN,HIGH);
  delay (1);

  if(Comm->write(TxBuffer,(int)(nTX + 2)) != (nTX + 2)) {
    Error = RS_TXERROR;
  }

  while (!(UCSR1A & (1 << UDRE1))) {
    UCSR1A |= 1 << TXC1;
  }
  while (!(UCSR1A & (1 << TXC1)));
  
  digitalWrite(RTS_PIN,LOW);   

  if (KB_DEBUG) Serial.print(" -RX:");

  b = 0;  
  startTimeout = millis();
  do {
    if (Comm->available() > 0) {
      // incoming byte
      RxBuffer[b] = Comm->read();
      if (KB_DEBUG) Serial.print(RxBuffer[b],DEC); 

      if (b == 0){

        // first step, check device address
        if(device == 250) {
          if ((RxBuffer[b] >= 1) && (RxBuffer[b] <= 250)) {
            // device address is valid
            b++;
          }
        } else {
          if(RxBuffer[b] == device )  {
            // same device address -> ok
            b++;    
          } else {
            // wrong device address
            if (KB_DEBUG) Serial.print("***");
          }
        }
      } 
      else if(b == 1) {
        // second step, check for function code
        // handle exception errors (communication protocol: 3.3.2.2 / Exception errors )

        if(RxBuffer[b] != (0x80 | TxBuffer[b])) { 

          if( RxBuffer[b] == TxBuffer[b]) {
            // the transmitted and the recieved function code are the same 
            b++;
          } else {
            // function code was wrong, check for device id and do again the second step
            if(device == 250) {
              if ((RxBuffer[b] >= 1) || (RxBuffer[b] <= 250)) {
                if (KB_DEBUG) Serial.print("+++");
                RxBuffer[b-1] = RxBuffer[b]; 
              } else {
                b = 0;
                if (KB_DEBUG) Serial.print("***");
              }
            } else {
              if(RxBuffer[b] == device )  {
                if (KB_DEBUG) Serial.print("+++");
                RxBuffer[b-1] = RxBuffer[b];
              } else {
                b = 0;
                if (KB_DEBUG) Serial.print("***");
              }
            }
          }
        } else {
          // exception error flag is set
          b++;
          nRX = 3;
          if (KB_DEBUG) Serial.print(" EX ");
        }
      } else {
       // step > 2
        b++;
      }

      if (KB_DEBUG) Serial.print("'");      

      startTimeout = millis();    

    }      
    now = millis();    
  } 
  while((b < nRX) && (now - startTimeout <= timeout) && (Error == RS_OK)); // timeout 

  if (now - startTimeout > timeout) {
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
    // Checksumme überprüfen
    if (KB_DEBUG) Serial.print("\r\nCRC:");  

    for(p = 0; p < b-2;p++) {
      if (KB_DEBUG) {
        Serial.print(RxBuffer[p]); 
        Serial.print("'");
      }
      crcBuff[p] = RxBuffer[p];
    }
    Crc = checksum.CRC16(crcBuff,b-2);
    if (KB_DEBUG) {
      Serial.print(" -- HB:"); 
      Serial.print(highByte(Crc));
      Serial.print(" LB:");
      Serial.print(lowByte(Crc));
      Serial.println("");
    }
    if((highByte(Crc) != RxBuffer[b-2]) || (lowByte(Crc) != RxBuffer[b-1])) {
      Error = RS_BADCRC;
      if (KB_DEBUG)Serial.println("*** BAD CRC");
    } 
  }

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

  // Close HWSerial
  close();
  if (KB_DEBUG) Serial.print("Fehler:");
  if (KB_DEBUG) Serial.println(Error);
}

/*
  Function: getCH0
  
  Returns the value for CH0.
  
  Returns:
  Value for CH0.
*/

float CKellerBus::getCH0(void) 
{
  return readChannel(CH_0);
}

/*
  Function: getP1
  
  Returns the value for P1.
  
  Parameter:  
  unit - Pressure unit.
  
  Returns:  
  Value for P1.
  
  See:  
  For available values for "unit", see <pressureConversion>.
*/

float CKellerBus::getP1(uint8_t unit) 
{ 
  return pressureConversion(readChannel(CH_P1),unit);
}


/*
  Function: getP2
  
  Returns the value for P2.
  
  Parameter: 
  unit - Pressure unit.
  
  Returns:
  Value for P2.
  
  See:
  For available values for "unit", see <pressureConversion>.
*/

float CKellerBus::getP2(uint8_t unit) 
{
  return pressureConversion(readChannel(CH_P2),unit);
}


/*
  Function: getTOB1
  
  Returns the value for TOB1.
  
  Parameter:
  unit - Temperature unit.
  
  Returns:
  Value for TOB1.
  
  See:
  For available values for "unit", see <temperatureConversion>.
*/

float CKellerBus::getTOB1(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB1),unit);
}

/*
  Function: getTOB2
  
  Returns the value for TOB2.
  
  Parameter:
  unit - Temperature unit.
  
  Returns:
  Value for TOB2.
  
  See:
  For available values for "unit", see <temperatureConversion>.
*/

float CKellerBus::getTOB2(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB2),unit);
}

/*
  Function: getT
  
  Returns the value for T.
  
  Parameter:
  unit - Temperature unit.
  
  Returns:
  Value for T.
  
  See:
  For available values for "unit", see <temperatureConversion>.
*/

float CKellerBus::getT(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_T),unit);
}

/*
  Function: open
  
  Open the hardwareserial port
*/

void CKellerBus::open(void)
{
  Comm->begin(baudrate);  
}

/*
  Function: close
  
  Close the hardwareserial port
*/

void CKellerBus::close(void)
{
  Comm->end();
}

/*
  Function: pressureConversion
  
  Converts the pressure value.
  
  Parameter:
  sValue - pressure in bar
  unit - Target pressure unit.
   
  _available values for "unit"_

  - P_BAR    conversion to Bar
  - P_MBAR   conversion to mBar
  - P_PSI    conversion to Psi
  - P_PA     conversion to Pascal
  - P_HPA    conversion to hPascal
  - P_KPA    conversion to kPascal
  - P_MPA    conversion to MPascal
  - P_MH2O   conversion to mH2O
  - P_MWS    conversion to mWs Meter Wassersäule
  - P_MWG    conversion to mWg meters, water gauge
  - P_INHG   conversion to inHg
  - P_MMHG   conversion to mmHg
  - P_INH2O  conversion to inH2O
  - P_TORR   conversion to Torr
  - P_FTH2O  conversion to ftH2O
  
  Returns:
  Converted pressure value.
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

/*
  Function: temperatureConversion
  
  Converts the temperature value.
  
  Parameter:
  sValue - temperature in deg celsius
  unit - Target temperature unit.
  
  _available values for "unit"_

  - T_DEGC  conversion to °C Celsius
  - T_DEGK  conversion to °K Kelvin
  - T_DEGF  conversion to °F Fahreinheit
  - T_DEGR  conversion to °R Rankine
  
  Returns:
  Converted temperature value.
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

/*
  Function: getError
  
  Reads the last error code.
  
  Returns:
  Error code.
*/ 

int8_t CKellerBus::getError(void)
{
  return Error;
}

/*
  Function: setTimeout
  
  Sets the transmission timeout.
  
  Parameters:
  _timeout - The new timeout in milliseconds.
*/  

void CKellerBus::setTimeout(uint16_t _timeout)
{
  timeout = _timeout;
}

/*
  Function: getTimeout
  
  Returns the transmission timeout.
  
  Returns:
  Transmission timeout in milliseconds.
*/ 

uint16_t CKellerBus::getTimeout(void)
{
  return timeout;
}

/*
  Function: getDeviceAddress
  
  Returns the device address
  
  Returns:
  Device address.
*/ 

uint8_t CKellerBus::getDeviceAddress(void)
{
  return device;
}
