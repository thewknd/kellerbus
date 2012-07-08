 kellerbus, a library for arduino

## requirements
- Arduino IDE 1.0.1
- a free hardware serial interface (works best with Arduino Leonardo, Mega, Mega2560)
- RS485 compatible Transceiver, like MAX487

## features
- read serialnumber
- read pressure P1/P2
- read temperature T/TOB1/TOB2
- read CH0
- read configuration (gain/offset)
- read device type/software version
- read calibration data (min/max pressure min/max temperature)
- read/write device address
- read/write device time
- read battery capacity

## planned features
- soft serial support
- read/write record configuration (DCX)
- read record (DCX)

## how to install
1.
download and copy the kellebus folder to your Arduino library directory
on mac/linux: ~/Arduino/libraries/

2.
restart the Arduino IDE

3.
Select an example from the Examples/kellerbus menu

4.
connect your compatible* Keller transducer


### wire 

Arduino Leonardo - RS485 Transceiver - Keller Transducer

devices with external power supply
1 - WHITE - +OUT/GND
2 - BLACK - +SUPPLY
4 - BLUE - RS485A
5 - YELLOW - RS485B

devices with internal power supply
4 - BLUE - RS485A
5 - YELLOW - RS485B

5.
upload your code to the Arduino and start the serial monitor

(*)compatible are transducers with digital RS485 output
eg Series 33X/35X/36X/39X/41X/46X/DCX


## license

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.