/**

Example with Arduino Mega, stores records to the attached sd card.

Pin configuration
-----------------
22: rts for the rs485 transceiver
23: chip select SD card
50: MISO 
51: MOSI
52: SCK

TX1/RX1: RX/TX rs485 trasceiver

@author thewknd
@date 7.2012

@copyright
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

*/

#include <kellerbus.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <SD.h>


CKellerBus kbus((Stream*)&Serial1,9600,22,250);

const int chipSelect = 23;

time_t kbusTime;

File dataFile;
char filename[] = "LOGGER00.CSV";

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  pinMode(SS, OUTPUT);
  pinMode(23, OUTPUT);
  delay(3000);
  while (!Serial) ;
  Serial.println("\n\n\r--- DATAEXPERT ---");
  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
}


void loop() {

  uint16_t pageAddress;
  uint16_t actPageAddress, startPageAddress, endPageAddress, timegap;
  uint8_t index = 0, overflow = 0, offset = 0, datatype, chan;
  float measure;
  char txt[4];

  uint32_t oldtime;

  kbus.initDevice(250); 

  // get the actual page address
  actPageAddress = kbus.readActualPageAddress();
  startPageAddress = actPageAddress;
  
  

  do {

    // create new file with a uniqe filename
    char filename[] = "LOGGER00.CSV";  
    for (uint8_t i = 0; i < 100; i++) {
      filename[6] = i/10 + '0';
      filename[7] = i%10 + '0';
      if (! SD.exists(filename)) {
        // only open a new file if it doesn't exist
        dataFile = SD.open(filename, FILE_WRITE); 
        break;  // leave the loop!
      }
    }

    if (! dataFile) {
      Serial.println("couldnt create file");
      while(1) ;
    }  

    Serial.print("\n\rRecord ["); 
    Serial.print(index); 
    Serial.print("] Progress: ");
    index++;
    
    // get the end page
    endPageAddress = startPageAddress;

    // get first page of this record
    startPageAddress = kbus.readRecordPageStartPointer(startPageAddress);

    // overflow handling
    if((overflow == 1) && (startPageAddress < actPageAddress)) {
      overflow++;
      startPageAddress = actPageAddress + 1;
    } else if(startPageAddress > actPageAddress) {
      overflow++;
    }
    
    // go trough all pages of this record
    for(pageAddress = startPageAddress; pageAddress <= endPageAddress; pageAddress++) {
      Serial.print("."); 

      // get time of this page
      kbusTime = kbus.readRecordPageTime(pageAddress); 
      oldtime = kbusTime-1;

      for(offset = 8; offset <= 63; offset = offset + 4 ) {

        // read out the record rom
        if( kbus.getRecordPageContent(pageAddress, offset, &datatype,  &chan, &measure, &timegap, txt) != 1) {
          datatype = 5;
        }

        switch (datatype) {
          case 0: 
            kbusTime = kbusTime + timegap;
            if(oldtime < kbusTime) {
              oldtime = kbusTime;
              dataFile.println("");
              dataFile.print(day(kbusTime),DEC);
              dataFile.print(".");
              dataFile.print(month(kbusTime),DEC);
              dataFile.print(".");
              dataFile.print(year(kbusTime),DEC);
              dataFile.print(" ");
              dataFile.print(hour(kbusTime),DEC);
              dataFile.print(":");
              dataFile.print(minute(kbusTime),DEC);
              dataFile.print(":");
              dataFile.print(second(kbusTime),DEC);
            }

            dataFile.print(",");
            dataFile.print(measure,10);
            break;
          case 1: 
            kbusTime = kbusTime + timegap;
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

    // decrement startPageAddress, this will be the end Page of the next record
    startPageAddress--;
    // close the file
    dataFile.close();
    delay(30); 

  } while((overflow < 2) && (kbus.getError() == RS_OK));
  

  Serial.println("\n\rDone...");

  //delay(30000);
  while(1) ;
}