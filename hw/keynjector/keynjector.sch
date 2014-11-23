EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:keynjector
LIBS:keynjector-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 7
Title ""
Date "6 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 2100 2200 600  600 
U 53A03BBE
F0 "keynjector_usb_power" 50
F1 "keynjector_usb_power.sch" 50
F2 "USB_N" B R 2700 2450 60 
F3 "USB_P" B R 2700 2600 60 
$EndSheet
$Sheet
S 3800 1050 1500 6450
U 53A05C3D
F0 "keynjector_mcu" 50
F1 "keynjector_mcu.sch" 50
F2 "XIN12" I L 3800 1200 60 
F3 "XOUT12" I L 3800 1400 60 
F4 "USB_P" B L 3800 2600 60 
F5 "USB_N" B L 3800 2450 60 
F6 "~TPCS" O R 5300 1400 60 
F7 "TPDC" O R 5300 1500 60 
F8 "~TPWR" O R 5300 1600 60 
F9 "~TPRD" O R 5300 1700 60 
F10 "~TPRES" O R 5300 1900 60 
F11 "SPCK" O R 5300 2100 60 
F12 "NPCS2" O R 5300 2200 60 
F13 "MOSI" O R 5300 2300 60 
F14 "BUSY" I R 5300 2400 60 
F15 "MISO" I R 5300 2500 60 
F16 "~PENIRQ" I R 5300 2600 60 
F17 "DB8" B R 5300 3400 60 
F18 "DB9" B R 5300 3500 60 
F19 "DB10" B R 5300 3600 60 
F20 "DB11" B R 5300 3700 60 
F21 "DB12" B R 5300 3800 60 
F22 "DB13" B R 5300 3900 60 
F23 "DB14" B R 5300 4000 60 
F24 "DB15" B R 5300 4100 60 
F25 "TMS" B R 5300 4600 60 
F26 "TCK" B R 5300 4700 60 
F27 "TDO" B R 5300 4800 60 
F28 "TDI" B R 5300 4900 60 
F29 "NRST" B R 5300 5000 60 
F30 "MCDA0" B R 5300 5500 60 
F31 "MCDA1" B R 5300 5600 60 
F32 "MCDA2" B R 5300 5700 60 
F33 "MCDA3" B R 5300 5800 60 
F34 "MCCK" O R 5300 5900 60 
F35 "MCCDA" O R 5300 6000 60 
F36 "MCCD" I R 5300 6100 60 
$EndSheet
$Sheet
S 2100 1050 550  550 
U 53A16366
F0 "keynjector_oscillators" 50
F1 "keynjector_oscillators.sch" 50
F2 "XOUT12" I R 2650 1400 60 
F3 "XIN12" I R 2650 1200 60 
$EndSheet
$Sheet
S 6750 4500 550  700 
U 53A1696D
F0 "keynjector_debug" 50
F1 "keynjector_debug.sch" 50
F2 "TMS" B L 6750 4600 60 
F3 "TCK" B L 6750 4700 60 
F4 "TDO" B L 6750 4800 60 
F5 "TDI" B L 6750 4900 60 
F6 "NRST" B L 6750 5000 60 
$EndSheet
$Sheet
S 6750 1100 550  3150
U 53A178B1
F0 "keynjector_lcd_touch" 50
F1 "keynjector_lcd_touch.sch" 50
F2 "~TPCS" I L 6750 1400 60 
F3 "TPDC" I L 6750 1500 60 
F4 "~TPWR" I L 6750 1600 60 
F5 "~TPRD" I L 6750 1700 60 
F6 "~TPRES" I L 6750 1900 60 
F7 "SPCK" I L 6750 2100 60 
F8 "NPCS2" I L 6750 2200 60 
F9 "MOSI" I L 6750 2300 60 
F10 "BUSY" O L 6750 2400 60 
F11 "MISO" O L 6750 2500 60 
F12 "~PENIRQ" O L 6750 2600 60 
F13 "DB8" B L 6750 3400 60 
F14 "DB9" B L 6750 3500 60 
F15 "DB10" B L 6750 3600 60 
F16 "DB11" B L 6750 3700 60 
F17 "DB12" B L 6750 3800 60 
F18 "DB13" B L 6750 3900 60 
F19 "DB14" B L 6750 4000 60 
F20 "DB15" B L 6750 4100 60 
$EndSheet
$Sheet
S 6750 5400 550  850 
U 53A298BB
F0 "keynjector_sd_card" 50
F1 "keynjector_sd_card.sch" 50
F2 "MCDA0" B L 6750 5500 60 
F3 "MCDA1" B L 6750 5600 60 
F4 "MCDA2" B L 6750 5700 60 
F5 "MCDA3" B L 6750 5800 60 
F6 "MCCK" I L 6750 5900 60 
F7 "MCCDA" I L 6750 6000 60 
F8 "MCCD" O L 6750 6100 60 
$EndSheet
Wire Wire Line
	2700 2450 3800 2450
Wire Wire Line
	2700 2600 3800 2600
Wire Wire Line
	2650 1200 3800 1200
Wire Wire Line
	2650 1400 3800 1400
Wire Wire Line
	5300 1400 6750 1400
Wire Wire Line
	6750 1500 5300 1500
Wire Wire Line
	5300 1600 6750 1600
Wire Wire Line
	6750 1700 5300 1700
Wire Wire Line
	6750 1900 5300 1900
Wire Wire Line
	6750 2100 5300 2100
Wire Wire Line
	5300 2200 6750 2200
Wire Wire Line
	5300 2300 6750 2300
Wire Wire Line
	6750 2400 5300 2400
Wire Wire Line
	5300 2500 6750 2500
Wire Wire Line
	6750 2600 5300 2600
Wire Wire Line
	6750 3400 5300 3400
Wire Wire Line
	5300 3500 6750 3500
Wire Wire Line
	6750 3600 5300 3600
Wire Wire Line
	5300 3700 6750 3700
Wire Wire Line
	5300 3800 6750 3800
Wire Wire Line
	5300 3900 6750 3900
Wire Wire Line
	5300 4000 6750 4000
Wire Wire Line
	6750 4100 5300 4100
Wire Wire Line
	5300 4600 6750 4600
Wire Wire Line
	5300 4700 6750 4700
Wire Wire Line
	5300 4800 6750 4800
Wire Wire Line
	5300 4900 6750 4900
Wire Wire Line
	5300 5000 6750 5000
Wire Wire Line
	5300 5500 6750 5500
Wire Wire Line
	6750 5600 5300 5600
Wire Wire Line
	5300 5700 6750 5700
Wire Wire Line
	5300 5800 6750 5800
Wire Wire Line
	5300 5900 6750 5900
Wire Wire Line
	5300 6000 6750 6000
Wire Wire Line
	5300 6100 6750 6100
Text Notes 7700 2150 0    60   ~ 0
Copyright 2014 Jonas Hagmar\n\nThis file is part of keynjector.\n\nkeynjector is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 2 of the License, or\n(at your option) any later version.\n\nkeynjector is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with keynjector.  If not, see <http://www.gnu.org/licenses/>
$EndSCHEMATC
