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
Sheet 2 7
Title ""
Date "2 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PRTR5V0U2X U1
U 1 1 53A03D6D
P 3950 4500
F 0 "U1" H 3450 4900 60  0000 L CNN
F 1 "PRTR5V0U2X" H 3450 4800 60  0000 L CNN
F 2 "" H 3950 4500 60  0000 C CNN
F 3 "" H 3950 4500 60  0000 C CNN
	1    3950 4500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 53A03D73
P 2650 4450
F 0 "#PWR01" H 2650 4450 30  0001 C CNN
F 1 "GND" H 2650 4380 30  0001 C CNN
F 2 "" H 2650 4450 60  0000 C CNN
F 3 "" H 2650 4450 60  0000 C CNN
	1    2650 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 53A03D79
P 2750 3700
F 0 "#PWR02" H 2750 3700 30  0001 C CNN
F 1 "GND" H 2750 3630 30  0001 C CNN
F 2 "" H 2750 3700 60  0000 C CNN
F 3 "" H 2750 3700 60  0000 C CNN
	1    2750 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 3150 5100 3150
Wire Wire Line
	2650 4450 2650 4400
Wire Wire Line
	2650 4400 3150 4400
Wire Wire Line
	2750 3300 5100 3300
Wire Wire Line
	2950 3300 2950 4600
Connection ~ 2950 3300
Wire Wire Line
	2950 4600 3150 4600
Wire Wire Line
	2750 3000 6100 3000
Wire Wire Line
	4800 3000 4800 4400
Wire Wire Line
	4800 4400 4750 4400
Connection ~ 4800 3000
Wire Wire Line
	4750 4600 4900 4600
Wire Wire Line
	4900 4600 4900 3150
Connection ~ 4900 3150
Wire Wire Line
	2750 3600 2750 3700
Wire Wire Line
	5100 3000 5100 2650
Connection ~ 5100 3000
$Comp
L GND #PWR03
U 1 1 53A03D9E
P 6000 3250
F 0 "#PWR03" H 6000 3250 30  0001 C CNN
F 1 "GND" H 6000 3180 30  0001 C CNN
F 2 "" H 6000 3250 60  0000 C CNN
F 3 "" H 6000 3250 60  0000 C CNN
	1    6000 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 3200 6000 3200
Wire Wire Line
	6000 3200 6000 3250
Wire Wire Line
	5700 3000 5700 3400
Wire Wire Line
	5700 3400 6100 3400
Connection ~ 5700 3000
Wire Wire Line
	7950 3100 9050 3100
$Comp
L GND #PWR04
U 1 1 53A03DB0
P 8600 3750
F 0 "#PWR04" H 8600 3750 30  0001 C CNN
F 1 "GND" H 8600 3680 30  0001 C CNN
F 2 "" H 8600 3750 60  0000 C CNN
F 3 "" H 8600 3750 60  0000 C CNN
	1    8600 3750
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C1
U 1 1 53A03DB6
P 8600 3400
F 0 "C1" H 8650 3500 40  0000 L CNN
F 1 "10u" H 8650 3300 40  0000 L CNN
F 2 "" H 8700 3250 30  0000 C CNN
F 3 "" H 8600 3400 300 0000 C CNN
	1    8600 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 3200 8600 3100
Connection ~ 8600 3100
Wire Wire Line
	8600 3600 8600 3750
Text HLabel 5100 3150 2    60   BiDi ~ 0
USB_N
Text HLabel 5100 3300 2    60   BiDi ~ 0
USB_P
NoConn ~ 2750 3450
$Comp
L VCC_USB_P5V0 #PWR05
U 1 1 53A34D4D
P 5100 2650
F 0 "#PWR05" H 5100 2750 30  0001 C CNN
F 1 "VCC_USB_P5V0" H 5100 2750 30  0000 C CNN
F 2 "" H 5100 2650 60  0000 C CNN
F 3 "" H 5100 2650 60  0000 C CNN
	1    5100 2650
	1    0    0    -1  
$EndComp
$Comp
L VCC_MCU_P3V3 #PWR06
U 1 1 53A34E0E
P 9050 3100
F 0 "#PWR06" H 9050 3200 30  0001 C CNN
F 1 "VCC_MCU_P3V3" H 9050 3200 30  0000 C CNN
F 2 "" H 9050 3100 60  0000 C CNN
F 3 "" H 9050 3100 60  0000 C CNN
	1    9050 3100
	1    0    0    -1  
$EndComp
$Comp
L SPX3819M5-L/TR U2
U 1 1 53A7E4E5
P 7050 3450
F 0 "U2" H 6400 4200 60  0000 L CNN
F 1 "SPX3819M5-L/TR" H 6400 4100 60  0000 L CNN
F 2 "" H 7050 3500 60  0000 C CNN
F 3 "" H 7050 3500 60  0000 C CNN
	1    7050 3450
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 53A7E4F4
P 8150 3350
F 0 "R1" V 8230 3350 40  0000 C CNN
F 1 "56k" V 8157 3351 40  0000 C CNN
F 2 "~" V 8080 3350 30  0000 C CNN
F 3 "~" H 8150 3350 30  0000 C CNN
	1    8150 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 3600 8150 3900
$Comp
L R R2
U 1 1 53A7E527
P 8150 4150
F 0 "R2" V 8230 4150 40  0000 C CNN
F 1 "36k" V 8157 4151 40  0000 C CNN
F 2 "~" V 8080 4150 30  0000 C CNN
F 3 "~" H 8150 4150 30  0000 C CNN
	1    8150 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 3300 7950 3750
Wire Wire Line
	7950 3750 8150 3750
Connection ~ 8150 3750
Wire Wire Line
	8150 4400 8150 4500
$Comp
L GND #PWR07
U 1 1 53A7E56E
P 8150 4500
F 0 "#PWR07" H 8150 4500 30  0001 C CNN
F 1 "GND" H 8150 4430 30  0001 C CNN
F 2 "" H 8150 4500 60  0000 C CNN
F 3 "" H 8150 4500 60  0000 C CNN
	1    8150 4500
	1    0    0    -1  
$EndComp
Connection ~ 8150 3100
$Comp
L 10118192-0001LF J1
U 1 1 53A9D96C
P 2300 3300
F 0 "J1" H 2050 3900 60  0000 C CNN
F 1 "10118192-0001LF" H 2000 3800 60  0000 L CNN
F 2 "" H 2150 3250 60  0000 C CNN
F 3 "" H 2150 3250 60  0000 C CNN
	1    2300 3300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 53A9D979
P 2300 4050
F 0 "#PWR08" H 2300 4050 30  0001 C CNN
F 1 "GND" H 2300 3980 30  0001 C CNN
F 2 "" H 2300 4050 60  0000 C CNN
F 3 "" H 2300 4050 60  0000 C CNN
	1    2300 4050
	1    0    0    -1  
$EndComp
$Comp
L TP TP5
U 1 1 53AF2A15
P 4150 2750
F 0 "TP5" H 4150 3000 60  0000 C CNN
F 1 "TP" H 4150 2800 60  0001 C CNN
F 2 "~" H 4150 2750 60  0000 C CNN
F 3 "~" H 4150 2750 60  0000 C CNN
	1    4150 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 2750 4150 3000
Connection ~ 4150 3000
Text Notes 7700 800  0    60   ~ 0
Copyright 2014 Jonas Hagmar\n\nThis file is part of keynjector.\n\nkeynjector is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 2 of the License, or\n(at your option) any later version.\n\nkeynjector is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with keynjector.  If not, see <http://www.gnu.org/licenses/>
$EndSCHEMATC
