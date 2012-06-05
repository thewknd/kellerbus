/*

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.


*/

#include <kellerbus.h>

CKellerBus kbus(&Serial1,9600,5,100);


const uint16_t numOfLoops = 20;

void setup() {
  
  Serial.begin(115200);
  while (!Serial) ;
  Serial.println("\n--- KELLERBUS --- BENCHMARK ---");
  kbus.initDevice(250);
}

void loop() {
  
	uint32_t cnt;
	uint32_t start,end;
	Serial.print("S: ");
	Serial.print(numOfLoops);
	Serial.println(" samples");
        
	start = millis();
	for( cnt = 0; cnt < numOfLoops; cnt++) {
  		kbus.getP1(P_BAR);
	} 
	end = millis();
	
	Serial.print("P: ");
	Serial.print(kbus.getP1(P_BAR));
	Serial.println(" bar");
	Serial.print("T: ");
	Serial.print(end - start);	
	Serial.println(" ms");
	Serial.print(">: ");
	Serial.print((end - start) / numOfLoops);	
	Serial.println(" ms pro Messung");
	Serial.print(">: ");
	Serial.print(1000 / ((end - start) / numOfLoops));	
	Serial.println(" samples/s");
	Serial.println(" - - - - - - - - - - - - - - - - ");
	delay(3000);
	
}