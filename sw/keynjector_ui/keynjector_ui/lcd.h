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
#ifndef LCD_H_
#define LCD_H_

/// Driver for the LCD

/// Initialize the LCD
void lcd_init(void);

#ifndef LCD_INLINE
/// Select the window to which pixel data is written
void lcd_select_window(int x0, int y0, int x1, int y1);
/// Write a pixel to the next cursor position
void lcd_write_pixel(int r, int g, int b);
#else
#include <asf.h>
#define lcd_write_command(command) ioport_set_pin_level(TP_READ, true);\
ioport_set_pin_level(TP_DC, false);\
ioport_set_pin_level(TP_CS, false);\
ioport_set_pin_level(TP_WRITE, false);\
ioport_set_port_level(TP_DATA_PORT, TP_DATA_MASK, (command));\
ioport_set_pin_level(TP_WRITE, true);\
ioport_set_pin_level(TP_DC, true);

#define lcd_write_data(command) ioport_set_pin_level(TP_READ, true);\
ioport_set_pin_level(TP_DC, true);\
ioport_set_pin_level(TP_CS, false);\
ioport_set_pin_level(TP_WRITE, false);\
ioport_set_port_level(TP_DATA_PORT, TP_DATA_MASK, (command));\
ioport_set_pin_level(TP_WRITE, true);

#define ILI9341_CMD_COLUMN_ADDRESS_SET 0x2A
#define ILI9341_CMD_PAGE_ADDRESS_SET 0x2B
#define ILI9341_CMD_MEMORY_WRITE 0x2C

#define lcd_select_window(x0, y0, x1, y1) lcd_write_command(ILI9341_CMD_COLUMN_ADDRESS_SET);\
	lcd_write_data(((x0) >> 8) & 0xFF);\
	lcd_write_data((x0) & 0xFF);\
	lcd_write_data(((x1) >> 8) & 0xFF);\
	lcd_write_data((x1) & 0xFF);\
	lcd_write_command(ILI9341_CMD_PAGE_ADDRESS_SET);\
	lcd_write_data(((y0) >> 8) & 0xFF);\
	lcd_write_data((y0) & 0xFF);\
	lcd_write_data(((y1) >> 8) & 0xFF);\
	lcd_write_data((y1) & 0xFF);\
	lcd_write_command(ILI9341_CMD_MEMORY_WRITE);

#define lcd_write_pixel(r, g, b) lcd_write_data((unsigned char )(b));\
	lcd_write_data((unsigned char )(g));\
	lcd_write_data((unsigned char )(r));
#endif

#endif