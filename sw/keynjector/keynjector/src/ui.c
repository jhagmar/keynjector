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
#include <string.h>
#include "ui.h"
#include "ui_data.h"
#include "lcd.h"

// callback when UI command has been completed
extern void lcd_command_complete(int command);

// callback when passphrase view OK button has been clicked
extern void lcd_passphrase_ok(void);

// callback when PIN view OK button has been clicked
extern void lcd_pin_ok(void);

// callback when long press on a password item has been detected
extern void lcd_item_long_press(char *value);

#define ANIMATION_TICKS 10 // number of timer ticks for fade ins/outs
#define LONG_PRESS_TICKS 20 // number of timer ticks for long press detection

//#define TOP_R 30
//#define TOP_G 87
//#define TOP_B 153

//#define BOT_R 91
//#define BOT_G 169
//#define BOT_B 229

// allowed password characters
static char const *LBX_VALUE_CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

/*typedef struct {
	int r,g,b;
	int dr, dg, db;
	int er, eg, eb;
} bkrnd_ctx_t;*/

/*static void background_init(bkrnd_ctx_t *ctx) {
	ctx->r = TOP_R;
	ctx->g = TOP_G;
	ctx->b = TOP_B;
	ctx->dr = BOT_R - TOP_R;
	ctx->dg = BOT_G - TOP_G;
	ctx->db = BOT_B - TOP_B;
	ctx->er = ctx->eg = ctx->eb = 0;
}*/

/*static void background_advance(bkrnd_ctx_t *ctx) {
	ctx->er += ctx->dr;
	if ((ctx->er << 1) >= LCD_HEIGHT) {
		ctx->r++; ctx->er -= LCD_HEIGHT;
	}
	ctx->eg += ctx->dg;
	if ((ctx->eg << 1) >= LCD_HEIGHT) {
		ctx->g++; ctx->eg -= LCD_HEIGHT;
	}
	ctx->eb += ctx->db;
	if ((ctx->eb << 1) >= LCD_HEIGHT) {
		ctx->b++; ctx->eb -= LCD_HEIGHT;
	}
}*/

/*static void background_draw_line(bkrnd_ctx_t *ctx) {
	int x;

	int r,g,b;

	r = ctx->r;
	g = ctx->g;
	b = ctx->b;

	for (x = 0; x < LCD_WIDTH; x++) {
		lcd_write_pixel(r, g, b);
	}

	background_advance(ctx);
}*/

// draw background gradient in specified area
static void draw_background(int x, int y, int width, int height) {

	int x0, x1, y1;
	unsigned char r, g, b;

	x0 = x;
	x1 = x + width;
	y1 = y + height;

	lcd_select_window(x, y, x1 - 1, y1 - 1);

	for ( ; y < y1; y++) {
		r = bg[y].r;
		g = bg[y].g;
		b = bg[y].b;
		for (x = x0; x < x1; x++) {
			lcd_write_pixel(r, g, b);
		}
	}

}

typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned char pixel_data[];
} bmp_t;

// draw centered bitmap in specified location
// bitmaps are compressed with a simple RLE scheme
// bmp: bitmap to draw
// x, y: top left corner
// width: width of drawing area
// inverted: invert colors
// alpha: alpha used for drawing
static void draw_bmp(bmp_t *bmp, int x, int y, int width, int inverted, unsigned char alpha) {

	int x0, x1, x2, y2;
	int b_r, b_g, b_b;
	int r, g, b;
	int i;
	int c;
	int a;
	int background_weight;
	int pixel_color;

	x0 = x;
	x2 = x + width;
	y2 = y + bmp->height;
	lcd_select_window(x, y, x2 - 1, y2 - 1);

	x1 = ((width - bmp->width) >> 1) + x; // x coord where bmp starts
	i = 0;
	c = 0; // RLE count
	
	for ( ; y < y2; y++) {
		// cache background color for this y
		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;
		// draw left margin (background only)
		for (x = x0; x < x1; x++) {
			lcd_write_pixel(b_r, b_g, b_b);
		}
		// draw bitmap blended with background
		for (x = 0; x < (int)bmp->width; x++) {
			if (c == 0) {
				a = bmp->pixel_data[i++];
				// least significant bit = 0 => pixel repeat value follows
				if (!(a & 1)) {
					c = bmp->pixel_data[i++] - 1;
				}
				// invert if needed
				if (inverted) {
					a = 0xFF - a;
				}
				// blend colors
				a = (a * alpha) >> 8;
				background_weight = 0xFF - a;
				pixel_color = a * 0xFF;
				r = ( background_weight * b_r + pixel_color ) >> 8;
				g = ( background_weight * b_g + pixel_color ) >> 8;
				b = ( background_weight * b_b + pixel_color ) >> 8;
			}
			else {
				c--;
			}
			
			lcd_write_pixel(r, g, b);
		}
		x += x1;
		// right margin - backround only
		for ( ; x < x2; x++) {
			lcd_write_pixel(b_r, b_g, b_b);
		}
	}
}

#define FONT_HEIGHT 20
#define FULL_LENGTH 0

// determine pixel width of s if rendered
static int text_width(char const *s, size_t s_len) {

	int i,j,k;
	int x;
	unsigned char c,c2;
	int advance;
	int xoffset;
	int kerning;

	x = 0;
	if (s_len == FULL_LENGTH) {
		s_len = strlen(s);
	}

	for (i = 0; i < (int)s_len; i++) {
		c = s[i];
		xoffset = char_coord[c].xoffset;
		advance = char_coord[c].xadvance;

		// determine kerning
		if (i < (int)(s_len - 1)) {
			c2 = s[i+1];
			j = kern_index[c];
			k = kern_index[c+1];
			for ( ; j < k; j++) {
				if (kern[j].second == c2) {
					kerning = kern[j].amount;
					advance += kerning;
				}
			}
		}

		if (xoffset < 0) {
			x += xoffset;
		}

		x += advance;
	}

	return x;
}

// draw centered text on screen
// s: string to render
// x, y: top left corner
// y_start: offset for y
// width: width of drawing area
// height: height of drawing area
// back_fill: fill background
// alpha: alphas used for rendering
// dalpha: whether to step in alpha array for each row
static void draw_text(char const *s, int x, int y, int y_start, int width, int height, int back_fill, unsigned char *alpha, int dalpha) {

	int i,j,k;
	int x0, y0, y1, y2;
	size_t s_len;
	unsigned char c,c2;
	int c_height;
	int c_width;
	int left_pad;
	int right_pad;
	int top_pad;
	int bottom_pad;
	int advance;
	int xoffset;
	int yoffset;
	int total_width;
	int cl, cx, cy, ci;
	int r, g, b;
	int b_r, b_g, b_b;
	int a;
	int kerning;
	int s_width;
	int background_weight, text_color;
	int right_margin, left_margin;
	unsigned char *alpha0;

	// compute rendered text width for centering
	s_width = text_width(s, FULL_LENGTH);
	s_len = strlen(s);
	
	while (s_width > width) {
		s_len--;
		s_width = text_width(s, s_len);
	}

	x0 = x;
	y0 = y;
	y1 = y + height;
	alpha0 = alpha;

	left_margin = (width - s_width) >> 1; // width of left margin
	right_margin = width - left_margin - s_width; // width of right margin
	// left margin
	lcd_select_window(x, y, x + left_margin - 1, y1 - 1);
	for ( ; y < y1; y++) {
		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;
		
		if (back_fill) {
			background_weight = 0xFF - *alpha;
			r = (b_r * background_weight + 0xFF * *alpha) >> 8;
			g = (b_g * background_weight + 0xFF * *alpha) >> 8;
			b = (b_b * background_weight + 0xFF * *alpha) >> 8;
		}
		else {
			r = b_r;
			g = b_g;
			b = b_b;
		}

		for (k = 0; k < left_margin; k++) {
			lcd_write_pixel(r, g, b);
		}
		if (dalpha) alpha++;
	}

	x = x0 + left_margin;
	
	// render characters
	for (i = 0; i < (int)s_len; i++) {
		c = s[i];
		// cache character dimensions
		c_height = char_coord[c].height;
		c_width = char_coord[c].width;
		xoffset = char_coord[c].xoffset;
		yoffset = char_coord[c].yoffset;
		advance = char_coord[c].xadvance;

		// find kerning
		if (i < (int)(s_len - 1)) {
			c2 = s[i+1];
			j = kern_index[c];
			k = kern_index[c+1];
			for ( ; j < k; j++) {
				if (kern[j].second == c2) {
					kerning = kern[j].amount;
					advance += kerning;
				}
			}
		}

		cl = char_coord[c].x;
		cy = char_coord[c].y;
		
		// compute margins (paddings) for character
		if (xoffset < 0) {
			x += xoffset;
			left_pad = 0;
		}
		else {
			left_pad = xoffset;
		}
		top_pad = yoffset;
		bottom_pad = FONT_HEIGHT - top_pad - c_height;
		if (advance > (left_pad + c_width)) {
			right_pad = advance - left_pad - c_width;
		}
		else {
			right_pad = 0;
		}
		total_width = left_pad + c_width + right_pad;

		top_pad -= y_start;
		if (top_pad < 0) {
			c_height += top_pad;
			cy -= top_pad;
			top_pad = 0;
		}
		if (c_height < 0) {
			bottom_pad += c_height;
			c_height = 0;
		}

		y2 = FONT_HEIGHT - y_start - height;
		bottom_pad -= y2;
		if (bottom_pad < 0) {
			c_height += bottom_pad;
			bottom_pad = 0;
		}
		if (c_height < 0) {
			top_pad += c_height;
			c_height = 0;
		}

		lcd_select_window(x, y0, x + total_width - 1, y0 + height - 1);

		// top pad
		y2 = y0 + top_pad;
		alpha = alpha0;
		for (y = y0; y < y2; y++) {
			b_r = bg[y].r;
			b_g = bg[y].g;
			b_b = bg[y].b;

			if (back_fill) {
			background_weight = 0xFF - *alpha;
				r = (b_r * background_weight + 0xFF * *alpha) >> 8;
				g = (b_g * background_weight + 0xFF * *alpha) >> 8;
				b = (b_b * background_weight + 0xFF * *alpha) >> 8;
			}
			else {
				r = b_r;
				g = b_g;
				b = b_b;
			}

			for (k = 0; k < total_width; k++) {
				lcd_write_pixel(r, g, b);
			}
			if (dalpha) alpha++;
		}

		y2 += c_height;
		for ( ; y < y2; y++, cy++) {
			b_r = bg[y].r;
			b_g = bg[y].g;
			b_b = bg[y].b;
			//left pad
			for (k = 0; k < left_pad; k++) {
				if (back_fill) {
					background_weight = 0xFF - *alpha;
					r = (b_r * background_weight + 0xFF * *alpha) >> 8;
					g = (b_g * background_weight + 0xFF * *alpha) >> 8;
					b = (b_b * background_weight + 0xFF * *alpha) >> 8;
				}
				else {
					r = b_r;
					g = b_g;
					b = b_b;
				}
				lcd_write_pixel(r, g, b);
			}

			//character
			cx = cl;
			ci = (cy * font.width + cx) >> 1;
			for (k = 0; k < c_width; k++, cx++) {
				
				a = font.pixel_data[ci];

				if (cx & 1) {
					ci++;
					a = (a & 0xF) << 4;
				}
				else {
					a &= 0xF0;
				}
				if (back_fill) {
					a = 0xFF - a;
				}
				a = (a * *alpha) >> 8;
				background_weight = 0xFF - a;
				text_color = a * 0xFF;
				r = ( background_weight * b_r + text_color ) >> 8;
				g = ( background_weight * b_g + text_color ) >> 8;
				b = ( background_weight * b_b + text_color ) >> 8;
				lcd_write_pixel(r, g, b);
			}

			//right pad
			for (k = 0; k < right_pad; k++) {
				if (back_fill) {
				background_weight = 0xFF - *alpha;
					r = (b_r * background_weight + 0xFF * *alpha) >> 8;
					g = (b_g * background_weight + 0xFF * *alpha) >> 8;
					b = (b_b * background_weight + 0xFF * *alpha) >> 8;
				}
				else {
					r = b_r;
					g = b_g;
					b = b_b;
				}

				lcd_write_pixel(r, g, b);
			}
			if (dalpha) alpha++;
		}

		// bottom pad
		y2 += bottom_pad;
		for ( ; y < y2; y++) {
			b_r = bg[y].r;
			b_g = bg[y].g;
			b_b = bg[y].b;

			if (back_fill) {
			background_weight = 0xFF - *alpha;
				r = (b_r * background_weight + 0xFF * *alpha) >> 8;
				g = (b_g * background_weight + 0xFF * *alpha) >> 8;
				b = (b_b * background_weight + 0xFF * *alpha) >> 8;
			}
			else {
				r = b_r;
				g = b_g;
				b = b_b;
			}

			for (k = 0; k < total_width; k++) {
				lcd_write_pixel(r, g, b);
			}
			if (dalpha) alpha++;
		}

		x += advance;
	}

	// right margin
	y = y0;
	alpha = alpha0;
	lcd_select_window(x, y, x + right_margin - 1, y1 - 1);
	for ( ; y < y1; y++) {
		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;

		if (back_fill) {
			background_weight = 0xFF - *alpha;
			r = (b_r * background_weight + 0xFF * *alpha) >> 8;
			g = (b_g * background_weight + 0xFF * *alpha) >> 8;
			b = (b_b * background_weight + 0xFF * *alpha) >> 8;
		}
		else {
			r = b_r;
			g = b_g;
			b = b_b;
		}

		for (k = 0; k < right_margin; k++) {
			lcd_write_pixel(r, g, b);
		}
		if (dalpha) alpha++;
	}

}

