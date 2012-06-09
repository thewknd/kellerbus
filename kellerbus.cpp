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
  if (Error != RS_OK) {
      // 2nd try for sleeping dcx
    delay(30);
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 48;
    TransferData(2,10);
  }
}

void CKellerBus::initDevice(uint8_t _device, uint8_t* _class, uint8_t* _group, uint8_t* _year, uint8_t* _week, uint8_t* _buffer, uint8_t* _state) 
{
  device = _device;
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 48;
   
  TransferData(2,10);
  if (Error == RS_OK) {
    *_class = RxBuffer[2];
    *_group = RxBuffer[3];
    *_year = RxBuffer[4];
    *_week = RxBuffer[5];
    *_buffer = RxBuffer[6];
    *_state = RxBuffer[7];
  } else {
      // 2nd try for sleeping dcx
    delay(30);
    TxBuffer[0] = device;
    TxBuffer[1] = 0b01111111 & 48;
    TransferData(2,10);
    if (Error == RS_OK) {
      *_class = RxBuffer[2];
      *_group = RxBuffer[3];
      *_year = RxBuffer[4];
      *_week = RxBuffer[5];
      *_buffer = RxBuffer[6];
      *_state = RxBuffer[7];
    } 
  }
}

//################## getSerialnumber ###################
// Takes:   -
// Returns: Serialnumber / Error: -3
// Effect:  Reads the serialnumber out of the device

uint32_t CKellerBus::getSerialnumber() 
{
  TxBuffer[0] = device;
  TxBuffer[1] = 0b01111111 & 69;
  
  TransferData(2,8);
  
  // Serialnumber calculation, see protocol documentation
  return 256*65536*(uint32_t)RxBuffer[2] + 65536*(uint32_t)RxBuffer[3] + 256*(uint32_t)RxBuffer[4] + (uint32_t)RxBuffer[5];
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
	    
	  bteArr[0] = RxBuffer[5];
	  bteArr[1] = RxBuffer[4];
	  bteArr[2] = RxBuffer[3];
	  bteArr[3] = RxBuffer[2];
	  
	  return *(float*)(&bteArr[0]);
  }
  else {
		Error = SW_INVALIDPARAM;
		return -3;  
  }
}  

//################## TransferData ###################
// Takes:   length of data and response
// Returns: -
// Effect:  Transfer data to the device, recieve response

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
	  Serial.print("\nTX:");
	  for(p = 0; p < nTX + 2; p++) {
			Serial.print(TxBuffer[p],DEC);
			Serial.print ("'") ;
	  }
	  Serial.println("");
  }
  
  // Open HWSerial
  Open();
  
  digitalWrite(RTS_PIN,HIGH);
  delay(2);
  
  if(Comm->write(TxBuffer,(int)(nTX + 2)) != (nTX + 2)) {
	  Error = RS_TXERROR;
  }
  delay(5);
  
  digitalWrite(RTS_PIN,LOW);  
  delay(1); 
  
  if (KB_DEBUG) Serial.print("RX:");
  
  b = 0;  
  startTimeout = millis();
  do {
    if (Comm->available() > 0) {
      RxBuffer[b] = Comm->read();
      if (KB_DEBUG) Serial.print(RxBuffer[b],DEC); 
      
      if (b == 0){

      	if(device == 250) {
	      	if ((RxBuffer[b] >= 1) && (RxBuffer[b] <= 250)) {
		      	b++;
	      	}
      	} else {
	      	if(RxBuffer[b] == device )	{
		      	b++;  	
	      	} else {
		      	if (KB_DEBUG) Serial.print("***");
	      	}
      	}
	    } else if(b == 1) {
	    
	    	// handle exception errors (communication protocol: 3.3.2.2 / Exception errors )
	    	
	    	if(RxBuffer[b] != (0x80 | TxBuffer[b])) {	
	    	
		    	if( RxBuffer[b] == TxBuffer[b]) {
		    		b++;
		    	} else {
			    	if(device == 250) {
			      	if ((RxBuffer[b] >= 1) || (RxBuffer[b] <= 250)) {
			      		if (KB_DEBUG) Serial.print("+++");
			      		RxBuffer[b-1] = RxBuffer[b]; 
			      	} else {
				      	b = 0;
				      	if (KB_DEBUG) Serial.print("***");
			      	}
		      	} else {
			      	if(RxBuffer[b] == device )	{
				      	if (KB_DEBUG) Serial.print("+++");
				      	RxBuffer[b-1] = RxBuffer[b];
			      	} else {
			      		b = 0;
				      	if (KB_DEBUG) Serial.print("***");
			      	}
		      	}
		    	}
	    	} else {
		    	b++;
		    	nRX = 3;
		    	if (KB_DEBUG) Serial.print(" EX ");
	    	}
	    } else {
		    b++;
	    }
	    
	    if (KB_DEBUG) Serial.print("'");      
      
      startTimeout = millis();    
      
    }      
    now = millis();    
  } while((b < nRX) && (now - startTimeout <= timeout) && (Error == RS_OK)); // timeout 
  
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
  		case 1 : Error = DEVICE_NONFUNCTION; break;
  		case 2 : Error = DEVICE_INCPARAMETERS; break;
  		case 3 : Error = DEVICE_ERRONEOUSDATA; break;
  		case 32: Error = DEVICE_INIT; break;
  		default: Error = RS_ERROR; break;
  	}
  }  
  	
  // Close HWSerial
  Close();
  if (KB_DEBUG) Serial.print("Fehler:");
  if (KB_DEBUG) Serial.println(Error);
}

float CKellerBus::getCH0() 
{
  return readChannel(CH_0);
}


float CKellerBus::getP1(uint8_t unit) 
{	
  return pressureConversion(readChannel(CH_P1),unit);
}


float CKellerBus::getP2(uint8_t unit) 
{
  return pressureConversion(readChannel(CH_P2),unit);
}


float CKellerBus::getTOB1(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB1),unit);
}


float CKellerBus::getTOB2(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_TOB2),unit);
}


float CKellerBus::getT(uint8_t unit) 
{
  return temperatureConversion(readChannel(CH_T),unit);
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
			pValue = sValue * 1;
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

//################## temperatureConversion ###################
// Takes:  -
// Returns: Error code
// Effect:  

int8_t CKellerBus::getError()
{
	return Error;
}