/**

@author thewknd
@date 7.2012

@copyright
This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

#include <kellerbus.h>

CKellerBus kbus(&Serial1,9600,5,100);

void setup() {
  
  Serial.begin(115200);
  while (!Serial) ;
}

void loop() {
  uint8_t mclass,group,year,week;
  Serial.println("\n-- KELLERBUS --");
  
  kbus.initDevice(250,&mclass,&group,&year,&week,0,0);
  Serial.print("TYPE: ");
  Serial.print(mclass,DEC);
  Serial.print(".");
  Serial.print(group,DEC);
  Serial.print("-");
  Serial.print(year,DEC);
  Serial.print(".");
  Serial.println(week,DEC);

  
  Serial.print("SN  : ");
  Serial.println(kbus.getSerialnumber(),DEC);
  Serial.println(" - - - - - - - -");
  Serial.println("P1  : ");
  Serial.print(kbus.getP1(P_BAR),DEC);
  Serial.println(" bar");
  Serial.print(kbus.getP1(P_PSI),DEC);
  Serial.println(" psi");
  Serial.print(kbus.getP1(P_TORR),DEC);
  Serial.println(" Torr");
  Serial.print(kbus.getP1(P_HPA),DEC);
  Serial.println(" hPa");
  
  
  Serial.println(" - - - - - - - -");
  
  Serial.println("TOB1: ");
  
  Serial.print(kbus.getTOB1(T_DEGC),DEC);
  Serial.println(" C");
  Serial.print(kbus.getTOB1(T_DEGK),DEC);
  Serial.println(" K");
  Serial.print(kbus.getTOB1(T_DEGF),DEC);
  Serial.println(" F");
  Serial.print(kbus.getTOB1(T_DEGR),DEC);
  Serial.println(" R");
  
  delay(10000);
}