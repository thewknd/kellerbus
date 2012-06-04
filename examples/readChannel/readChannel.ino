/*

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.


readChannel.pde

output:

-- KELLERBUS --
TYPE: 5.5-7.6
SN  : 127812
P1  : 0.9669585227 bar
TOB1: 26.7539062500 C

*/

#include <kellerbus.h>

CKellerBus kbus(&Serial1,9600,2);

void setup() {
  
  Serial.begin(115200);
  while (!Serial) ;
}

void loop() {
  
  Serial.println("-- KELLERBUS --");
  
  if(kbus.initDevice(250) == COMM_OK) { 
    Serial.print("TYPE: ");
    Serial.print(kbus.getClass(),DEC);
    Serial.print(".");
    Serial.print(kbus.getGroup(),DEC);
    Serial.print("-");
    Serial.print(kbus.getYear(),DEC);
    Serial.print(".");
    Serial.println(kbus.getWeek(),DEC);
  } else {
    Serial.println("COMM ERROR");
  }
  
  if(kbus.readSerialnumber() == COMM_OK) { 
    Serial.print("SN  : ");
    Serial.println(kbus.getSerialnumber(),DEC);
  } else {
    Serial.println("COMM ERROR");
  }
  
  if(kbus.readChannelP1() == COMM_OK) {
    Serial.print("P1  : ");
    Serial.print(kbus.getP1(),DEC);
    Serial.println(" bar");
  } else {
    Serial.println("COMM ERROR");
  }

  if(kbus.readChannelTOB1() == COMM_OK) {
    Serial.print("TOB1: ");
    Serial.print(kbus.getTOB1(),DEC);
    Serial.println(" C");
  } else {
    Serial.println("COMM ERROR");
  }
  
  delay(1000);
}