// common corner pixel data type
typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned char	 pixel_data[];
} corner_t;

// draw a rectangle with rounded corners
// x,y: top left position
// width, height: dimensions of rectangle
// wx, wy: top left position of "window" in rectangle for text or bitmap
// wwidth, wheight: dimensions of window
// back_fill: whether to fill background
// alpha: alpha used for drawing
static void draw_rrect(int x, int y, int width, int height, int wx, int wy, int wwidth, int wheight, int back_fill, int alpha) {

	int x0, x1, x2, x3;
	int y0, y1, y2, y3;
	int bx, by;
	int a;
	int t;
	int i;
	int b_r, b_g, b_b;
	int r, g, b;
	int background_weight;
	int pixel_color;
	corner_t *c;

	// pick correct corner (filled or unfilled background)
	if (back_fill) {
		c = (corner_t*)&corner_fill;
	}
	else {
		c = (corner_t*)&corner;
	}

	// compute x positions
	x0 = x;
	x1 = wx;
	x2 = wx + wwidth;
	x3 = x + width;

	// compute y positions
	y0 = y;
	y1 = wy;
	y2 = wy + wheight;
	y3 = y + height;

	// draw left edge
	lcd_select_window(x0, y0, x1-1, y3-1);
	by = 0;
	for ( ; y < y3; ) {

		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;

		i = by * c->width;

		// draw line from corner data
		bx = 0;
		for (x = x0; x < x1; x++) {
			a = c->pixel_data[i];
			a = (a * alpha) >> 8;
			background_weight = 0xFF - a;
			pixel_color = a * 0xFF;
			r = ( background_weight * b_r + pixel_color ) >> 8;
			g = ( background_weight * b_g + pixel_color ) >> 8;
			b = ( background_weight * b_b + pixel_color ) >> 8;
			lcd_write_pixel(r, g, b);

			if (bx < ((int)c->width - 1)) { 
				bx++;
				i++;
			}
		}
		y++;
		t = y3 - y - 1;
		// close to bottom?
		if (t < (int)c->height) {
			// yes - go back up over bitmap to draw inverted corner
			by = t;
		}
		// close to top?
		else if (by < ((int)c->height - 1)) {
			// yes - advance down over bitmap to draw corner
			by++;
		}
	}

	// draw right edge similarly to left, only reversed
	lcd_select_window(x2, y0, x3-1, y3-1);
	by = 0;
	for (y = y0 ; y < y3; ) {

		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;

		bx = x3 - x2 - 1;
		if (bx >= (int)c->width) bx = c->width - 1;
		i = by * c->width + bx;

		for (x = x2; x < x3; x++) {

			if (bx > (x3 - x - 1)) { 
				bx--;
				i--;
			}

			a = c->pixel_data[i];
			a = (a * alpha) >> 8;
			background_weight = 0xFF - a;
			pixel_color = a * 0xFF;
			r = ( background_weight * b_r + pixel_color ) >> 8;
			g = ( background_weight * b_g + pixel_color ) >> 8;
			b = ( background_weight * b_b + pixel_color ) >> 8;
			lcd_write_pixel(r, g, b);
		}
		y++;
		t = y3 - y - 1;
		if (t < (int)c->height) {
			by = t;
		}
		else if (by < ((int)c->height - 1)) {
			by++;
		}
	}

	// draw top edge using last column of corner data
	lcd_select_window(x1, y0, x2-1, y1-1);
	i = c->width - 1;
	by = 0;
	for (y = y0 ; y < y1; ) {

		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;

		a = c->pixel_data[i];
		a = (a * alpha) >> 8;
		background_weight = 0xFF - a;
		pixel_color = a * 0xFF;
		r = ( background_weight * b_r + pixel_color ) >> 8;
		g = ( background_weight * b_g + pixel_color ) >> 8;
		b = ( background_weight * b_b + pixel_color ) >> 8;

		for (x = x1; x < x2; x++) {
			a = c->pixel_data[i];
			
			lcd_write_pixel(r, g, b);
		}
		y++;
		if (by < ((int)c->height - 1)) {
			by++;
			i += c->width;
		}
	}

	// draw bottom edge using last column of corner data
	lcd_select_window(x1, y2, x2-1, y3-1);
	by = y3 - y2 - 1;
	if (by >= (int)c->height) by = c->height - 1;
	i = (by + 1) * c->width - 1;
	for (y = y2 ; y < y3; ) {

		b_r = bg[y].r;
		b_g = bg[y].g;
		b_b = bg[y].b;

		a = c->pixel_data[i];
		a = (a * alpha) >> 8;
		background_weight = 0xFF - a;
		pixel_color = a * 0xFF;
		r = ( background_weight * b_r + pixel_color ) >> 8;
		g = ( background_weight * b_g + pixel_color ) >> 8;
		b = ( background_weight * b_b + pixel_color ) >> 8;

		for (x = x1; x < x2; x++) {
			a = c->pixel_data[i];
			
			lcd_write_pixel(r, g, b);
		}
		y++;
		if (by > (y3 - y - 1)) {
			by--;
			i -= c->width;
		}
	}

}

typedef struct control_t control_t;
typedef struct view_t view_t;

// common control data type
struct control_t {
	void (*render)(control_t *c); // render callback
	void (*mouse_down)(view_t *v, control_t *c, int x, int y); // mouse down callback
	void (*mouse_up)(view_t *v, control_t *c, int x, int y); // mouse up callback
	void (*mouse_move)(view_t *v, control_t *c, int x, int y); // mouse move callback
	// position
	int x;
	int y;
	// dimensions
	int width;
	int height;
};

// background control data type - no extra data needed
typedef struct {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
} bg_control_t;

// bitmap control data type
typedef struct {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
	bmp_t *bmp; // pointer to bitmap data
	int alpha; // alpha used for rendering
} bmp_control_t;

// label control data type
typedef struct {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
	char const *caption; // label caption
	int alpha; // alpha used for rendering
} lbl_control_t;

typedef struct btn_control_t btn_control_t;

