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
#include "send_keys.h"
#include <asf.h>

#define KEY_DELAY_MS 5

typedef struct {
	char c;
	uint8_t hid_key;
	uint8_t hid_modifier;
} key_hid_t;

#define N_HID_KEYS 62

// mapping of characters to their respective HID equivalents
static key_hid_t key_hid_map[N_HID_KEYS] = {
	{'A', HID_A, HID_MODIFIER_LEFT_SHIFT},
	{'a', HID_A, HID_MODIFIER_NONE},
	{'B', HID_B, HID_MODIFIER_LEFT_SHIFT},
	{'b', HID_B, HID_MODIFIER_NONE},
	{'C', HID_C, HID_MODIFIER_LEFT_SHIFT},
	{'c', HID_C, HID_MODIFIER_NONE},
	{'D', HID_D, HID_MODIFIER_LEFT_SHIFT},
	{'d', HID_D, HID_MODIFIER_NONE},
	{'E', HID_E, HID_MODIFIER_LEFT_SHIFT},
	{'e', HID_E, HID_MODIFIER_NONE},
	{'F', HID_F, HID_MODIFIER_LEFT_SHIFT},
	{'f', HID_F, HID_MODIFIER_NONE},
	{'G', HID_G, HID_MODIFIER_LEFT_SHIFT},
	{'g', HID_G, HID_MODIFIER_NONE},
	{'H', HID_H, HID_MODIFIER_LEFT_SHIFT},
	{'h', HID_H, HID_MODIFIER_NONE},
	{'I', HID_I, HID_MODIFIER_LEFT_SHIFT},
	{'i', HID_I, HID_MODIFIER_NONE},
	{'J', HID_J, HID_MODIFIER_LEFT_SHIFT},
	{'j', HID_J, HID_MODIFIER_NONE},
	{'K', HID_K, HID_MODIFIER_LEFT_SHIFT},
	{'k', HID_K, HID_MODIFIER_NONE},
	{'L', HID_L, HID_MODIFIER_LEFT_SHIFT},
	{'l', HID_L, HID_MODIFIER_NONE},
	{'M', HID_M, HID_MODIFIER_LEFT_SHIFT},
	{'m', HID_M, HID_MODIFIER_NONE},
	{'N', HID_N, HID_MODIFIER_LEFT_SHIFT},
	{'n', HID_N, HID_MODIFIER_NONE},
	{'O', HID_O, HID_MODIFIER_LEFT_SHIFT},
	{'o', HID_O, HID_MODIFIER_NONE},
	{'P', HID_P, HID_MODIFIER_LEFT_SHIFT},
	{'p', HID_P, HID_MODIFIER_NONE},
	{'Q', HID_Q, HID_MODIFIER_LEFT_SHIFT},
	{'q', HID_Q, HID_MODIFIER_NONE},
	{'R', HID_R, HID_MODIFIER_LEFT_SHIFT},
	{'r', HID_R, HID_MODIFIER_NONE},
	{'S', HID_S, HID_MODIFIER_LEFT_SHIFT},
	{'s', HID_S, HID_MODIFIER_NONE},
	{'T', HID_T, HID_MODIFIER_LEFT_SHIFT},
	{'t', HID_T, HID_MODIFIER_NONE},
	{'U', HID_U, HID_MODIFIER_LEFT_SHIFT},
	{'u', HID_U, HID_MODIFIER_NONE},
	{'V', HID_V, HID_MODIFIER_LEFT_SHIFT},
	{'v', HID_V, HID_MODIFIER_NONE},
	{'W', HID_W, HID_MODIFIER_LEFT_SHIFT},
	{'w', HID_W, HID_MODIFIER_NONE},
	{'X', HID_X, HID_MODIFIER_LEFT_SHIFT},
	{'x', HID_X, HID_MODIFIER_NONE},
	{'Y', HID_Y, HID_MODIFIER_LEFT_SHIFT},
	{'y', HID_Y, HID_MODIFIER_NONE},
	{'Z', HID_Z, HID_MODIFIER_LEFT_SHIFT},
	{'z', HID_Z, HID_MODIFIER_NONE},
	{'0', HID_0, HID_MODIFIER_NONE},
	{'1', HID_1, HID_MODIFIER_NONE},
	{'2', HID_2, HID_MODIFIER_NONE},
	{'3', HID_3, HID_MODIFIER_NONE},
	{'4', HID_4, HID_MODIFIER_NONE},
	{'5', HID_5, HID_MODIFIER_NONE},
	{'6', HID_6, HID_MODIFIER_NONE},
	{'7', HID_7, HID_MODIFIER_NONE},
	{'8', HID_8, HID_MODIFIER_NONE},
	{'9', HID_9, HID_MODIFIER_NONE},	
};

void send_keys(char *s) {
	
	int i;
	int j;
	char c;
	
	i = 0;
	while (s[i] != 0) {
		
		c = s[i];
		
		for (j = 0; j < N_HID_KEYS; j++) {
			if (c == key_hid_map[j].c) {
				udi_hid_kbd_modifier_down(key_hid_map[j].hid_modifier);
				delay_ms(KEY_DELAY_MS);
				udi_hid_kbd_down(key_hid_map[j].hid_key);
				delay_ms(KEY_DELAY_MS);
				udi_hid_kbd_up(key_hid_map[j].hid_key);
				delay_ms(KEY_DELAY_MS);
				udi_hid_kbd_modifier_up(key_hid_map[j].hid_modifier);
				delay_ms(KEY_DELAY_MS);
				break;
			}
		}
		
		i++;
	}
	
}