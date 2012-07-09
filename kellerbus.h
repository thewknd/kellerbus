/**

@author thewknd
@date 7.2012

@copyright
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#ifndef _H_KELLERBUS
#define _H_KELLERBUS

#include <Arduino.h>
#include <CRC16.h>
#include <Time.h>

// Error codes
#define RS_OK                       0
#define RS_ERROR                    -1
#define RS_TIMEOUT                  -4
#define RS_BADDATA                  -5
#define SW_INVALIDPARAM             -6
#define RS_TXERROR                  -7
#define RS_BADCRC                   -8
#define DEVICE_NONFUNCTION          -10
#define DEVICE_INCPARAMETERS        -11
#define DEVICE_ERRONEOUSDATA        -12
#define DEVICE_INIT                 -13

#define COMM_TX_MAX                 20
#define COMM_RX_MAX                 260

// pressure conversation

#define P_BAR                       0     // Bar
#define P_MBAR                      1     // mBar
#define P_PSI                       2     // Psi
#define P_PA                        3     // Pascal
#define P_HPA                       1     // hPascal
#define P_KPA                       4     // kPascal
#define P_MPA                       5     // MPascal
#define P_MH2O                      6     // mH2O
#define P_MWS                       6     // mWs Meter Wassersaeule
#define P_MWG                       6     // mWg meters, water gauge
#define P_INHG                      7     // inHg
#define P_MMHG                      8     // mmHg
#define P_INH2O                     9     // inH2O
#define P_TORR                      8     // Torr
#define P_FTH2O                     10    // ftH2O

// temperature conversation

#define T_DEGC                      0     // °C Celsius
#define T_DEGK                      1     // °K Kelvin
#define T_DEGF                      2     // °F Fahreinheit
#define T_DEGR                      3     // °R Rankine

#define MAX_CHANNELS                6

// Function F73, read channel value

#define CH_0                        0
#define CH_P1                       1
#define CH_P2                       2
#define CH_T                        3
#define CH_TOB1                     4
#define CH_TOB2                     5

// Functin F30, read coefficient, read scaling values

#define OFFSET_P1                   64
#define GAIN_P1                     65
#define OFFSET_P2                   66
#define GAIN_P2                     67
#define OFFSET_ANALOG               68
#define GAIN_ANALOG                 69
#define OFFSET_CH0                  70
#define GAIN_CH0                    71

#define MIN_P1                      80
#define MAX_P1                      81
#define MIN_P2                      82
#define MAX_P2                      83
#define MIN_T                       84
#define MAX_T                       85
#define MIN_TOB1                    86
#define MAX_TOB1                    87
#define MIN_TOB2                    88
#define MAX_TOB2                    89
#define MIN_CH0                     90
#define MAX_CH0                     91

#define MIN_ANALOG                  94
#define MAX_ANALOG                  95

#define KB_DEBUG                    0 // 1 = Debug mode active, 0 = Debug mode disabled

#define delayus                     delayMicroseconds

class CKellerBus
{
  private:
    HardwareSerial *Comm;
  
    CRC checksum; 
  
    uint16_t baudrate; // baudrate for the rs485 connection
    uint8_t RTS_PIN; // RTS hardware pin on Arduino
    uint16_t timeout; // Communication timeout for the rs485 connection
  
    uint8_t TxBuffer[COMM_TX_MAX]; // Transmit buffer
    uint8_t RxBuffer[COMM_TX_MAX + COMM_RX_MAX]; // Recieve buffer
  
    int8_t Error; // Error code of the last error
  
    uint8_t device; // device address
  
    void open(void);
    void close(void);
  
    void TransferData(uint8_t, uint8_t);
    
    float readChannel(uint8_t);
  
  public:  
  
    CKellerBus(HardwareSerial*, uint16_t, uint8_t, uint16_t);
  
    int8_t getError();

    void initDevice(uint8_t); 
    void initDevice(uint8_t, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*, uint8_t*); 
  
    uint8_t getDeviceAddress(void);
    uint32_t getSerialnumber(void);
    
    
    float readScalingValue(uint8_t);
    float pressureConversion(float, uint8_t);
    float temperatureConversion(float, uint8_t);
    void writeDeviceAddress(uint8_t);
    
    void readConfiguration(uint8_t*, uint8_t*, uint8_t*);
    
    int8_t readBatCapacity(void);
  
    time_t readDeviceTime(void);
    void writeDeviceTime(uint8_t, uint8_t, uint16_t, uint8_t, uint8_t, uint8_t);
    
    float getCH0();
    float getP1(uint8_t);
    float getP2(uint8_t);
    float getTOB1(uint8_t);
    float getTOB2(uint8_t);
    float getT(uint8_t);
    
    void setTimeout(uint16_t);
    uint16_t getTimeout(void);
    int16_t readActualPageAddress(void);
};
#endif