// button control data type
struct btn_control_t {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
	char const *caption; // button caption
	int inverted; // whether to paint inverted
	int alpha; // alpha used for rendering
	void (*click)(btn_control_t *b); // click callback
};

// graphic button control data type
typedef struct {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
	bmp_t *bmp; // bitmap pointer
	int inverted; // whether to paint inverted
	int alpha; // alpha used for rendering
	void (*click)(btn_control_t *b); // click callback
} gbtn_control_t;

typedef struct lbx_control_t lbx_control_t;

// list box control data type
struct lbx_control_t {
	void (*render)(control_t *c);
	void (*mouse_down)(view_t *v, control_t *c, int x, int y);
	void (*mouse_up)(view_t *v, control_t *c, int x, int y);
	void (*mouse_move)(view_t *v, control_t *c, int x, int y);
	int x;
	int y;
	int width;
	int height;
	char *items; // items displayed in listbox
	int n_items; // number of items
	int origin; // origin of list
	int mouse_down_y; // y position of mouse down event
	int mouse_down_origin; // origin of mouse down event
	int selected_item; // currently selected item
	void (*timer_tick)(lbx_control_t *b); // timer tick
	void (*click)(lbx_control_t *b); // click callback
	void (*long_press)(lbx_control_t *b); // long press callback
	int long_press_timer; // timer for long press
	int alpha; // alpha used for rendering
};

// view data type
struct view_t {
	void (*capture)(view_t *v, control_t *c); // capture function, called to lock input to a control
	void (*timer_tick)(view_t *v);
	void (*start_command)(int command); // start (animation) command
	control_t *capturing_control; // current capturing control, receiving all mouse input
	control_t *controls[]; // array of controls belonging to this view
};

// currently displayed view
static view_t *current_view = NULL;

// render background control
static void bg_render(control_t *c) {
	bg_control_t *bc;

	bc = (bg_control_t*)c;
	draw_background(bc->x, bc->y, bc->width, bc->height);
}

// render bitmap control
static void bmp_render(control_t *c) {
	bmp_control_t *bc;

	bc = (bmp_control_t*)c;
	draw_bmp(bc->bmp, bc->x, bc->y, bc->width, 0, bc->alpha);
}

// render label control 
static void lbl_render(control_t *c) {
	lbl_control_t *lc;
	lc = (lbl_control_t*)c;
	unsigned char alpha = lc->alpha;
	draw_text(lc->caption, lc->x, lc->y, 0, lc->width, FONT_HEIGHT, 0, &alpha, 0);
}

// render button control
static void btn_render(control_t *c) {
	btn_control_t *bc;
	int wx, wy, wwidth;
	unsigned char alpha;

	bc = (btn_control_t*)c;
	// compute dimensions and position of centered caption
	wwidth = text_width(bc->caption, FULL_LENGTH);
	wx = ((bc->width - wwidth) >> 1) + bc->x;
	wy = ((bc->height - FONT_HEIGHT) >> 1) + bc->y;
	alpha = bc->alpha;
	draw_rrect(bc->x, bc->y, bc->width, bc->height, wx, wy, wwidth, FONT_HEIGHT, bc->inverted, bc->alpha);
	draw_text(bc->caption, wx, wy, 0, wwidth, FONT_HEIGHT, bc->inverted, &alpha, 0);
}

// render graphic button
static void gbtn_render(control_t *c) {
	gbtn_control_t *bc;
	int wx, wy, wwidth, wheight;

	bc = (gbtn_control_t*)c;
	// compute dimensions and position of centered bitmap
	wwidth = bc->bmp->width;
	wheight = bc->bmp->height;
	wx = ((bc->width - wwidth) >> 1) + bc->x;
	wy = ((bc->height - wheight) >> 1) + bc->y;
	draw_rrect(bc->x, bc->y, bc->width, bc->height, wx, wy, wwidth, wheight, bc->inverted, bc->alpha);
	draw_bmp(bc->bmp, wx, wy, wwidth, bc->inverted, bc->alpha);
}

// check if c is in listbox key charset
static int in_lbx_key_charset(char c) {
	// check if c is in the set of displayable characters
	if (char_coord[(int)c].xadvance == 0) return 0;
	return 1;
}

// count number of leading characters of s that are in
// listbox key charset
static int count_lbx_key_chars(char *s) {
	int i;

	i = 0;
	while (in_lbx_key_charset(s[i])) {
		i++;
	}
	return i;
}

// check if c is in listbox value charset
static int in_lbx_value_charset(char c) {
	int i;
	
	i = 0;
	while (LBX_VALUE_CHARSET[i] != 0) {
		if (LBX_VALUE_CHARSET[i] == c) return 1;
		i++;
	}
	return 0;
}

// count number of leading characters of s that are in
// listbox value charset
static int count_lbx_value_chars(char *s) {
	int i;

	i = 0;
	while (in_lbx_value_charset(s[i])) {
		i++;
	}
	return i;
}

// count number of key-value delimiting chararacters in s
static int count_lbx_key_value_delimiter_chars(char *s) {
	int i;

	i = 0;
	while (s[i] == '\t') i++;

	return i;
}

// count number of key-value pair delimiting characters in s
static int count_lbx_entry_delimiter_chars(char *s) {
	int i;

	i = 0;
	while ((s[i] == '\r') || (s[i] == '\n')) i++;

	return i;
}

// validate a listbox key-value pair in s
static int validate_lbx_entry(char *s) {
	int key_chars;
	int delimiter_chars;
	int entry_chars;
	int end_chars;
	int total_length;

	// count characters of key-value pair components
	// key
	key_chars = count_lbx_key_chars(s);
	total_length = key_chars;
	// delimiter
	delimiter_chars = count_lbx_key_value_delimiter_chars(s + total_length);
	total_length += delimiter_chars;
	// value
	entry_chars = count_lbx_value_chars(s + total_length);
	total_length += entry_chars;
	// delimiter
	end_chars = count_lbx_entry_delimiter_chars(s + total_length);
	total_length += end_chars;

	// check if all parts of the entry are accounted for
	// (if last entry, we do not require final delimiter)
	if ((key_chars > 0)
		&& (delimiter_chars > 0)
		&& (entry_chars > 0)
		&& ((end_chars > 0) || (s[total_length] == 0))) {
		return total_length;
	}
	else {
		return 0;
	}
}

// validate a list of listbox key-value items
static int validate_lbx_items(char *s) {
	int total_length;
	int entry_length;
	int n;

	total_length = 0;
	n = 0;
	do {
		entry_length = validate_lbx_entry(s + total_length);
		total_length += entry_length;
		if (entry_length > 0) n++;
	} while (entry_length > 0);

	// have reached end of string?
	if (s[total_length] == 0) {
		return n;
	}
	else {
		// invalid
		return -1;
	}
}

// advance to next listbox key-value pair
static char *lbx_advance_entry(char *s) {
	s += count_lbx_key_chars(s);
	s += count_lbx_key_value_delimiter_chars(s);
	s += count_lbx_value_chars(s);
	s += count_lbx_entry_delimiter_chars(s);
	return s;
}

// copy listbox key text from s to buffer of length max_len
static void lbx_get_key_text(char *s, char *buffer, int max_len) {
	int i;
	int n;

	n = count_lbx_key_chars(s);
	if (n > (max_len - 1)) n = max_len - 1;

	for (i = 0; i < n; i++) {
		buffer[i] = s[i];
	}
	buffer[i] = 0;
}

// copy listbox value text from s to buffer of length max_len
static void lbx_get_value_text(char *s, char *buffer, int max_len) {
	int i;
	int n;

	n = count_lbx_value_chars(s);
	if (n > (max_len - 1)) n = max_len - 1;

	for (i = 0; i < n; i++) {
		buffer[i] = s[i];
	}
	buffer[i] = 0;
}

// set listbox items
static int lbx_set_items(lbx_control_t *lc, char *items) {

	int n;

	n = validate_lbx_items(items);

	if (n >= 0) {
		lc->items = items;
		lc->n_items = n;
		return 1;
	}

	lc->items = NULL;
	lc->n_items = 0;
	return 0;
}

#define LBX_PAD 5 // padding between listbox items
#define LBX_ITEM_HEIGHT (2*LBX_PAD + FONT_HEIGHT) // listbox item height

// restrict listbox y coordinate to valid range
static int lbx_limit_y(lbx_control_t *lc, int y) {
	if (y < lc->y) return lc->y;
	if (y >= (lc->y + lc->height)) return (lc->y + lc->height);
	return y;
}

#define LBX_BUF_LEN 1024

// render listbox
static void lbx_render(control_t *c) {
	lbx_control_t *lc;
	int i,j,k;
	int y;
	int y_stop;
	int y1, y2, y3, h, offset, dy;
	char *s;
	int n;
	unsigned char alpha[FONT_HEIGHT];
	char buf[LBX_BUF_LEN];
	int selected;
	int master_alpha;

	lc = (lbx_control_t*)c;

	master_alpha = lc->alpha;
	i = lc->origin / LBX_ITEM_HEIGHT - 1;
	y_stop = lc->y + lc->height;
	n = lc->n_items;
	y = lc->y - lc->origin % LBX_ITEM_HEIGHT;
	s = lc->items;
	for (j = 0; j < i; j++) s = lbx_advance_entry(s);
	while (y < y_stop) {

		// top pad
		y1 = lbx_limit_y(lc, y);
		y2 = lbx_limit_y(lc, y + LBX_PAD);
		h = y2 - y1;
		if (h > 0) {
			draw_background(lc->x, y1, lc->width, h);
		}

		// text
		y1 = y + LBX_PAD;
		y3 = lbx_limit_y(lc, y1 + FONT_HEIGHT);
		h = y3 - y2;
		if (h > 0) {
			if ((i >= 0) && (i < n)) {
				offset = y2 - y1;
				lbx_get_key_text(s, buf, LBX_BUF_LEN);
				for (j = y2, k = 0; j < y3; j++, k++) {
					dy = j - lc->y;
					// make an alpha gradient if close to top or bottom
					if (dy < 32) {
						alpha[k] = dy * 8;
					}
					else {
						dy = y_stop - j;
						if (dy < 32) {
							alpha[k] = dy * 8;
						}
						else {
							alpha[k] = 0xFF;
						}
					}
					alpha[k] = (alpha[k] * master_alpha) >> 8;
				}
				selected = (i == lc->selected_item);
				draw_text(buf, lc->x, y2, offset, lc->width, h, selected, alpha, 1);
			}
			else {
				draw_background(lc->x, y2, lc->width, h);
			}
		}

		// bottom pad
		y1 = y3;
		y2 = lbx_limit_y(lc, y + LBX_ITEM_HEIGHT);
		h = y2 - y1;
		if (h > 0) {
			draw_background(lc->x, y1, lc->width, h);
		}

		y += LBX_ITEM_HEIGHT;
		if (i >= 0) s = lbx_advance_entry(s);
		i++;
		
	}

}

