/*

 This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 
 
 test.ino
 
 */

#include <kellerbus.h>


CKellerBus kbus(&Serial1,9600,5,100);

char pUnit[] = "hPa";
char tUnit[] = "C";


void setup() {
  Serial.begin(115200);
  while (!Serial) ;

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


  Serial.print("SN : ");
  Serial.println(kbus.getSerialnumber(),DEC);
  Serial.println("\n\r- Measurement -");
  Serial.print("CH0 : ");
  Serial.print(kbus.getCH0(),DEC);
  Serial.println();
  Serial.print("P1 : ");
  Serial.print(kbus.getP1(P_HPA),DEC);
  Serial.println(pUnit);
  Serial.print("P2 : ");
  Serial.print(kbus.getP2(P_HPA),DEC);
  Serial.println(pUnit);
  Serial.print("TOB1: ");
  Serial.print(kbus.getTOB1(T_DEGC),DEC);
  Serial.println(tUnit);
  Serial.print("TOB2: ");
  Serial.print(kbus.getTOB2(T_DEGC),DEC);
  Serial.println(tUnit);
  Serial.print("T: ");
  Serial.print(kbus.getT(T_DEGC),DEC);
  Serial.println(tUnit);
  
  Serial.println("\n\r- Gain/Offset -");
  Serial.println("Channel            Offset                  Gain");  
  Serial.print("P1              : "); 
  Serial.print(kbus.readScalingValue(64),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(65),DEC);
  Serial.print("P2              : "); 
  Serial.print(kbus.readScalingValue(66),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(67),DEC);
  Serial.print("ANALOG          : "); 
  Serial.print(kbus.readScalingValue(68),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(69),DEC);
  Serial.print("CH0             : "); 
  Serial.print(kbus.readScalingValue(70),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(71),DEC);
  
  Serial.println("\n\r- Calibration data -");
  Serial.println("Channel            Min                  Max");  
  Serial.print("P1      (");
  Serial.print(pUnit); 
  Serial.print(")   : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(80),P_HPA),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(81),P_HPA),DEC);
  Serial.print("P2      (");
  Serial.print(pUnit); 
  Serial.print(")   : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(82),P_HPA),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(83),P_HPA),DEC);
  Serial.print("T       (");
  Serial.print(tUnit); 
  Serial.print(")     : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(84),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(85),T_DEGC),DEC);
  Serial.print("TOB1    (");
  Serial.print(tUnit); 
  Serial.print(")     : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(86),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(87),T_DEGC),DEC);
  Serial.print("TOB2    (");
  Serial.print(tUnit); 
  Serial.print(")     : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(88),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(89),T_DEGC),DEC);
  Serial.print("CH0     (");
  Serial.print("--"); 
  Serial.print(")    : "); 
  Serial.print(kbus.readScalingValue(90),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(91),DEC);
  Serial.print("ANALOG  (");
  Serial.print("V/mA"); 
  Serial.print(")  : "); 
  Serial.print(kbus.readScalingValue(94),DEC); 
  Serial.print("         ");
  Serial.println(kbus.readScalingValue(95),DEC);
}


void loop() {
}
