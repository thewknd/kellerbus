/*
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <kellerbus.h>

CKellerBus::CKellerBus(HardwareSerial* _comm, uint16_t _baudrate, uint8_t _rts, uint16_t _timeout)
{
  baudrate = _baudrate;
  timeout = _timeout;
  Comm = _comm;
  RTS_PIN = _rts;
  
  pinMode(RTS_PIN,OUTPUT);
  digitalWrite(RTS_PIN,LOW);
}

void CKellerBus::initDevice(uint8_t _device) 
{
  
  device = _device;
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 48;
  
  TransferData(2,10);
  if (Error == COMM_OK) {
    cClass = RxBuffer[2];
    cGroup = RxBuffer[3];
    cYear = RxBuffer[4];
    cWeek = RxBuffer[5];
    cBuffer = RxBuffer[6];
    cState = RxBuffer[7];
  } else {
      // 2nd try for sleeping dcx
    delay(30);
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 48;
    TransferData(2,10);
    if (Error == COMM_OK) {
      cClass = RxBuffer[2];
      cGroup = RxBuffer[3];
      cYear = RxBuffer[4];
      cWeek = RxBuffer[5];
      cBuffer = RxBuffer[6];
      cState = RxBuffer[7];
    } 
  }
}

//################## getSerialnumber ###################
// Takes:   nothing
// Returns: Serialnumber
// Effect:  Reads the serialnumber out of the transducer

uint32_t CKellerBus::getSerialnumber() 
{
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 69;
  
  TransferData(2,8);
  Serialnumber = 256*65536*(unsigned long)RxBuffer[2] + 65536*(unsigned long)RxBuffer[3] + 256*(unsigned long)RxBuffer[4] + (unsigned long)RxBuffer[5];
  
  return Serialnumber;
}


void CKellerBus::readChannel(uint8_t Channel)
{
  uint8_t bteArr[4];
  float value;
  
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 73;
  TxBuffer[2] = Channel;
  TransferData(3,9);
    
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
  }
   
}

//################## TransferData ###################
// Takes:   length of data and response
// Returns: status
// Effect:  Transfer Data to client, recieve response

void CKellerBus::TransferData(uint8_t nTX, uint8_t nRX) 
{
  uint16_t Crc; 
  uint16_t b=0;
  uint32_t startTimeout,now;

  // Open HWSerial
  Open();
  digitalWrite(RTS_PIN,HIGH);
  delay(2);
  
  // Calculate CRC16
  Crc = checksum.CRC16(TxBuffer,nTX);
  TxBuffer[nTX]= (Crc>>8)&0xFF; 
  TxBuffer[nTX+1]= Crc&0xFF;  
  
  Comm->write(TxBuffer,(int)(nTX + 2));
  delay(6);
  
  digitalWrite(RTS_PIN,LOW);  
  delay(3);  
  
  b = 0;  
  startTimeout = millis();
  do {
    if (Comm->available() > 0) {
      RxBuffer[b] = Comm->read(); 
      b++;
      startTimeout = millis();
    } 
    now = millis();  
  } while((startTimeout + timeout >= now ) && (b < nRX)); // timeout max 105ms
  
  if (b == nRX) {
    Error = COMM_OK;             
  } else if (now > startTimeout + timeout) {
	  Error = RS_TIMEOUT;
  } else {
    Error = RS_ERROR;   
  }
  	
  // Close HWSerial
  Close();
}


uint8_t CKellerBus::getClass() 
{
  return cClass;
}

uint8_t CKellerBus::getGroup() 
{
  return cGroup;
}

uint8_t CKellerBus::getYear() 
{
  return cYear;
}

uint8_t CKellerBus::getWeek() 
{
  return cWeek;
}

uint8_t CKellerBus::getBuffer() 
{
  return cBuffer;
}

uint8_t CKellerBus::getState() 
{
  return cState;
}

uint8_t CKellerBus::getDevice() 
{
  return device;
}


float CKellerBus::getCH0() 
{
	readChannel(CH_0);
  return ch0;
}


float CKellerBus::getP1(byte unit) 
{
	readChannel(CH_P1);		
  return pressureConversion(chP1,unit);
}


float CKellerBus::getP2(byte unit) 
{
	readChannel(CH_P2);
  return pressureConversion(chP2,unit);
}


float CKellerBus::getTOB1(byte unit) 
{
	readChannel(CH_TOB1);
  return temperatureConversion(chTOB1,unit);
}


float CKellerBus::getTOB2(byte unit) 
{
	readChannel(CH_TOB2);
  return temperatureConversion(chTOB2,unit);
}


float CKellerBus::getT(uint8_t unit) 
{
	readChannel(CH_T);
  return temperatureConversion(chT,unit);
}


void CKellerBus::Open()
{
  Comm->begin(baudrate);  
}


void CKellerBus::Close()
{
  Comm->end();
}

//################## pressureConversion ###################
// Takes:   target unit, Pressure in bar
// Returns: Pressure in target unit
// Effect:  bar in target unit conversion

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
			pValue = sValue * 14.503773773;
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
			pValue = sValue;
			break;
			
		case P_INHG :
			pValue = sValue * 1;
			break;
			
		case 8 : // Torr and mmHg
			pValue = sValue * 750.061682704;
			break;
			
		case P_INH2O :
			pValue = sValue * 1;
			break;
			
		case P_FTH2O :
			pValue = sValue * 33.46;
			break;	
	}
	
  return pValue;
}

//################## temperatureConversion ###################
// Takes:  temperature in deg celsius, target unit
// Returns: temperature in target unit
// Effect:  deg celsius conversion in target unit

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