// let control c capture future mouse input
static void view_capture(view_t *v, control_t *c) {
	v->capturing_control = c;
}

// button mouse down callback
static void btn_mouse_down(view_t *v, control_t *c, int x, int y) {
	btn_control_t *b;

	b = (btn_control_t*)c;
	// paint button inverted
	if (!b->inverted) {
		b->inverted = 1;
		b->render(c);
	}
	// capture mouse input
	v->capturing_control = c;
}

// button mouse up callback
static void btn_mouse_up(view_t *v, control_t *c, int x, int y) {
	btn_control_t *b;

	// release capture if held
	if (v->capturing_control == c) {
		v->capturing_control = NULL;
		b = (btn_control_t*)c;
		// if button is inverted, this means that mouse was released
		// on button, and click event should be triggered
		if (b->inverted) {
			b->inverted = 0;
			b->render(c);
			if (b->click) {
				b->click(b);
			}
		}
	}
}

// button mouse move callback
static void btn_mouse_move(view_t *v, control_t *c, int x, int y) {
	btn_control_t *b;
	int inside;
	int dx, dy;

	// if button is capturing, it means that we received a mouse
	// down event previously
	if (v->capturing_control == c) {
		b = (btn_control_t*)c;
		dx = x - b->x;
		dy = y - b->y;
		inside = (dx >= 0) && (dy >= 0) && (dx < b->width) && (dy < b->height);
		// check if mouse has now entered or exited control and adjust
		// inverted state accordingly
		if (inside != b->inverted) {
			b->inverted = inside;
			b->render(c);
		}
	}
}

// list box mouse down callback
static void lbx_mouse_down(view_t *v, control_t *c, int x, int y) {
	lbx_control_t *b;
	int selected_y, selected_item;

	b = (lbx_control_t*)c;
	// check which list box item that was selected
	b->mouse_down_y = y;
	b->mouse_down_origin = b->origin;
	v->capturing_control = c;
	selected_y = (y - b->y - LBX_ITEM_HEIGHT + b->origin);
	selected_item = selected_y / LBX_ITEM_HEIGHT;
	if ((selected_y < 0) || (selected_item >= b->n_items)) {
		b->selected_item = -1;
	}
	else {
		// select item and start long press timer
		b->selected_item = selected_item;
		b->long_press_timer = LONG_PRESS_TICKS;
	}
	b->render(c);
}

// listbox mouse up callback
static void lbx_mouse_up(view_t *v, control_t *c, int x, int y) {
	lbx_control_t *b;

	b = (lbx_control_t*)c;
	// if mouse is released from selected item
	// a click event should be generated
	if (v->capturing_control == c) {
		v->capturing_control = NULL;
		b = (lbx_control_t*)c;
		if (b->selected_item != -1) {
			if (b->click) {
				b->click(b);
			}
		}
		b->selected_item = -1;
		b->long_press_timer = 0;
		b->render(c);
	}
}

#define ABS(x) (((x) > 0) ? (x) : (-(x)))

// listbox mouse move callback
static void lbx_mouse_move(view_t *v, control_t *c, int x, int y) {
	lbx_control_t *b;
	int dy;
	int origin;
	int max_origin;

	if (v->capturing_control == c) {
		b = (lbx_control_t*)c;
		dy = y - b->mouse_down_y;
		// if mouse has moved too much from mouse down,
		// the selected item is deselected
		if (ABS(dy) > (LBX_ITEM_HEIGHT / 2)) {
			b->selected_item = -1;
		}
		// recompute origin to scroll the list accordingly
		if (dy != 0) {
			origin = b->mouse_down_origin - dy;
			max_origin = (b->n_items - (b->height / LBX_ITEM_HEIGHT - 2)) * LBX_ITEM_HEIGHT;
			if (origin >  max_origin) origin = max_origin;
			if (origin < 0) origin = 0;
			b->origin = origin;
			b->render(c);
		}
	}
}

// listbox timer tick callback
static void lbx_timer_tick(lbx_control_t *b) {
	// if timer runs out before item is released,
	// a long press event is generated
	if ((b->selected_item != -1) && (b->long_press_timer > 0)) {
		b->long_press_timer--;
		if (b->long_press_timer == 0) {
			if (b->long_press) {
				b->long_press(b);
			}
			b->selected_item = -1;
			b->render((control_t*)b);
		}
	}
}

// listbox long press callback
static void lbx_long_press(lbx_control_t *b) {
	int i;
	char *s;
	char buf[LBX_BUF_LEN];
	
	s = b->items;
	i = b->selected_item;
	
	// advance to selected item
	while (i--) s = lbx_advance_entry(s);
	s += count_lbx_key_chars(s);
	s += count_lbx_key_value_delimiter_chars(s);
	// copy value to buffer
	lbx_get_value_text(s, buf, LBX_BUF_LEN);
	// call external callback
	lcd_item_long_press(buf);
}

// logo view definition
#define LOGO_Y1 (95)
#define LOGO_Y2 (LOGO_Y1 + LOGO_HEIGHT)
static bg_control_t logo_bg_1 = {bg_render, NULL, NULL, NULL, 0, 0, LCD_WIDTH, LOGO_Y1};
static bmp_control_t logo_logo = {bmp_render, NULL, NULL, NULL, 0, LOGO_Y1, LCD_WIDTH, LOGO_HEIGHT, (bmp_t*)&logo, 0x00};
static bg_control_t logo_bg_2 = {bg_render, NULL, NULL, NULL, 0, LOGO_Y2, LCD_WIDTH, LCD_HEIGHT - LOGO_Y2};

void logo_timer_tick(view_t *);
void logo_start_command(int command);
static view_t logo_view = {
	view_capture,
	logo_timer_tick,
	logo_start_command,
	NULL,
	{(control_t*)&logo_bg_1, 
	(control_t*)&logo_logo, 
	(control_t*)&logo_bg_2,
	NULL}};

// calibration view definition
#define CALIB_BUTTON_SIZE 20
#define CALIB_X_0 (CALIB_POINTS_X1 - CALIB_BUTTON_SIZE/2)
#define CALIB_X_1 (CALIB_X_0 + CALIB_BUTTON_SIZE)
#define CALIB_X_2 (CALIB_POINTS_X0 - CALIB_BUTTON_SIZE/2)
#define CALIB_X_3 (CALIB_X_2 + CALIB_BUTTON_SIZE)
#define CALIB_X_4 (CALIB_POINTS_X2 - CALIB_BUTTON_SIZE/2)
#define CALIB_X_5 (CALIB_X_4 + CALIB_BUTTON_SIZE)
#define CALIB_Y_0 (CALIB_POINTS_Y1 - CALIB_BUTTON_SIZE/2)
#define CALIB_Y_1 (CALIB_Y_0 + CALIB_BUTTON_SIZE)
#define CALIB_Y_2 100
#define CALIB_Y_3 (CALIB_Y_2 + FONT_HEIGHT)
#define CALIB_Y_4 (CALIB_POINTS_Y0 - CALIB_BUTTON_SIZE/2)
#define CALIB_Y_5 (CALIB_Y_4 + CALIB_BUTTON_SIZE)
#define CALIB_Y_6 (CALIB_POINTS_Y2 - CALIB_BUTTON_SIZE/2)
#define CALIB_Y_7 (CALIB_Y_6 + CALIB_BUTTON_SIZE)
static bg_control_t calib_bg_1 = {bg_render, NULL, NULL, NULL, 0, 0, LCD_WIDTH, CALIB_Y_0};
static bg_control_t calib_bg_2 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_0, CALIB_X_0, CALIB_BUTTON_SIZE};
static btn_control_t calib_btn_1 = {btn_render, NULL, NULL, NULL, CALIB_X_0, CALIB_Y_0, CALIB_BUTTON_SIZE, CALIB_BUTTON_SIZE, "", 0, 0x00, NULL};
static bg_control_t calib_bg_3 = {bg_render, NULL, NULL, NULL, CALIB_X_1, CALIB_Y_0, CALIB_X_4-CALIB_X_1, CALIB_BUTTON_SIZE};
static btn_control_t calib_btn_2 = {btn_render, NULL, NULL, NULL, CALIB_X_4, CALIB_Y_0, CALIB_BUTTON_SIZE, CALIB_BUTTON_SIZE, "", 0, 0x00, NULL};
static bg_control_t calib_bg_4 = {bg_render, NULL, NULL, NULL, CALIB_X_5, CALIB_Y_0, LCD_WIDTH-CALIB_X_5, CALIB_BUTTON_SIZE};
static bg_control_t calib_bg_5 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_1, LCD_WIDTH, CALIB_Y_2-CALIB_Y_1};
static lbl_control_t calib_lbl = {lbl_render, NULL, NULL, NULL, 0, CALIB_Y_2, LCD_WIDTH, FONT_HEIGHT, "Tap markers to calibrate screen", 0x00};
static bg_control_t calib_bg_6 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_3, LCD_WIDTH, CALIB_Y_4-CALIB_Y_3};
static bg_control_t calib_bg_7 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_4, CALIB_X_2, CALIB_BUTTON_SIZE};
static btn_control_t calib_btn_5 = {btn_render, NULL, NULL, NULL, CALIB_X_2, CALIB_Y_4, CALIB_BUTTON_SIZE, CALIB_BUTTON_SIZE, "", 0, 0x00, NULL};
static bg_control_t calib_bg_8 = {bg_render, NULL, NULL, NULL, CALIB_X_3, CALIB_Y_4, LCD_WIDTH-CALIB_X_3, CALIB_BUTTON_SIZE};
static bg_control_t calib_bg_9 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_5, LCD_WIDTH, CALIB_Y_6-CALIB_Y_5};
static bg_control_t calib_bg_10 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_6, CALIB_X_0, CALIB_BUTTON_SIZE};
static btn_control_t calib_btn_4 = {btn_render, NULL, NULL, NULL, CALIB_X_0, CALIB_Y_6, CALIB_BUTTON_SIZE, CALIB_BUTTON_SIZE, "", 0, 0x00, NULL};
static bg_control_t calib_bg_11 = {bg_render, NULL, NULL, NULL, CALIB_X_1, CALIB_Y_6, CALIB_X_4-CALIB_X_1, CALIB_BUTTON_SIZE};
static btn_control_t calib_btn_3 = {btn_render, NULL, NULL, NULL, CALIB_X_4, CALIB_Y_6, CALIB_BUTTON_SIZE, CALIB_BUTTON_SIZE, "", 0, 0x00, NULL};
static bg_control_t calib_bg_12 = {bg_render, NULL, NULL, NULL, CALIB_X_5, CALIB_Y_6, LCD_WIDTH-CALIB_X_5, CALIB_BUTTON_SIZE};
static bg_control_t calib_bg_13 = {bg_render, NULL, NULL, NULL, 0, CALIB_Y_7, LCD_WIDTH, LCD_HEIGHT-CALIB_Y_7};

