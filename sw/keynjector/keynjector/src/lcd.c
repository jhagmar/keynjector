/*
* Copyright 2014 Jonas Hagmar
*
* This file is part of keynjector.
*
* keynjector is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* keynjector is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with keynjector.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <asf.h>
#include "lcd.h"

#ifndef LCD_INLINE
static void lcd_write_command(ioport_port_mask_t command) {
	ioport_set_pin_level(TP_READ, true);
	ioport_set_pin_level(TP_DC, false);
	ioport_set_pin_level(TP_CS, false);
	ioport_set_pin_level(TP_WRITE, false);
	ioport_set_port_level(TP_DATA_PORT, TP_DATA_MASK, command);
	ioport_set_pin_level(TP_WRITE, true);
	ioport_set_pin_level(TP_DC, true);
}

static void lcd_write_data(ioport_port_mask_t command) {
	ioport_set_pin_level(TP_READ, true);
	ioport_set_pin_level(TP_DC, true);
	ioport_set_pin_level(TP_CS, false);
	ioport_set_pin_level(TP_WRITE, false);
	ioport_set_port_level(TP_DATA_PORT, TP_DATA_MASK, command);
	ioport_set_pin_level(TP_WRITE, true);
}

#define ILI9341_CMD_COLUMN_ADDRESS_SET 0x2A
#define ILI9341_CMD_PAGE_ADDRESS_SET 0x2B
#define ILI9341_CMD_MEMORY_WRITE 0x2C

void lcd_select_window(int x0, int y0, int x1, int y1) {
	lcd_write_command(ILI9341_CMD_COLUMN_ADDRESS_SET);
	lcd_write_data((x0 >> 8) & 0xFF);
	lcd_write_data(x0 & 0xFF);
	lcd_write_data((x1 >> 8) & 0xFF);
	lcd_write_data(x1 & 0xFF);
	
	lcd_write_command(ILI9341_CMD_PAGE_ADDRESS_SET);
	lcd_write_data((y0 >> 8) & 0xFF);
	lcd_write_data(y0 & 0xFF);
	lcd_write_data((y1 >> 8) & 0xFF);
	lcd_write_data(y1 & 0xFF);
	
	lcd_write_command(ILI9341_CMD_MEMORY_WRITE);
}

void lcd_write_pixel(int r, int g, int b) {
	lcd_write_data((unsigned char )b);
	lcd_write_data((unsigned char )g);
	lcd_write_data((unsigned char )r);
}

#endif
	
void lcd_init(void) {
	
	//display OFF
	lcd_write_command(0x28);
	
	//exit SLEEP mode
	lcd_write_command(0x11);
	lcd_write_data(0x00);
	
	//Power Control A
	lcd_write_command(0xCB);
	lcd_write_data(0x39); //always 0x39
	lcd_write_data(0x2C); //always 0x2C
	lcd_write_data(0x00); //always 0x00
	lcd_write_data(0x34); //Vcore = 1.6V
	lcd_write_data(0x02); //DDVDH = 5.6V
	
	//Power Control B
	lcd_write_command(0xCF);
	lcd_write_data(0x00); //always 0x00
	lcd_write_data(0x81); //PCEQ off
	lcd_write_data(0x30); //ESD protection

	//Driver timing control A
	lcd_write_command(0xE8);
	lcd_write_data(0x85); //non?overlap
	lcd_write_data(0x01); //EQ timing
	lcd_write_data(0x79); //Pre?charge timing
	
	//Driver timing control B
	lcd_write_command(0xEA);
	lcd_write_data(0x00); //Gate driver timing
	lcd_write_data(0x00); //always 0x00
	
	//Power?On sequence control
	lcd_write_command(0xED);
	lcd_write_data(0x64); //soft start
	lcd_write_data(0x03); //power on sequence
	lcd_write_data(0x12); //power on sequence
	lcd_write_data(0x81); //DDVDH enhance on
	
	//Pump ratio control
	lcd_write_command(0xF7); 
	lcd_write_data(0x20); //DDVDH=2xVCI
	
	//power control 1
	lcd_write_command(0xC0);
	lcd_write_data(0x26);
	
	//power control 2
	lcd_write_command(0xC1);
	lcd_write_data(0x11); 
	
	//VCOM control 1
	lcd_write_command(0xC5);
	lcd_write_data(0x35);
	lcd_write_data(0x3E);
	
	//VCOM control 2
	lcd_write_command(0xC7);
	lcd_write_data(0xBE);
	
	//memory access control = BGR
	lcd_write_command(0x36);
	lcd_write_data(0x88);
	
	//frame rate control
	lcd_write_command(0xB1);
	lcd_write_data(0x00);
	lcd_write_data(0x10);
	
	//display function control
	lcd_write_command(0xB6);
	lcd_write_data(0x0A);
	lcd_write_data(0xA2);
	
	// pixel format = 18 bit per pixel
	lcd_write_command(0x3A);
	lcd_write_data(0x06);
	
	//3G Gamma control
	lcd_write_command(0xF2);
	lcd_write_data(0x02); //off
	
	//Gamma curve 3
	lcd_write_command(0x26);
	lcd_write_data(0x01); 
	
	// set row order
	lcd_write_command(0x36);
	lcd_write_data(0x00);
	
	// set brightness
	lcd_write_command(0x51);
	lcd_write_data(0xFF);
	
	// display on
	lcd_write_command(0x29);
}

