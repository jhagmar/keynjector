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
Sheet 4 7
Title ""
Date "6 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 3550 3600 0    60   Input ~ 0
XOUT12
Text HLabel 7700 3600 2    60   Input ~ 0
XIN12
$Comp
L C C14
U 1 1 53A1636E
P 4400 4100
F 0 "C14" H 4400 4200 40  0000 L CNN
F 1 "15p" H 4406 4015 40  0000 L CNN
F 2 "~" H 4438 3950 30  0000 C CNN
F 3 "~" H 4400 4100 60  0000 C CNN
	1    4400 4100
	1    0    0    -1  
$EndComp
$Comp
L C C15
U 1 1 53A16387
P 6900 3950
F 0 "C15" H 6900 4050 40  0000 L CNN
F 1 "15p" H 6906 3865 40  0000 L CNN
F 2 "~" H 6938 3800 30  0000 C CNN
F 3 "~" H 6900 3950 60  0000 C CNN
	1    6900 3950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR039
U 1 1 53A1638F
P 4400 4350
F 0 "#PWR039" H 4400 4350 30  0001 C CNN
F 1 "GND" H 4400 4280 30  0001 C CNN
F 2 "" H 4400 4350 60  0000 C CNN
F 3 "" H 4400 4350 60  0000 C CNN
	1    4400 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR040
U 1 1 53A1639C
P 6900 4200
F 0 "#PWR040" H 6900 4200 30  0001 C CNN
F 1 "GND" H 6900 4130 30  0001 C CNN
F 2 "" H 6900 4200 60  0000 C CNN
F 3 "" H 6900 4200 60  0000 C CNN
	1    6900 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 3600 4800 3600
Wire Wire Line
	4400 3900 4400 3600
Connection ~ 4400 3600
Wire Wire Line
	6450 3600 7700 3600
Wire Wire Line
	6900 3750 6900 3600
Connection ~ 6900 3600
Text Label 3800 3600 0    60   ~ 0
XOUT12
Text Label 7200 3600 0    60   ~ 0
XIN12
Wire Wire Line
	4400 4300 4400 4350
Wire Wire Line
	6900 4150 6900 4200
$Comp
L 7A-12.000MAAJ-T XC1
U 1 1 53B10BD0
P 5600 3600
F 0 "XC1" H 5100 3850 60  0000 L CNN
F 1 "7A-12.000MAAJ-T" H 5100 3950 60  0000 L CNN
F 2 "" H 5600 3600 60  0000 C CNN
F 3 "" H 5600 3600 60  0000 C CNN
	1    5600 3600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