void calib_timer_tick(view_t *);
void calib_start_command(int command);
static int current_command = 0;
static int animation_timer = 0;
static view_t calib_view = {
	view_capture,
	calib_timer_tick,
	calib_start_command,
	NULL,
	{(control_t*)&calib_bg_1, 
	(control_t*)&calib_bg_2, 
	(control_t*)&calib_btn_1,
	(control_t*)&calib_bg_3,
	(control_t*)&calib_btn_2,
	(control_t*)&calib_bg_4,
	(control_t*)&calib_bg_5,
	(control_t*)&calib_lbl,
	(control_t*)&calib_bg_6,
	(control_t*)&calib_bg_7,
	(control_t*)&calib_btn_5,
	(control_t*)&calib_bg_8,
	(control_t*)&calib_bg_9,
	(control_t*)&calib_bg_10,
	(control_t*)&calib_btn_4,
	(control_t*)&calib_bg_11,
	(control_t*)&calib_btn_3,
	(control_t*)&calib_bg_12,
	(control_t*)&calib_bg_13,
	NULL}};

// title view definition
static bg_control_t title_bg_1 = {bg_render, NULL, NULL, NULL, 0, 0, 240, 100};
static lbl_control_t title_lbl = {lbl_render, NULL, NULL, NULL, 0, 100, LCD_WIDTH, 20, NULL, 0x00};
static bg_control_t title_bg_2 = {bg_render, NULL, NULL, NULL, 0, 120, 240, LCD_HEIGHT - 120};

void title_timer_tick(view_t *);
void title_start_command(int command);
static view_t title_view = {
	view_capture,
	title_timer_tick,
	title_start_command,
	NULL,
	{(control_t*)&title_bg_1, 
	(control_t*)&title_lbl,
	(control_t*)&title_bg_2,
	NULL}};

static char const *passphrase_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static int const PASSPHRASE_CHARSET_LEN = 62;

// passphrase view definition
static int passphrase_maxlen = 0;
static int passphrase_len = 0;
static int passphrase_index = 0;
static void pass_back_click(btn_control_t *b);
static void pass_up_click(btn_control_t *b);
static void pass_down_click(btn_control_t *b);
static void pass_fwd_click(btn_control_t *b);
static void pass_ok_click(btn_control_t *b);
#define PASS_BTN_HEIGHT 45
#define PASS_BTN_WIDTH 60
#define PASS_BTN_PAD 5
#define PASS_BTN_CENTER_ROW 170
#define PASS_ROW_1 (PASS_BTN_CENTER_ROW - PASS_BTN_HEIGHT - PASS_BTN_PAD/2)
#define PASS_ROW_2 (PASS_BTN_CENTER_ROW - PASS_BTN_HEIGHT/2)
#define PASS_ROW_3 (PASS_ROW_1 + PASS_BTN_HEIGHT + PASS_BTN_PAD)
#define PASS_ROW_4 (PASS_ROW_3 + PASS_BTN_HEIGHT + PASS_BTN_PAD)
#define PASS_COL_2 ((LCD_WIDTH - PASS_BTN_WIDTH)/2)
#define PASS_COL_1 (PASS_COL_2 - PASS_BTN_PAD - PASS_BTN_WIDTH)
#define PASS_COL_3 (PASS_COL_2 + PASS_BTN_PAD + PASS_BTN_WIDTH)
static bg_control_t pass_bg_1 = {bg_render, NULL, NULL, NULL, 0, 0, LCD_WIDTH, 20};
static lbl_control_t pass_lbl = {lbl_render, NULL, NULL, NULL, 0, 20, LCD_WIDTH, 20, "Type passphrase to unlock", 0x00};
static bg_control_t pass_bg_2 = {bg_render, NULL, NULL, NULL, 0, 40, LCD_WIDTH, 5};
static lbl_control_t pass_passphrase = {lbl_render, NULL, NULL, NULL, 0, 45, LCD_WIDTH, 20, "", 0x00};
static bg_control_t pass_bg_3 = {bg_render, NULL, NULL, NULL, 0, 65, LCD_WIDTH, PASS_ROW_1 - 65};
static gbtn_control_t pass_btn_up = {gbtn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PASS_COL_2, PASS_ROW_1, PASS_BTN_WIDTH, PASS_BTN_HEIGHT, (bmp_t*)&up_arrow, 0, 0x00, pass_up_click};
static gbtn_control_t pass_btn_down = {gbtn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PASS_COL_2, PASS_ROW_3, PASS_BTN_WIDTH, PASS_BTN_HEIGHT, (bmp_t*)&down_arrow, 0, 0x00, pass_down_click};
static bg_control_t pass_bg_4 = {bg_render, NULL, NULL, NULL, PASS_COL_2 - PASS_BTN_PAD, PASS_ROW_2, PASS_BTN_PAD, PASS_BTN_HEIGHT};
static bg_control_t pass_bg_5 = {bg_render, NULL, NULL, NULL, PASS_COL_3 - PASS_BTN_PAD, PASS_ROW_1, PASS_BTN_PAD, PASS_ROW_3 + PASS_BTN_HEIGHT - PASS_ROW_1};
static bg_control_t pass_bg_6 = {bg_render, NULL, NULL, NULL, PASS_COL_2, PASS_ROW_3 - PASS_BTN_PAD, PASS_BTN_WIDTH, PASS_BTN_PAD};
static bg_control_t pass_bg_7 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_1, PASS_COL_2, PASS_ROW_2 - PASS_ROW_1};
static bg_control_t pass_bg_8 = {bg_render, NULL, NULL, NULL, PASS_COL_2 + PASS_BTN_WIDTH, PASS_ROW_1, LCD_WIDTH - PASS_COL_2 - PASS_BTN_WIDTH, PASS_ROW_2 - PASS_ROW_1};
static bg_control_t pass_bg_9 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_2, PASS_COL_1, PASS_BTN_HEIGHT};
static bg_control_t pass_bg_10 = {bg_render, NULL, NULL, NULL, PASS_COL_3 + PASS_BTN_WIDTH, PASS_ROW_2, LCD_WIDTH - PASS_COL_3 - PASS_BTN_WIDTH, PASS_BTN_HEIGHT};
static bg_control_t pass_bg_11 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_2 + PASS_BTN_HEIGHT, PASS_COL_2, PASS_ROW_3 + PASS_BTN_HEIGHT - PASS_ROW_2 - PASS_BTN_HEIGHT};
static bg_control_t pass_bg_12 = {bg_render, NULL, NULL, NULL, PASS_COL_2 + PASS_BTN_WIDTH, PASS_ROW_2 + PASS_BTN_HEIGHT, LCD_WIDTH - PASS_COL_2 - PASS_BTN_WIDTH, PASS_ROW_3 + PASS_BTN_HEIGHT - PASS_ROW_2 - PASS_BTN_HEIGHT};
static gbtn_control_t pass_btn_back = {gbtn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PASS_COL_1, PASS_ROW_2, PASS_BTN_WIDTH, PASS_BTN_HEIGHT, (bmp_t*)&left_arrow, 0, 0x00, pass_back_click};
static gbtn_control_t pass_btn_fwd = {gbtn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PASS_COL_3, PASS_ROW_2, PASS_BTN_WIDTH, PASS_BTN_HEIGHT, (bmp_t*)&right_arrow, 0, 0x00, pass_fwd_click};
static btn_control_t pass_btn_ok = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PASS_COL_3, PASS_ROW_4, PASS_BTN_WIDTH, PASS_BTN_HEIGHT, "OK", 0, 0x00, pass_ok_click};
static bg_control_t pass_bg_13 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_3 + PASS_BTN_HEIGHT, LCD_WIDTH, PASS_BTN_PAD};
static bg_control_t pass_bg_14 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_4, PASS_COL_3, PASS_BTN_HEIGHT};
static bg_control_t pass_bg_15 = {bg_render, NULL, NULL, NULL, PASS_COL_3 + PASS_BTN_WIDTH, PASS_ROW_4, LCD_WIDTH - PASS_COL_3 - PASS_BTN_WIDTH, PASS_BTN_HEIGHT};
static bg_control_t pass_bg_16 = {bg_render, NULL, NULL, NULL, 0, PASS_ROW_4 + PASS_BTN_HEIGHT, LCD_WIDTH, LCD_HEIGHT - PASS_ROW_4 - PASS_BTN_HEIGHT};

