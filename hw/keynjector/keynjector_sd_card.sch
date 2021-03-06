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
Sheet 7 7
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
L GND #PWR060
U 1 1 53A298FF
P 6250 5250
F 0 "#PWR060" H 6250 5250 30  0001 C CNN
F 1 "GND" H 6250 5180 30  0001 C CNN
F 2 "" H 6250 5250 60  0000 C CNN
F 3 "" H 6250 5250 60  0000 C CNN
	1    6250 5250
	1    0    0    -1  
$EndComp
Text Label 3900 4050 0    60   ~ 0
MCDA0
Text Label 3900 4150 0    60   ~ 0
MCDA1
Text Label 3900 4250 0    60   ~ 0
MCDA2
Text Label 3900 4350 0    60   ~ 0
MCDA3
Text Label 3900 4450 0    60   ~ 0
MCCK
Text Label 3900 4550 0    60   ~ 0
MCCDA
Text Label 3900 4650 0    60   ~ 0
MCCD
$Comp
L R R5
U 1 1 53A29A41
P 4400 3650
F 0 "R5" V 4480 3650 40  0000 C CNN
F 1 "10k" V 4407 3651 40  0000 C CNN
F 2 "~" V 4330 3650 30  0000 C CNN
F 3 "~" H 4400 3650 30  0000 C CNN
	1    4400 3650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR061
U 1 1 53A29C5E
P 7850 4300
F 0 "#PWR061" H 7850 4300 30  0001 C CNN
F 1 "GND" H 7850 4230 30  0001 C CNN
F 2 "" H 7850 4300 60  0000 C CNN
F 3 "" H 7850 4300 60  0000 C CNN
	1    7850 4300
	1    0    0    -1  
$EndComp
$Comp
L C C19
U 1 1 53A29C8C
P 8100 4250
F 0 "C19" H 8100 4350 40  0000 L CNN
F 1 "100n" H 8106 4165 40  0000 L CNN
F 2 "~" H 8138 4100 30  0000 C CNN
F 3 "~" H 8100 4250 60  0000 C CNN
	1    8100 4250
	1    0    0    -1  
$EndComp
$Comp
L C C20
U 1 1 53A29C99
P 8350 4250
F 0 "C20" H 8350 4350 40  0000 L CNN
F 1 "10u" H 8356 4165 40  0000 L CNN
F 2 "~" H 8388 4100 30  0000 C CNN
F 3 "~" H 8350 4250 60  0000 C CNN
	1    8350 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR062
U 1 1 53A29CB7
P 8100 4500
F 0 "#PWR062" H 8100 4500 30  0001 C CNN
F 1 "GND" H 8100 4430 30  0001 C CNN
F 2 "" H 8100 4500 60  0000 C CNN
F 3 "" H 8100 4500 60  0000 C CNN
	1    8100 4500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR063
U 1 1 53A29CBD
P 8350 4500
F 0 "#PWR063" H 8350 4500 30  0001 C CNN
F 1 "GND" H 8350 4430 30  0001 C CNN
F 2 "" H 8350 4500 60  0000 C CNN
F 3 "" H 8350 4500 60  0000 C CNN
	1    8350 4500
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 53A2A1EB
P 6250 5000
F 0 "R6" V 6330 5000 40  0000 C CNN
F 1 "39R" V 6257 5001 40  0000 C CNN
F 2 "~" V 6180 5000 30  0000 C CNN
F 3 "~" H 6250 5000 30  0000 C CNN
	1    6250 5000
	-1   0    0    1   
$EndComp
Text HLabel 3800 4050 0    60   BiDi ~ 0
MCDA0
Text HLabel 3800 4150 0    60   BiDi ~ 0
MCDA1
Text HLabel 3800 4250 0    60   BiDi ~ 0
MCDA2
Text HLabel 3800 4350 0    60   BiDi ~ 0
MCDA3
Text HLabel 3800 4450 0    60   Input ~ 0
MCCK
Text HLabel 3800 4550 0    60   Input ~ 0
MCCDA
Text HLabel 3800 4650 0    60   Output ~ 0
MCCD
Wire Wire Line
	6450 4050 3800 4050
Wire Wire Line
	6450 4150 3800 4150
Wire Wire Line
	6450 4250 3800 4250
Wire Wire Line
	6450 4350 3800 4350
Wire Wire Line
	6450 4450 3800 4450
Wire Wire Line
	6450 4550 3800 4550
Wire Wire Line
	6450 4750 6250 4750
Wire Wire Line
	4400 3350 4400 3400
Wire Wire Line
	4400 3900 4400 4550
Connection ~ 4400 4550
Wire Wire Line
	7850 4150 7850 4300
Wire Wire Line
	7850 4050 8350 4050
Wire Wire Line
	8350 4050 8350 4000
Wire Wire Line
	3800 4650 6450 4650
Connection ~ 8100 4050
Wire Wire Line
	8100 4450 8100 4500
Wire Wire Line
	8350 4450 8350 4500
$Comp
L VCC_MCU_P3V3 #PWR064
U 1 1 53A36C03
P 4400 3350
F 0 "#PWR064" H 4400 3450 30  0001 C CNN
F 1 "VCC_MCU_P3V3" H 4400 3450 30  0000 C CNN
F 2 "" H 4400 3350 60  0000 C CNN
F 3 "" H 4400 3350 60  0000 C CNN
	1    4400 3350
	1    0    0    -1  
$EndComp
$Comp
L VCC_MCU_P3V3 #PWR065
U 1 1 53A36C49
P 8350 4000
F 0 "#PWR065" H 8350 4100 30  0001 C CNN
F 1 "VCC_MCU_P3V3" H 8350 4100 30  0000 C CNN
F 2 "" H 8350 4000 60  0000 C CNN
F 3 "" H 8350 4000 60  0000 C CNN
	1    8350 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR066
U 1 1 53A9E5B3
P 7850 4750
F 0 "#PWR066" H 7850 4750 30  0001 C CNN
F 1 "GND" H 7850 4680 30  0001 C CNN
F 2 "" H 7850 4750 60  0000 C CNN
F 3 "" H 7850 4750 60  0000 C CNN
	1    7850 4750
	0    -1   -1   0   
$EndComp
$Comp
L 492250821 J4
U 1 1 53A9F531
P 7150 4400
F 0 "J4" H 6750 5000 60  0000 L CNN
F 1 "492250821" H 6750 4900 60  0000 L CNN
F 2 "" H 7050 4450 60  0000 C CNN
F 3 "" H 7050 4450 60  0000 C CNN
	1    7150 4400
	1    0    0    -1  
$EndComp
Text Notes 6200 3200 0    60   ~ 0
Pull up MCDA0-MCDA3 and C/D_A.
Text Notes 850  800  0    60   ~ 0
Copyright 2014 Jonas Hagmar\n\nThis file is part of keynjector.\n\nkeynjector is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 2 of the License, or\n(at your option) any later version.\n\nkeynjector is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with keynjector.  If not, see <http://www.gnu.org/licenses/>
$EndSCHEMATC
