### kellerbus, a library for arduino

## requirements
- Arduino IDE 1.0.1
- a free hardware serial interface (works best with Arduino Leonardo, Mega, Mega2560)
- RS485 compatible transceiver, like MAX487
- compatible* Keller transmitter

![keller transmitter 33X/35X](http://www.elektrotechnik.vogel.de/imgserver/bdb/212800/212831/4.jpg)

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
1. download and copy the kellerbus folder to your Arduino library directory.<br/>on mac: /Users/_username_/Documents/Arduino
2. restart the Arduino IDE
3. Select an example from the Examples/kellerbus menu
4. connect your compatible* Keller transmitter
5. upload your code to the Arduino and start the serial monitor

(*)compatible are transmitters with digital RS485 output
eg Series 33X/35X/36X/39X/41X/46X/DCX

## wiring

![wiring](http://dl.dropbox.com/u/2486346/rs485-5.jpg)


## online resources
- Online documentation: http://thewknd.github.com/kellerbus/
- Github.com repo: https://github.com/thewknd/kellerbus
- Blog (german): http://tinytiger.ch/blog/
- Bus protocol description (with S30, DCX, dV2, Leo and LEX): http://www.keller-druck.ch/home_e/paprod_e/progrcode_e.asp
- Bus protocol description (only for S30, but the newer one): http://www.keller-druck.ch/picts/pdf/engl/comm_protocol_e.pdf

## license

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.