void pass_timer_tick(view_t *);
void pass_start_command(int command);
static view_t pass_view = {
	view_capture,
	pass_timer_tick,
	pass_start_command,
	NULL,
	{(control_t*)&pass_bg_1,
	(control_t*)&pass_lbl,
	(control_t*)&pass_bg_2,
	(control_t*)&pass_passphrase,
	(control_t*)&pass_bg_3,
	(control_t*)&pass_btn_up,
	(control_t*)&pass_btn_down,
	(control_t*)&pass_bg_4,
	(control_t*)&pass_bg_5,
	(control_t*)&pass_bg_6,
	(control_t*)&pass_bg_7,
	(control_t*)&pass_bg_8,
	(control_t*)&pass_bg_9,
	(control_t*)&pass_bg_10,
	(control_t*)&pass_bg_11,
	(control_t*)&pass_bg_12,
	(control_t*)&pass_btn_back,
	(control_t*)&pass_btn_fwd,
	(control_t*)&pass_bg_13,
	(control_t*)&pass_btn_ok,
	(control_t*)&pass_bg_14,
	(control_t*)&pass_bg_15,
	(control_t*)&pass_bg_16,
	NULL}};

// PIN view definition
#define PIN_BTN_SIZE 50
#define PIN_BTN_SPACE 5

#define PIN_ROW_1 90
#define PIN_ROW_2 (PIN_ROW_1 + PIN_BTN_SIZE + PIN_BTN_SPACE)
#define PIN_ROW_3 (PIN_ROW_2 + PIN_BTN_SIZE + PIN_BTN_SPACE)
#define PIN_ROW_4 (PIN_ROW_3 + PIN_BTN_SIZE + PIN_BTN_SPACE)
#define PIN_COL_1 (LCD_WIDTH / 2 - (PIN_BTN_SIZE * 3)/2 - PIN_BTN_SPACE)
#define PIN_COL_2 (PIN_COL_1 + PIN_BTN_SIZE + PIN_BTN_SPACE)
#define PIN_COL_3 (PIN_COL_2 + PIN_BTN_SIZE + PIN_BTN_SPACE)

