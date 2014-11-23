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
Sheet 5 7
Title ""
Date "6 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L M50-3600542 J2
U 1 1 53A16A20
P 5800 3650
F 0 "J2" H 5350 4100 60  0000 L CNN
F 1 "M50-3600542" H 5350 4000 60  0000 L CNN
F 2 "" H 5650 3450 60  0000 C CNN
F 3 "" H 5650 3450 60  0000 C CNN
	1    5800 3650
	1    0    0    -1  
$EndComp
NoConn ~ 5050 3750
$Comp
L GND #PWR041
U 1 1 53A16A42
P 4500 4850
F 0 "#PWR041" H 4500 4850 30  0001 C CNN
F 1 "GND" H 4500 4780 30  0001 C CNN
F 2 "" H 4500 4850 60  0000 C CNN
F 3 "" H 4500 4850 60  0000 C CNN
	1    4500 4850
	1    0    0    -1  
$EndComp
Text Label 7800 3450 0    60   ~ 0
TMS
Text Label 7800 3650 0    60   ~ 0
TDO
Text Label 7800 3750 0    60   ~ 0
TDI
Text Label 7800 3850 0    60   ~ 0
NRST
Text HLabel 8100 3450 2    60   BiDi ~ 0
TMS
Text HLabel 8100 3550 2    60   BiDi ~ 0
TCK
Text HLabel 8100 3650 2    60   BiDi ~ 0
TDO
Text HLabel 8100 3750 2    60   BiDi ~ 0
TDI
Text HLabel 8100 3850 2    60   BiDi ~ 0
NRST
$Comp
L VCC_MCU_P3V3 #PWR042
U 1 1 53A3679A
P 4500 2500
F 0 "#PWR042" H 4500 2600 30  0001 C CNN
F 1 "VCC_MCU_P3V3" H 4500 2600 30  0000 C CNN
F 2 "" H 4500 2500 60  0000 C CNN
F 3 "" H 4500 2500 60  0000 C CNN
	1    4500 2500
	1    0    0    -1  
$EndComp
Text Notes 6350 2600 0    60   ~ 0
Pull up TMS, TDI (and NRST, which has a permanent pull-up). Pull down TCK.
Wire Wire Line
	4500 2500 4500 3450
Wire Wire Line
	4500 3450 5050 3450
Wire Wire Line
	5050 3550 4500 3550
Wire Wire Line
	4500 3550 4500 4850
Wire Wire Line
	5050 3650 4500 3650
Connection ~ 4500 3650
Wire Wire Line
	5050 3850 4500 3850
Connection ~ 4500 3850
Wire Wire Line
	6550 3450 8100 3450
Wire Wire Line
	6550 3650 8100 3650
Wire Wire Line
	6550 3850 8100 3850
Text Label 7800 3550 0    60   ~ 0
TCK
Wire Wire Line
	6550 3550 8100 3550
Wire Wire Line
	6550 3750 8100 3750
$EndSCHEMATC
