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
#ifndef KEYNJECTOR_UI
#define KEYNJECTOR_UI

/// keynjector user interface

// screen dimensions
#define LCD_HEIGHT 320
#define LCD_WIDTH 240

// screen coordinates for calibration points 
#define CALIB_POINTS_X1 30
#define CALIB_POINTS_Y1 30
#define CALIB_POINTS_X2 210
#define CALIB_POINTS_Y2 290
#define CALIB_POINTS_X0 (LCD_WIDTH / 2)
#define CALIB_POINTS_Y0 (LCD_HEIGHT / 2)

// available views
#define VIEW_NONE 0
#define VIEW_LOGO 1
#define VIEW_CALIB 2
#define VIEW_TITLE 3
#define VIEW_PASS 4
#define VIEW_PIN 5
#define VIEW_KEY 6

// ui commands
#define CMD_CALIB_LBL_FADE_IN 1
#define CMD_CALIB_LBL_FADE_OUT 2
#define CMD_CALIB_POINT_1_FADE_IN 3
#define CMD_CALIB_POINT_1_FADE_OUT 4
#define CMD_CALIB_POINT_2_FADE_IN 5
#define CMD_CALIB_POINT_2_FADE_OUT 6
#define CMD_CALIB_POINT_3_FADE_IN 7
#define CMD_CALIB_POINT_3_FADE_OUT 8
#define CMD_CALIB_POINT_4_FADE_IN 9
#define CMD_CALIB_POINT_4_FADE_OUT 10
#define CMD_CALIB_POINT_5_FADE_IN 11
#define CMD_CALIB_POINT_5_FADE_OUT 12
#define CMD_FADE_IN 13
#define CMD_FADE_OUT 14

// set the current view
void lcd_set_view(int view);

// get the current view number
int lcd_get_view(void);

// signal timer tick to UI
void lcd_timer_tick(void);

// start a UI command
void lcd_start_command(int command);

// set message of "title" screen
void lcd_set_title_msg(char const *msg);

// set message of passphrase screen
void lcd_set_passphrase_msg(char const *msg);

// set the buffer and length of buffer for storing the passphrase
void lcd_set_passphrase_buffer(char *buf, int buflen);

// set the message of the PIN code screen
void lcd_set_pin_msg(char const *msg);

// set the buffer and length of buffer for storing the PIN code
void lcd_set_pin_buffer(char *buf, int buflen);

// set the key-value map with passwords to present to the user
int lcd_set_keys(char *keys);

// signal mouse down event to UI
void view_mouse_down(int x, int y);

// signal mouse up event to UI
void view_mouse_up(int x, int y);

// signal mouse move event to UI
void view_mouse_move(int x, int y);

//void convert_logo(void);
//void convert_font(void);
//void write_bg_data(void);
//void write_corner_data(void);

#endif