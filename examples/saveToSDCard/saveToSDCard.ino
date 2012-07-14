/**

@author thewknd
@date 7.2012

@copyright
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <kellerbus.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <SD.h>


CKellerBus kbus(&Serial1,9600,22,250);

char subfolder[] = "datalogger";

const int chipSelect = 23;

bool sdCardReady = false;

time_t kbusTime;

void setup() {
  Serial.begin(115200);
  pinMode(SS, OUTPUT);
  pinMode(23, OUTPUT);
  while (!Serial) ;
  Serial.println("--- DATAEXPERT ---");
  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
}


void loop() {

  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  if (dataFile) {
    sdCardReady = true;
    Serial.println("Saving data... do not disconnect");
  } else {
    Serial.println("error opening datalog.csv");
  } 

  uint8_t mclass,group,myear,week,cfg_p,cfg_t,cnt_t,buffer,state;

  uint16_t pageAddress;
  uint16_t actPageAddress, startPageAddress, endPageAddress, timegap;
  uint8_t index = 0, overflow = 0, offset = 0, datatype, chan;
  float measure;
  char txt[4];

  kbus.initDevice(250,&mclass,&group,&myear,&week,&buffer,&state); 

  actPageAddress = kbus.readActualPageAddress();
  startPageAddress = actPageAddress;
  
  uint32_t tmptime;
  uint8_t buf0, buf1, buf2, buf3; 

  do {
    Serial.print("\n\rRecord ["); 
    Serial.print(index); 
    Serial.print("] Progress: ");
    index++;
    endPageAddress = startPageAddress;
    startPageAddress = kbus.readRecordPageStartPointer(startPageAddress);

    if((overflow == 1) && (startPageAddress < actPageAddress)) {
      overflow++;
      startPageAddress = actPageAddress + 1;
    } else if(startPageAddress > actPageAddress) {
      overflow++;
    }
    
    
    for(pageAddress = startPageAddress; pageAddress <= endPageAddress; pageAddress++) {
      Serial.print("."); 


      kbusTime = kbus.readRecordPageTime(pageAddress); 
      tmptime = kbusTime;

      for(offset = 8; offset <= 63; offset = offset + 4 ) {

        if( kbus.getRecordPageContent(pageAddress, offset, &datatype,  &chan, &measure, &timegap, txt) != 1) {
          datatype = 5;
        }

        switch (datatype) {
          case 0: 
            if(chan == 1) {

            }
            tmptime = tmptime + timegap;
            break;
          case 1: 

            tmptime = tmptime + timegap;
            break;
          case 2: 
            Serial.println(txt); 
            break;
          case 3: 
            offset = 64;
            break; 
          case 5: 
            Serial.println("kommunikationsfehler"); 
            break; 
          default:
            Serial.print("ausnahmefehler: "); 
            Serial.println(datatype); 
            break; 
        }

      }

    }

    startPageAddress--;

  } while((overflow < 2) && (kbus.getError() == RS_OK));
  
  
  Serial.println("\n\rDone...");

  delay(30000);
  //while(1) ;
}