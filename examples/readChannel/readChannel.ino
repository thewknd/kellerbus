/*

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.


readChannel.pde

*/

#include <kellerbus.h>

CKellerBus kbus(&Serial1,9600,5,100);

void setup() {
  
  Serial.begin(115200);
  while (!Serial) ;
}

void loop() {
  
  Serial.println("\n-- KELLERBUS --");
  
  kbus.initDevice(250);
  Serial.print("TYPE: ");
  Serial.print(kbus.getClass(),DEC);
  Serial.print(".");
  Serial.print(kbus.getGroup(),DEC);
  Serial.print("-");
  Serial.print(kbus.getYear(),DEC);
  Serial.print(".");
  Serial.println(kbus.getWeek(),DEC);

  
  Serial.print("SN  : ");
  Serial.println(kbus.getSerialnumber(),DEC);
  Serial.println(" - - - - - - - -");
  Serial.println("P1  : ");
  Serial.print(kbus.getP1(),DEC);
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