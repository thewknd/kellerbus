/*

 This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 
 
 test.ino
 
 */

#include <kellerbus.h>
#include <Time.h>


CKellerBus kbus(&Serial1,9600,5,250);

time_t kbusTime;

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
}


void loop() {
  uint8_t mclass,group,myear,week,cfg_p,cfg_t,cnt_t,buffer,state;
  Serial.println("\n-- KELLERBUS --");

  kbus.initDevice(250,&mclass,&group,&myear,&week,&buffer,&state);
  Serial.print("TYPE            : ");
  Serial.print(mclass,DEC);
  Serial.print(".");
  Serial.print(group,DEC);
  Serial.print("-");
  Serial.print(myear,DEC);
  Serial.print(".");
  Serial.println(week,DEC);
  Serial.print("BUFFER          : ");
  Serial.println(buffer,DEC);
  Serial.print("STATE           : ");
  Serial.println(state,BIN);
  Serial.print("SN              : ");
  Serial.println(kbus.getSerialnumber(),DEC);
  Serial.print("DEVICE ADDRESS  : ");
  kbus.writeDeviceAddress(0);
  Serial.println(kbus.getDeviceAddress(),DEC);
  kbusTime = kbus.readDeviceTime(); 
  Serial.print("DEVICE TIME     : "); 
  Serial.print(day(kbusTime),DEC);
  Serial.print(".");
  Serial.print(month(kbusTime),DEC);
  Serial.print(".");
  Serial.print(year(kbusTime),DEC);
  Serial.print(" ");
  Serial.print(hour(kbusTime),DEC);
  Serial.print(":");
  Serial.print(minute(kbusTime),DEC);
  Serial.print(":");
  Serial.print(second(kbusTime),DEC);
  Serial.println(" (d:m:y h:m:s)");
  
  Serial.println("\n\r- Configuration -");
  kbus.readConfiguration(&cfg_p,&cfg_t,&cnt_t);
  Serial.print("CFG_P           : ");
  Serial.println(cfg_p,BIN);
  Serial.print("CFG_T           : ");
  Serial.println(cfg_t,BIN);
  Serial.print("CNT_T           : ");
  Serial.println(cnt_t,BIN);
  
  Serial.println("\n\r- Measurement -");
  Serial.print("CH0             : ");
  Serial.println(kbus.getCH0(),DEC);
  Serial.print("P1              : ");
  Serial.println(kbus.getP1(P_BAR),DEC);
  Serial.print("P2              : ");
  Serial.println(kbus.getP2(P_BAR),DEC);
  Serial.print("TOB1            : ");
  Serial.println(kbus.getTOB1(T_DEGC),DEC);
  Serial.print("TOB2            : ");
  Serial.println(kbus.getTOB2(T_DEGC),DEC);
  Serial.print("T               : ");
  Serial.println(kbus.getT(T_DEGC),DEC);
  
  Serial.println("\n\r- Gain/Offset -");
  Serial.println("Channel           Offset               Gain");  
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
  Serial.println("Channel           Min                  Max");  
  Serial.print("P1              : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(80),P_BAR),DEC); 
  Serial.print("         ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(81),P_BAR),DEC);
  Serial.print("P2              : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(82),P_BAR),DEC); 
  Serial.print("         ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(83),P_BAR),DEC);
  Serial.print("T               : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(84),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(85),T_DEGC),DEC);
  Serial.print("TOB1            : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(86),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(87),T_DEGC),DEC);
  Serial.print("TOB2            : "); 
  Serial.print(kbus.pressureConversion(kbus.readScalingValue(88),T_DEGC),DEC); 
  Serial.print("       ");
  Serial.println(kbus.pressureConversion(kbus.readScalingValue(89),T_DEGC),DEC);
  Serial.print("CH0             : "); 
  Serial.print(kbus.readScalingValue(90),DEC); 
  Serial.print("        ");
  Serial.println(kbus.readScalingValue(91),DEC);
  Serial.print("ANALOG          : "); 
  Serial.print(kbus.readScalingValue(94),DEC); 
  Serial.print("         ");
  Serial.println(kbus.readScalingValue(95),DEC);
  
  delay(5000);
  //while(1) ;
}