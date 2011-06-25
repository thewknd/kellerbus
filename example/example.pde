/*
example.pde


output:
TYPE: 5.5-7.6
SN  : 127812
P1  : 0.9693163871 bar
TOB1: 25.9570312500 C

*/

#include "kellerbus.h"

CKellerBus kbus(&Serial1,9600,2);

void setup() {
  
  Serial.begin(9600);
  kbus.initDevice(250);    
}


void loop() {
  
  kbus.initDevice(); 
  Serial.print("TYPE: ");
  Serial.print(kbus.getClass(),DEC);
  Serial.print(".");
  Serial.print(kbus.getGroup(),DEC);
  Serial.print("-");
  Serial.print(kbus.getYear(),DEC);
  Serial.print(".");
  Serial.println(kbus.getWeek(),DEC);
  
  kbus.readSerialnumber();  
  Serial.print("SN  : ");
  Serial.println(kbus.getSerialnumber(),DEC);
  
  kbus.readChannel(1);
  Serial.print("P1  : ");
  Serial.print(kbus.getP1(),DEC);
  Serial.println(" bar");
  
  kbus.readChannel(4);
  Serial.print("TOB1: ");
  Serial.print(kbus.getTOB1(),DEC);
  Serial.println(" C");
  
  delay(1000);
}
