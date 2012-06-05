/*
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <Arduino.h>
#include <CRC16.h>

#ifndef _H_KELLERBUS
#define _H_KELLERBUS

#define RS_ERROR 										-1
#define RS_TIMEOUT          				-4
#define RS_BADDATA 									-5
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
    
    uint16_t baudrate;
    uint8_t RTS_PIN;
    uint16_t timeout;
        
    uint8_t TxBuffer[COMM_TX_MAX];
    uint8_t RxBuffer[COMM_TX_MAX + COMM_RX_MAX];
    
    void Open();
    void Close();
    
    byte TransferData(byte nTX, byte nRX);
    void readChannel(uint8_t Channel);
    float pressureConversion(float sValue, uint8_t);
    float temperatureConversion(float sValue, uint8_t);
    
    uint8_t Error;
    
    uint8_t cClass;
    uint8_t cGroup;
    uint8_t cYear;
    uint8_t cWeek;
    uint8_t cBuffer;
    uint8_t cState;
    
    unsigned long Serialnumber;
    
    byte device;
    
    float ch0;
    float chP1;
    float chP2;
    float chTOB1;
    float chTOB2;
    float chT;
    
    
  public:  
    CKellerBus(HardwareSerial* _comm, uint16_t _baudrate, uint8_t _rts, uint16_t _timeout);
    
    void initDevice(uint8_t _device);
     
    uint8_t getClass();
    uint8_t getGroup();
    uint8_t getYear();
    uint8_t getWeek();
    uint8_t getBuffer();
    uint8_t getState();
    uint8_t getDevice();
    uint32_t getSerialnumber();
    
    float getCH0();
    float getP1(byte unit = P_BAR);
    float getP2(byte unit = P_BAR);
    float getTOB1(byte unit = T_DEGC);
    float getTOB2(byte unit = T_DEGC);
    float getT(byte unit = T_DEGC);    
};
#endif