static int pin_maxlen = 0;
static int pin_len = 0;
static void pin_btn_click(btn_control_t *b);
static void pin_back_click(btn_control_t *b);
static void pin_ok_click(btn_control_t *b);
static bg_control_t pin_bg_2 = {bg_render, NULL, NULL, NULL, 0, 0, LCD_WIDTH, 20};
static lbl_control_t pin_lbl = {lbl_render, NULL, NULL, NULL, 0, 20, LCD_WIDTH, FONT_HEIGHT, "Please type PIN", 0x00};
static bg_control_t pin_bg_3 = {bg_render, NULL, NULL, NULL, 0, 40, LCD_WIDTH, 5};
static lbl_control_t pin_code = {lbl_render, NULL, NULL, NULL, 0, 45, LCD_WIDTH, FONT_HEIGHT, "", 0x00};
static bg_control_t pin_bg_4 = {bg_render, NULL, NULL, NULL, 0, 65, LCD_WIDTH, PIN_ROW_1 - 65};
static btn_control_t pin_btn_1 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_1, PIN_ROW_1, PIN_BTN_SIZE, PIN_BTN_SIZE, "1", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_2 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_2, PIN_ROW_1, PIN_BTN_SIZE, PIN_BTN_SIZE, "2", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_3 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_3, PIN_ROW_1, PIN_BTN_SIZE, PIN_BTN_SIZE, "3", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_4 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_1, PIN_ROW_2, PIN_BTN_SIZE, PIN_BTN_SIZE, "4", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_5 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_2, PIN_ROW_2, PIN_BTN_SIZE, PIN_BTN_SIZE, "5", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_6 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_3, PIN_ROW_2, PIN_BTN_SIZE, PIN_BTN_SIZE, "6", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_7 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_1, PIN_ROW_3, PIN_BTN_SIZE, PIN_BTN_SIZE, "7", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_8 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_2, PIN_ROW_3, PIN_BTN_SIZE, PIN_BTN_SIZE, "8", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_9 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_3, PIN_ROW_3, PIN_BTN_SIZE, PIN_BTN_SIZE, "9", 0, 0x00, pin_btn_click};
static gbtn_control_t pin_btn_back = {gbtn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_1, PIN_ROW_4, PIN_BTN_SIZE, PIN_BTN_SIZE, (bmp_t*)&left_arrow, 0, 0x00, pin_back_click};
static btn_control_t pin_btn_0 = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_2, PIN_ROW_4, PIN_BTN_SIZE, PIN_BTN_SIZE, "0", 0, 0x00, pin_btn_click};
static btn_control_t pin_btn_ok = {btn_render, btn_mouse_down, btn_mouse_up, btn_mouse_move, PIN_COL_3, PIN_ROW_4, PIN_BTN_SIZE, PIN_BTN_SIZE, "OK", 0, 0x00, pin_ok_click};
static bg_control_t pin_bg_5 = {bg_render, NULL, NULL, NULL, 0, PIN_ROW_1, PIN_COL_1, LCD_HEIGHT-PIN_ROW_1};
static bg_control_t pin_bg_6 = {bg_render, NULL, NULL, NULL, PIN_COL_3 + PIN_BTN_SIZE, PIN_ROW_1, LCD_WIDTH - (PIN_COL_3 + PIN_BTN_SIZE), LCD_HEIGHT-PIN_ROW_1};
static bg_control_t pin_bg_7 = {bg_render, NULL, NULL, NULL, PIN_COL_1, PIN_ROW_4 + PIN_BTN_SIZE, PIN_BTN_SIZE * 3 + PIN_BTN_SPACE * 2, LCD_HEIGHT-(PIN_ROW_4 + PIN_BTN_SIZE)};
static bg_control_t pin_bg_8 = {bg_render, NULL, NULL, NULL, PIN_COL_1, PIN_ROW_1 + PIN_BTN_SIZE, PIN_BTN_SIZE * 3 + PIN_BTN_SPACE * 2, PIN_BTN_SPACE};
static bg_control_t pin_bg_9 = {bg_render, NULL, NULL, NULL, PIN_COL_1, PIN_ROW_2 + PIN_BTN_SIZE, PIN_BTN_SIZE * 3 + PIN_BTN_SPACE * 2, PIN_BTN_SPACE};
static bg_control_t pin_bg_10 = {bg_render, NULL, NULL, NULL, PIN_COL_1, PIN_ROW_3 + PIN_BTN_SIZE, PIN_BTN_SIZE * 3 + PIN_BTN_SPACE * 2, PIN_BTN_SPACE};
static bg_control_t pin_bg_11 = {bg_render, NULL, NULL, NULL, PIN_COL_1 + PIN_BTN_SIZE, PIN_ROW_1, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_12 = {bg_render, NULL, NULL, NULL, PIN_COL_1 + PIN_BTN_SIZE, PIN_ROW_2, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_13 = {bg_render, NULL, NULL, NULL, PIN_COL_1 + PIN_BTN_SIZE, PIN_ROW_3, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_14 = {bg_render, NULL, NULL, NULL, PIN_COL_1 + PIN_BTN_SIZE, PIN_ROW_4, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_15 = {bg_render, NULL, NULL, NULL, PIN_COL_2 + PIN_BTN_SIZE, PIN_ROW_1, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_16 = {bg_render, NULL, NULL, NULL, PIN_COL_2 + PIN_BTN_SIZE, PIN_ROW_2, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_17 = {bg_render, NULL, NULL, NULL, PIN_COL_2 + PIN_BTN_SIZE, PIN_ROW_3, PIN_BTN_SPACE, PIN_BTN_SIZE};
static bg_control_t pin_bg_18 = {bg_render, NULL, NULL, NULL, PIN_COL_2 + PIN_BTN_SIZE, PIN_ROW_4, PIN_BTN_SPACE, PIN_BTN_SIZE};

void pin_timer_tick(view_t *);
void pin_start_command(int command);
static view_t pin_view = {
	view_capture,
	pin_timer_tick,
	pin_start_command,
	NULL,
	{(control_t*)&pin_bg_2,
	(control_t*)&pin_lbl,
	(control_t*)&pin_bg_3,
	(control_t*)&pin_code,
	(control_t*)&pin_bg_4,
	(control_t*)&pin_btn_1,
	(control_t*)&pin_btn_2, 
	(control_t*)&pin_btn_3,
	(control_t*)&pin_btn_4,
	(control_t*)&pin_btn_5, 
	(control_t*)&pin_btn_6,
	(control_t*)&pin_btn_7,
	(control_t*)&pin_btn_8, 
	(control_t*)&pin_btn_9,
	(control_t*)&pin_btn_back,
	(control_t*)&pin_btn_0, 
	(control_t*)&pin_btn_ok,
	(control_t*)&pin_bg_5,
	(control_t*)&pin_bg_6,
	(control_t*)&pin_bg_7,
	(control_t*)&pin_bg_8,
	(control_t*)&pin_bg_9,
	(control_t*)&pin_bg_10,
	(control_t*)&pin_bg_11,
	(control_t*)&pin_bg_12,
	(control_t*)&pin_bg_13,
	(control_t*)&pin_bg_14,
	(control_t*)&pin_bg_15,
	(control_t*)&pin_bg_16,
	(control_t*)&pin_bg_17,
	(control_t*)&pin_bg_18,
	NULL}};

// key view definitions
static bg_control_t key_bg_1 = {bg_render, NULL, NULL, NULL, 0, 0, 240, 10};
static bmp_control_t key_logo = {bmp_render, NULL, NULL, NULL, 0, 10, LCD_WIDTH, LOGO_HEIGHT, (bmp_t*)&logo, 0x00};
static bg_control_t key_bg_2 = {bg_render, NULL, NULL, NULL, 0, 40, 240, 10};
static lbx_control_t key_lbx = {lbx_render, lbx_mouse_down, lbx_mouse_up, lbx_mouse_move, 0, 50, LCD_WIDTH, LCD_HEIGHT-50, NULL, 0, 0, 0, 0, -1, lbx_timer_tick, NULL, lbx_long_press, 0, 0x00};

void key_timer_tick(view_t *v);
void key_start_command(int command);
static view_t key_view = {
	view_capture,
	key_timer_tick,
	key_start_command,
	NULL,
	{(control_t*)&key_bg_1, 
	(control_t*)&key_logo, 
	(control_t*)&key_bg_2,
	(control_t*)&key_lbx,
	NULL}};
	
void lcd_set_title_msg(char const *msg) {
	title_lbl.caption = (char*)msg;
}

void lcd_set_passphrase_msg(char const *msg) {
	pass_lbl.caption = (char*)msg;
	pass_lbl.render((control_t*)&pass_lbl);
}

void lcd_set_passphrase_buffer(char *buf, int buflen) {
	int i;

	if (buflen > 1) {
		for (i = 1; i < buflen; i++) buf[i] = 0;
		buf[0] = passphrase_charset[0];
		pass_passphrase.caption = buf;
		passphrase_maxlen = buflen;
		passphrase_len = 1;
		passphrase_index = 0;
	}
}

void lcd_set_pin_msg(char const *msg) {
	pin_lbl.caption = (char*)msg;
	pin_lbl.render((control_t*)&pin_lbl);
}

void lcd_set_pin_buffer(char *buf, int buflen) {
	int i;

	if (buflen > 1) {
		for (i = 0; i < buflen; i++) buf[i] = 0;
		pin_code.caption = buf;
		pin_maxlen = buflen;
		pin_len = 0;
	}
}

// passphrase screen back button click callback
static void pass_back_click(btn_control_t *b) {
	int i;
	char *s;

	s = (char*)pass_passphrase.caption;
	if (passphrase_len > 1) {
		// erase last character
		s[passphrase_len-1] = 0;
		passphrase_len--;
		// search for index of last character, so that new
		// characters are duplicates of this
		for (i = 0; i < PASSPHRASE_CHARSET_LEN; i++) {
			if (passphrase_charset[i] == s[passphrase_len-1]) {
				passphrase_index = i;
				break;
			}
		}
		pass_passphrase.render((control_t*)&pass_passphrase);
	}
}

// passphrase screen up button click callback
static void pass_up_click(btn_control_t *b) {
	char *s;

	s = (char*)pass_passphrase.caption;
	
	// step to next character
	passphrase_index++;
	if (passphrase_index >= PASSPHRASE_CHARSET_LEN) {
		// roll over
		passphrase_index = 0;
	}
	s[passphrase_len-1] = passphrase_charset[passphrase_index];
	pass_passphrase.render((control_t*)&pass_passphrase);
}

// passphrase screen down button click callback
static void pass_down_click(btn_control_t *b) {
	char *s;

	s = (char*)pass_passphrase.caption;
	
	// step to previous character
	passphrase_index--;
	if (passphrase_index < 0) {
		// roll over
		passphrase_index = PASSPHRASE_CHARSET_LEN - 1;
	}
	s[passphrase_len-1] = passphrase_charset[passphrase_index];
	pass_passphrase.render((control_t*)&pass_passphrase);
}

// passphrase screen forward button click callback
static void pass_fwd_click(btn_control_t *b) {
	char *s;

	s = (char*)pass_passphrase.caption;
	
	// append new character if there is room
	if (passphrase_len < passphrase_maxlen - 1) {
		passphrase_len++;
		s[passphrase_len-1] = passphrase_charset[passphrase_index]; 
		pass_passphrase.render((control_t*)&pass_passphrase);
	}
}

// passphrase screen ok button click callback
static void pass_ok_click(btn_control_t *b) {
	lcd_passphrase_ok();
}

// common PIN digit button click callback
static void pin_btn_click(btn_control_t *b) {
	char *s;

	s = (char*)pin_code.caption;

	if (pin_len < pin_maxlen - 1) {
		// take digit from button caption
		s[pin_len] = b->caption[0];
		pin_len++;
		pin_code.render((control_t*)&pin_code);
	}
}

// PIN screen back button click callback
static void pin_back_click(btn_control_t *b) {
	char *s;

	s = (char*)pin_code.caption;

	// remove digit if any
	if (pin_len > 0) {
		pin_len--;
		s[pin_len] = 0;
		pin_code.render((control_t*)&pin_code);
	}
}

// PIN screen ok button click callback
static void pin_ok_click(btn_control_t *b) {

	lcd_pin_ok();
}

// compute alpha for fade-ins based on animation timer
static int fade_in_alpha(void) {
	return (ANIMATION_TICKS - animation_timer) * (0xFF / ANIMATION_TICKS);
}

// compute alpha for fade-outs based on animation timer
static int fade_out_alpha(void) {
	return animation_timer * (0xFF / ANIMATION_TICKS);
}

// logo screen timer tick
void logo_timer_tick(view_t *v) {
	int finished_command;
	int alpha;

	// early exit if no command
	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_FADE_IN:
		alpha = fade_in_alpha();
		break;
	case CMD_FADE_OUT:
		alpha = fade_out_alpha();
		break;
	default:
		return;
		break;
	}

	// redraw with new alpha
	logo_logo.alpha = alpha;
	logo_logo.render((control_t*)&logo_logo);

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start logo screen command
void logo_start_command(int command) {
	switch (command) {
	case CMD_FADE_IN:
	case CMD_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}
}

// calibration screen timer tick
void calib_timer_tick(view_t *v) {
	btn_control_t *btn = NULL;
	lbl_control_t *lbl = NULL;
	int fade_in;
	int finished_command;
	int alpha;
	
	// early exit if no command
	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_CALIB_LBL_FADE_IN:
		lbl = &calib_lbl;
		fade_in = 1;
		break;
	case CMD_CALIB_LBL_FADE_OUT:
		lbl = &calib_lbl;
		fade_in = 0;
		break;
	case CMD_CALIB_POINT_1_FADE_IN:
		btn = &calib_btn_1;
		fade_in = 1;
		break;
	case CMD_CALIB_POINT_1_FADE_OUT:
		btn = &calib_btn_1;
		fade_in = 0;
		break;
	case CMD_CALIB_POINT_2_FADE_IN:
		btn = &calib_btn_2;
		fade_in = 1;
		break;
	case CMD_CALIB_POINT_2_FADE_OUT:
		btn = &calib_btn_2;
		fade_in = 0;
		break;
	case CMD_CALIB_POINT_3_FADE_IN:
		btn = &calib_btn_3;
		fade_in = 1;
		break;
	case CMD_CALIB_POINT_3_FADE_OUT:
		btn = &calib_btn_3;
		fade_in = 0;
		break;
	case CMD_CALIB_POINT_4_FADE_IN:
		btn = &calib_btn_4;
		fade_in = 1;
		break;
	case CMD_CALIB_POINT_4_FADE_OUT:
		btn = &calib_btn_4;
		fade_in = 0;
		break;
	case CMD_CALIB_POINT_5_FADE_IN:
		btn = &calib_btn_5;
		fade_in = 1;
		break;
	case CMD_CALIB_POINT_5_FADE_OUT:
		btn = &calib_btn_5;
		fade_in = 0;
		break;
	default:
		return;
		break;
	}

	// compute new alpha
	alpha = (fade_in) ? fade_in_alpha() : fade_out_alpha();

	// redraw with new alpha
	if (btn) {
		btn->alpha = alpha;
		btn->render((control_t *)btn);
	}

	if (lbl) {
		lbl->alpha = alpha;
		lbl->render((control_t *) lbl);
	}

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start calibration screen command
void calib_start_command(int command) {
	switch (command) {
	case CMD_CALIB_LBL_FADE_IN:
	case CMD_CALIB_LBL_FADE_OUT:
	case CMD_CALIB_POINT_1_FADE_IN:
	case CMD_CALIB_POINT_1_FADE_OUT:
	case CMD_CALIB_POINT_2_FADE_IN:
	case CMD_CALIB_POINT_2_FADE_OUT:
	case CMD_CALIB_POINT_3_FADE_IN:
	case CMD_CALIB_POINT_3_FADE_OUT:
	case CMD_CALIB_POINT_4_FADE_IN:
	case CMD_CALIB_POINT_4_FADE_OUT:
	case CMD_CALIB_POINT_5_FADE_IN:
	case CMD_CALIB_POINT_5_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}
}

// title screen timer tick
void title_timer_tick(view_t *v) {
	int fade_in;
	int finished_command;
	int alpha;

	// early exit if no command
	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_FADE_IN:
		fade_in = 1;
		break;
	case CMD_FADE_OUT:
		fade_in = 0;
		break;
	default:
		return;
		break;
	}

	// compute alpha
	alpha = (fade_in) ? fade_in_alpha() : fade_out_alpha();

	// redraw with new alpha
	title_lbl.alpha = alpha;
	title_lbl.render((control_t*)&title_lbl);

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start title screen command
void title_start_command(int command) {
	switch (command) {
	case CMD_FADE_IN:
	case CMD_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}
}

// passphrase screen timer tick
void pass_timer_tick(view_t *v) {
	int fade_in;
	int finished_command;
	int alpha;

	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_FADE_IN:
		fade_in = 1;
		break;
	case CMD_FADE_OUT:
		fade_in = 0;
		break;
	default:
		return;
		break;
	}

	// compute new alpha
	alpha = (fade_in) ? fade_in_alpha() : fade_out_alpha();

	// redraw with new alpha
	pass_lbl.alpha =
		pass_passphrase.alpha =
		pass_btn_back.alpha =
		pass_btn_up.alpha =
		pass_btn_down.alpha =
		pass_btn_fwd.alpha =
		pass_btn_ok.alpha = alpha;

	pass_lbl.render((control_t*)&pass_lbl);
	pass_passphrase.render((control_t*)&pass_passphrase);
	pass_btn_back.render((control_t*)&pass_btn_back);
	pass_btn_up.render((control_t*)&pass_btn_up);
	pass_btn_down.render((control_t*)&pass_btn_down);
	pass_btn_fwd.render((control_t*)&pass_btn_fwd);
	pass_btn_ok.render((control_t*)&pass_btn_ok);

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start passphrase screen command
void pass_start_command(int command) {
	switch (command) {
	case CMD_FADE_IN:
	case CMD_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}
}

// PIN screen timer tick
void pin_timer_tick(view_t *v) {
	int fade_in;
	int finished_command;
	int alpha;

	// early exit if no command
	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_FADE_IN:
		fade_in = 1;
		break;
	case CMD_FADE_OUT:
		fade_in = 0;
		break;
	default:
		return;
		break;
	}

	// compute new alpha
	alpha = (fade_in) ? fade_in_alpha() : fade_out_alpha();

	// redreaw with new alpha
	pin_lbl.alpha =
		pin_code.alpha =
		pin_btn_1.alpha =
		pin_btn_2.alpha =
		pin_btn_3.alpha =
		pin_btn_4.alpha =
		pin_btn_5.alpha =
		pin_btn_6.alpha =
		pin_btn_7.alpha =
		pin_btn_8.alpha =
		pin_btn_9.alpha =
		pin_btn_back.alpha =
		pin_btn_0.alpha =
		pin_btn_ok.alpha = alpha;

	pin_lbl.render((control_t*)&pin_lbl);
	pin_code.render((control_t*)&pin_code);
	pin_btn_1.render((control_t*)&pin_btn_1);
	pin_btn_2.render((control_t*)&pin_btn_2); 
	pin_btn_3.render((control_t*)&pin_btn_3);
	pin_btn_4.render((control_t*)&pin_btn_4);
	pin_btn_5.render((control_t*)&pin_btn_5);
	pin_btn_6.render((control_t*)&pin_btn_6);
	pin_btn_7.render((control_t*)&pin_btn_7);
	pin_btn_8.render((control_t*)&pin_btn_8); 
	pin_btn_9.render((control_t*)&pin_btn_9);
	pin_btn_back.render((control_t*)&pin_btn_back);
	pin_btn_0.render((control_t*)&pin_btn_0); 
	pin_btn_ok.render((control_t*)&pin_btn_ok);

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start PIN screen command
void pin_start_command(int command) {
	switch (command) {
	case CMD_FADE_IN:
	case CMD_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}

}

// key screen timer tick
void key_timer_tick(view_t *v) {

	int fade_in;
	int finished_command;
	int alpha;

	// call listbox timer tick for long press timer
	key_lbx.timer_tick(&key_lbx);

	// early exit if no command
	if (current_command == 0) return;

	animation_timer--;

	switch (current_command) {
	case CMD_FADE_IN:
		fade_in = 1;
		break;
	case CMD_FADE_OUT:
		fade_in = 0;
		break;
	default:
		return;
		break;
	}

	// compute new alpha
	alpha = (fade_in) ? fade_in_alpha() : fade_out_alpha();

	// redraw with new alpha
	key_logo.alpha =
		key_lbx.alpha = alpha;

	key_logo.render((control_t*)&key_logo);
	key_lbx.render((control_t*)&key_lbx);

	if (animation_timer == 0) {
		finished_command = current_command;
		current_command = 0;
		lcd_command_complete(finished_command);
	}
}

// start key screen command
void key_start_command(int command) {
	switch (command) {
	case CMD_FADE_IN:
	case CMD_FADE_OUT:
		current_command = command;
		animation_timer = ANIMATION_TICKS;
		break;
	}

}

// render all controls in view
static void render_view(view_t *v) {
	int i;

	i = 0;
	while (v->controls[i] != NULL) {
		v->controls[i]->render(v->controls[i]);

		i++;
	}
}

// find control containing at x and y coordinates
static control_t *find_control(view_t *v, int x, int y) {

	int i;
	int dx, dy;
	control_t *c;

	i = 0;
	while (v->controls[i]) {
		c = v->controls[i];
		dx = x - c->x;
		dy = y - c->y;
		if ((dx >= 0) && (dy >= 0) && (dx < c->width) && (dy < c->height)) {
			return c;
		}
		i++;
	}
	return NULL;
}

// get the receiver of (mouse) input
static control_t *get_receiver(view_t *v, int x, int y) {

	// if mouse is captured, the capturing control has priority
	if (v->capturing_control) 
	{
		return v->capturing_control;
	}
	// otherwise the receiver is the control at the given coordinates
	else {
		return find_control(v, x, y);
	}
}

// view mouse down event handler
void view_mouse_down(int x, int y) {
	control_t *c;

	// delegate event to receiver if available
	if (current_view) {
		c = get_receiver(current_view, x, y);
		if (c && c->mouse_down) {
			c->mouse_down(current_view, c, x, y);
		}
	}
}

// view mouse up handler
void view_mouse_up(int x, int y) {
	control_t *c;

	// delegate event to receiver if available
	if (current_view) {
		c = get_receiver(current_view, x, y);
		if (c && c->mouse_down) {
			c->mouse_up(current_view, c, x, y);
		}
	}
}

// view mouse move event handler
void view_mouse_move(int x, int y) {
	control_t *c;

	// delegate event to receiver if available
	if (current_view) {
		c = get_receiver(current_view, x, y);
		if (c && c->mouse_down) {
			c->mouse_move(current_view, c, x, y);
		}
	}
}

void lcd_set_view(int view) {
	switch (view) {
	case VIEW_LOGO:
		current_view = &logo_view;
		break;
	case VIEW_CALIB:
		current_view = &calib_view;
		break;
	case VIEW_TITLE:
		current_view = &title_view;
		break;
	case VIEW_PASS:
		current_view = &pass_view;
		break;
	case VIEW_PIN:
		current_view = &pin_view;
		break;
	case VIEW_KEY:
		current_view = &key_view;
		break;
	}
	render_view(current_view);
}

int lcd_get_view(void) {
	if (current_view == &logo_view) return VIEW_LOGO;
	if (current_view == &calib_view) return VIEW_CALIB;
	if (current_view == &title_view) return VIEW_TITLE;
	if (current_view == &pass_view) return VIEW_PASS;
	if (current_view == &pin_view) return VIEW_PIN;
	if (current_view == &key_view) return VIEW_KEY;
	return VIEW_NONE;
}

void lcd_timer_tick(void) {

	if (current_view && current_view->timer_tick) {
		current_view->timer_tick(current_view);
	}

}

void lcd_start_command(int command) {

	if (current_view && current_view->start_command) {
		current_view->start_command(command);
	}
}

int lcd_set_keys(char *keys) {
	return lbx_set_items(&key_lbx, keys);
}

//#include <stdio.h>

/*void convert_logo(void) {
	FILE *f;
	int i;
	int c;
	int v;
	int o;
	int n;
	int s;
	int r;

	o = v = left_arrow.pixel_data[0];
	c = 0;
	n = 0;
	s = 0;
	r = 0;

	f = fopen("r_arrow.c", "w");
	for (i = 0; i < 28*21; i++) {
		v = left_arrow.pixel_data[i*3];
		if (((v == o) || (c == 255)) && (i < 28*21-1)) {
			c++;
		}
		else {
			if (v == o) c++;
			if (c == 1) {
				fprintf(f, "%d, ", (o | 1));
				n++;
				s++;
				r++;
				if (r > 40) {
					fprintf(f, "\n");
					r = 0;
				}
			}
			else {
				fprintf(f, "%d, %d, ", (o & 0xFE), c);
				n += c;
				s += 2;
				r += 2;
				if (r > 40) {
					fprintf(f, "\n");
					r = 0;
				}
			}
			o = v;
			c = 1;
		}
	}
	fclose(f);
}*/

/*static void convert_font(void) {
	FILE *f;
	int i;
	int v;
	int n;

	n = 0;
	f = fopen("font.c", "w");
	for (i = 0; i < 256*75; i += 2) {
		v = (font.pixel_data[i*3] & 0xF0) | (font.pixel_data[(i+1)*3] >> 4);
		fprintf(f, "%d, ", v);
		n++;
		if (n > 40) {
			fprintf(f, "\n");
			n = 0;
		}
	}
	fclose(f);
}*/

/*static void write_bg_data(void) {

	bkrnd_ctx_t ctx;
	int i;
	int n;
	FILE *f;

	background_init(&ctx);
	
	n = 0;
	f = fopen("background.c", "w");
	for (i = 0; i < 320; i++) {
		fprintf(f, "{%d, %d, %d}, ", ctx.r, ctx.g, ctx.b);
		n++;
		if (n > 10) {
			fprintf(f, "\n");
			n = 0;
		}
		background_advance(&ctx);
	}
	fclose(f);
}*/

/*static void write_corner_data(void) {

	bkrnd_ctx_t ctx;
	int i, j, k;
	//int n;
	FILE *f;

	background_init(&ctx);
	
	//n = 0;
	i = 0;
	f = fopen("corner.c", "w");
	for (j = 0; j < 9; j++) {
		//fprintf(f, "{");
		for (k = 0; k < 9; k++) {
			fprintf(f, "%d, ", corner_fill.pixel_data[i]);
			i += 3;
		}
		//fprintf(f, "},\n");
		fprintf(f, "\n");
	}
	fclose(f);
}*/