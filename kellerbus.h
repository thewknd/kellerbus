/*
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <Arduino.h>
#include <CRC16.h>

#ifndef _H_KELLERBUS
#define _H_KELLERBUS

#define COMM_ERR_BAD_CRC           	-4
#define COMM_OK                     0

#define COMM_TX_MAX                 20
#define COMM_RX_MAX                 260

#define MAX_CHANNELS                32
#define MAX_UNITS                   15

#define P_BAR												0 		// Bar
#define P_MBAR											1 		// mBar
#define P_PSI												2 		// Psi
#define P_PA												3 		// Pascal
#define P_HPA												1 		// hPascal
#define P_KPA												4 		// kPascal
#define P_MPA												5 		// MPascal
#define P_MH2O											6 		// mH2O
#define P_MWS												6 		// mWs Meter Wassersäule
#define P_MWG												6 		// mWg meters, water gauge
#define P_INHG											7 		// inHg
#define P_MMHG											8 		// mmHg
#define P_INH2O											9 		// inH2O
#define P_TORR											8 		// Torr
#define P_FTH2O											10		// ftH2O


#define T_DEGC											0			// °C Celsius
#define T_DEGK											1			// °K Kelvin
#define T_DEGF											2			// °F Fahreinheit
#define T_DEGR											3			// °R Rankine

#define CH_0												0
#define CH_P1												1
#define CH_P2												2
#define CH_T												3
#define CH_TOB1											4
#define CH_TOB2											5

class CKellerBus
{
  private:
    HardwareSerial *Comm;
    
    CRC checksum;
    
    int baudrate;
    byte RTS_PIN;
    byte timeout;
        
    byte TxBuffer[COMM_TX_MAX];
    byte RxBuffer[COMM_TX_MAX + COMM_RX_MAX];
    
    byte Open();
    byte Close();
    
    byte TransferData(byte nTX, byte nRX);
    byte readChannel(byte Channel = CH_P1);
    float pressureConversion(float sValue, byte targetUnit = P_BAR);
    float temperatureConversion(float sValue, byte targetUnit = T_DEGC);
    
    byte cClass;
    byte cGroup;
    byte cYear;
    byte cWeek;
    byte cBuffer;
    byte cState;
    
    unsigned long Serialnumber;
    
    byte device;
    
    float ch0;
    float chP1;
    float chP2;
    float chTOB1;
    float chTOB2;
    float chT;
    
    
  public:  
    CKellerBus(HardwareSerial* _comm, int _baudrate, byte _rts, int _timeout);
    
    byte initDevice(byte _device);
    
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
    float getP1(byte unit = P_BAR);
    float getP2(byte unit = P_BAR);
    float getTOB1(byte unit = T_DEGC);
    float getTOB2(byte unit = T_DEGC);
    float getT(byte unit = T_DEGC);    
};
#endif
