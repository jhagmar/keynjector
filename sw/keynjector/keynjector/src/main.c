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
#include <stdlib.h>
#include <string.h>
#include <asf.h>
#include "lcd.h"
#include "ui.h"
#include "qsort.h"
#include "opgp.h"
#include "ff_stream.h"
#include "send_keys.h"
#include "sha1.h"

static volatile uint32_t g_ul_tick_count = 0;

// handler for system tick (every 1ms)
void SysTick_Handler(void)
{
	g_ul_tick_count++;

	// update touch panel status every 10ms
	// and lcd animation every 50ms
	if ((g_ul_tick_count % 10) == 0) {
		rtouch_process();
		if ((g_ul_tick_count % 50) == 0) {
			lcd_timer_tick();
		}
	}
}

// touch panel event handler
static void event_handler(rtouch_event_t const *event)
{
	switch(event->type) {
		case RTOUCH_MOVE:
		view_mouse_move(event->panel.x, event->panel.y);
		break;
		case RTOUCH_PRESS:
		view_mouse_down(event->panel.x, event->panel.y);
		break;
		case RTOUCH_RELEASE:
		view_mouse_up(event->panel.x, event->panel.y);
		break;
		default:
		break;
	}
}

// values for calibration status
#define CALIB_INIT 0
#define CALIB_P1 1
#define CALIB_P2 2
#define CALIB_P3 3
#define CALIB_P4 4
#define CALIB_P5 5
#define CALIB_DONE 6

#define PASSPHRASE_BUFSIZE 101
#define PIN_BUFSIZE 101

// file name for encrypted key file
static char KEY_FILE_NAME[] = "0:keys.gpg";

// state variables
static volatile FATFS fs;
static volatile FIL file_object;
static volatile int file_format_valid = 1;
static volatile char *items = NULL;
static volatile char pin[PIN_BUFSIZE];
static volatile int sd_card_inserted;
static volatile	int pin_ok = 0;
static volatile	int pin_entered = 0;
static volatile int pin_attempts = 0;
static volatile int decrypt_attempted = 0;
static volatile int decrypt_ok = 0;

#define N_CALIB_POINTS 5

// persistent state variables
static volatile struct {
	rtouch_calibration_point_t gs_calibration_points[N_CALIB_POINTS];
	int calib_state;
	char passphrase[PASSPHRASE_BUFSIZE];
	char pin[PIN_BUFSIZE];
	int pin_set;
	int passphrase_set;
	uint8_t digest[SHA1_DIGEST_SIZE]; // hash to know if state is valid
} g_state;

// compute the persistent state digest
static void compute_state_digest(uint8_t digest[SHA1_DIGEST_SIZE]) {
	uint8_t *p1;
	uint8_t *p2;
	sha1_ctx ctx;
	
	p1 = (uint8_t *)&g_state;
	p2 = g_state.digest;
	sha1_hash(&ctx, p1, p2 - p1);
	sha1_digest(&ctx, digest);
}

// set the state digest to mark it as valid
static void set_state_digest(void) {
	compute_state_digest(g_state.digest);
}

// verify that the persistent state is valid by checking the digest
static int verify_state_digest(void) {
	uint8_t digest[SHA1_DIGEST_SIZE];
	
	compute_state_digest(digest);
	return (memcmp(digest, g_state.digest, SHA1_DIGEST_SIZE) == 0);
}

// initialize the state
static void init_state(void) {
	int i;

	g_state.calib_state = CALIB_INIT;
	for (i = 0; i < PASSPHRASE_BUFSIZE; i++) g_state.passphrase[i] = 0;
	for (i = 0; i < PIN_BUFSIZE; i++) g_state.pin[i] = 0;
	g_state.pin_set = 0;
	g_state.passphrase_set = 0;
	g_state.gs_calibration_points[0].panel.x = CALIB_POINTS_X1;
	g_state.gs_calibration_points[0].panel.y = CALIB_POINTS_Y1;
	
	g_state.gs_calibration_points[1].panel.x = CALIB_POINTS_X2;
	g_state.gs_calibration_points[1].panel.y = CALIB_POINTS_Y1;
	
	g_state.gs_calibration_points[2].panel.x = CALIB_POINTS_X2;
	g_state.gs_calibration_points[2].panel.y = CALIB_POINTS_Y2;
	
	g_state.gs_calibration_points[3].panel.x = CALIB_POINTS_X1;
	g_state.gs_calibration_points[3].panel.y = CALIB_POINTS_Y2;
	
	g_state.gs_calibration_points[4].panel.x = CALIB_POINTS_X0;
	g_state.gs_calibration_points[4].panel.y = CALIB_POINTS_Y0;
	 
}

// NVRAM address for persistent state
#define STATE_ADDRESS_INT (IFLASH_ADDR + IFLASH_SIZE- IFLASH_PAGE_SIZE*8)

// read the persistent state from NVRAM
static void read_state(void) {
	nvm_read(INT_FLASH, STATE_ADDRESS_INT, &g_state, sizeof(g_state));
	if (!verify_state_digest()) {
		// no valid state read - initialize to default
		init_state();
	}
	if (g_state.calib_state == CALIB_DONE) {
		rtouch_compute_calibration((rtouch_calibration_point_t *) &g_state.gs_calibration_points);
	}
}

// write the persistent state to NVRAM
static void write_state(void) {
	set_state_digest();
	nvm_write(INT_FLASH, STATE_ADDRESS_INT, &g_state, sizeof(g_state));
}

// check if SD card is valid
static int sd_valid(void) {
	Ctrl_status status;
	FRESULT res;
	
	do {
		status = sd_mmc_test_unit_ready(0);
	} while (status == CTRL_BUSY);
	if (status != CTRL_GOOD) return 0;
	
	memset(&fs, 0, sizeof(FATFS));
	// attempt to mount unit
	res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
	
	return (res == FR_OK);
}

// check if key file exists
static int file_exists(void) {
	FRESULT res;
	
	res = f_stat(KEY_FILE_NAME, NULL);
	return (res == FR_OK);
}

// attempt to decrypt key file
static int decrypt_file(void) {
	mpgp_stream_t *in;
	mpgp_stream_t *key_stream;
	size_t buf_len = 1024;
	size_t bytes_read;
	char *buf;
	FRESULT res;
	
	// open file
	res = f_open(&file_object, KEY_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) return 0;
	// wrap FatFS file handle in stream interface
	in = mpgp_ff_sopen(&file_object);
	if (in == NULL) return 0;
	// decrypt file
	key_stream = mpgp_open(in, g_state.passphrase, strlen(g_state.passphrase));
	if (key_stream == NULL) {
		return 0;		
	}
	// free memory for key items if already allocated
	if (items != NULL) {
		free(items);
	}
	
	buf = malloc(buf_len);
	if (buf == NULL) return 0;
	bytes_read = mpgp_sread(key_stream, buf, 1, buf_len);
	buf[bytes_read] = 0;
	items = buf;
	mpgp_sclose(key_stream);
	
	return 1;
}

// check if used supplied PIN is correct
static int verify_pin(void) {
	return (strcmp(g_state.pin, pin) == 0);
}

// messages to user
static char const *MSG_INSERT_SD = "Insert SD card";
static char const *MSG_SD_INVALID = "SD card cannot be read";
static char const *MSG_NO_FILE = "Key file not found";
static char const *MSG_FILE_BAD_FORMAT = "Unknown key file format";
static char const *MSG_ENTER_PASSPHRASE = "Enter passphrase to unlock";
static char const *MSG_WRONG_PASSPHRASE = "Wrong passphrase - try again";
static char const *MSG_ASSIGN_PIN = "Assign PIN";
static char const *MSG_ENTER_PIN = "Enter PIN";
static char const *MSG_WRONG_PIN_1 = "Wrong PIN - 2 attempts left";
static char const *MSG_WRONG_PIN_2 = "Wrong PIN - 1 attempt left";

// determine which view to display based on state
static void set_next_view(void) {
	if (g_state.calib_state != CALIB_DONE) {
		// calibration needed
		g_state.calib_state = CALIB_INIT;
		lcd_set_view(VIEW_CALIB);
		lcd_start_command(CMD_CALIB_LBL_FADE_IN);
		return;
	}
	if (!sd_card_inserted) {
		// no SD card inserted
		// display title screen to alert user
		decrypt_attempted = 0;
		decrypt_ok = 0;
		pin_ok = 0;
		pin_entered = 0;
		file_format_valid = 1;
		lcd_set_title_msg(MSG_INSERT_SD);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!sd_valid()) {
		// SD card not valid - inform user
		lcd_set_title_msg(MSG_SD_INVALID);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!file_exists()) {
		// key file does not exist - inform user
		lcd_set_title_msg(MSG_NO_FILE);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	// attempt decryption
	if ((!decrypt_attempted) && g_state.passphrase_set) {
		// attempt to decrypt file
		decrypt_ok = decrypt_file();
	}
	decrypt_attempted = 1;
	if (!decrypt_ok) {
		// decryption failed - prompt for passphrase
		lcd_set_passphrase_buffer(g_state.passphrase, PASSPHRASE_BUFSIZE);
		g_state.passphrase_set = 0;
		decrypt_attempted = 0;
		g_state.pin_set = 0;
		lcd_set_passphrase_msg(MSG_ENTER_PASSPHRASE);
		lcd_set_view(VIEW_PASS);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	/*if (!file_valid) {
		lcd_set_title_msg(MSG_FILE_INVALID);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}*/
	if (!file_format_valid) {
		// file format is not valid - inform user
		lcd_set_title_msg(MSG_FILE_BAD_FORMAT);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	// attempt to set keys
	if (!lcd_set_keys(items)) {
		// could not set keys - mark file format as invalid
		file_format_valid = 0;
		set_next_view();
		return;
	}
	// verify pin
	if (pin_entered && g_state.pin_set) {
		pin_ok = verify_pin();
	}
	if (!g_state.pin_set) {
		// no PIN set - promt user for new PIN
		lcd_set_pin_msg(MSG_ASSIGN_PIN);
		lcd_set_pin_buffer(g_state.pin, PIN_BUFSIZE);
		pin_entered = 0;
		pin_ok = 0;
		lcd_set_view(VIEW_PIN);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!pin_entered || !pin_ok) {
		// promt for PIN
		lcd_set_pin_msg(MSG_ENTER_PIN);
		lcd_set_pin_buffer(pin, PIN_BUFSIZE);
		pin_entered = 0;
		pin_attempts = 3;
		lcd_set_view(VIEW_PIN);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	
	file_format_valid = 1;
	lcd_set_view(VIEW_KEY);
	lcd_start_command(CMD_FADE_IN);
}


#define CALIB_OVERSAMPLING 100

// get calibration data from user
static void get_calib_point(int i) {
	uint32_t x[CALIB_OVERSAMPLING],y[CALIB_OVERSAMPLING];
	int j;
	
	// wait for new touch panel press
	while (rtouch_is_pressed()) wdt_restart(WDT);
	while (!rtouch_is_pressed()) wdt_restart(WDT);
	
	// collect touch panel samples
	for (j = 0; j < CALIB_OVERSAMPLING; j++) {
		if (!rtouch_is_pressed()) return;
		rtouch_get_raw_point(&x[j], &y[j]);
	}
	// sort samples to obtain median
	quicksort(x, CALIB_OVERSAMPLING);
	quicksort(y, CALIB_OVERSAMPLING);
	g_state.gs_calibration_points[i].raw.x = x[CALIB_OVERSAMPLING/2];
	g_state.gs_calibration_points[i].raw.y = y[CALIB_OVERSAMPLING/2];
	
	// fade out current calibration point
	switch (i) {
	case 0:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_1_FADE_OUT);
		break;
	case 1:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_2_FADE_OUT);
		break;
	case 2:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_3_FADE_OUT);
		break;
	case 3:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_4_FADE_OUT);
		break;
	case 4:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_5_FADE_OUT);
		break;		
	}
	rtouch_wait_released();
}

// callback from UI that command is complete
void lcd_command_complete(int command);
void lcd_command_complete(int command) {

	switch (command) {
		case CMD_CALIB_LBL_FADE_IN:
		lcd_start_command(CMD_CALIB_POINT_1_FADE_IN);
		break;
		case CMD_CALIB_LBL_FADE_OUT:
		// calibration done
		set_next_view();
		break;
		case CMD_CALIB_POINT_1_FADE_IN:
		g_state.calib_state = CALIB_P1;
		break;
		case CMD_CALIB_POINT_1_FADE_OUT:
		lcd_start_command(CMD_CALIB_POINT_2_FADE_IN);
		break;
		case CMD_CALIB_POINT_2_FADE_IN:
		g_state.calib_state = CALIB_P2;
		break;
		case CMD_CALIB_POINT_2_FADE_OUT:
		lcd_start_command(CMD_CALIB_POINT_3_FADE_IN);
		break;
		case CMD_CALIB_POINT_3_FADE_IN:
		g_state.calib_state = CALIB_P3;
		break;
		case CMD_CALIB_POINT_3_FADE_OUT:
		lcd_start_command(CMD_CALIB_POINT_4_FADE_IN);
		break;
		case CMD_CALIB_POINT_4_FADE_IN:
		g_state.calib_state = CALIB_P4;
		break;
		case CMD_CALIB_POINT_4_FADE_OUT:
		lcd_start_command(CMD_CALIB_POINT_5_FADE_IN);
		break;
		case CMD_CALIB_POINT_5_FADE_IN:
		g_state.calib_state = CALIB_P5;
		break;
		case CMD_CALIB_POINT_5_FADE_OUT:
		// check if calibration quality is satisfactory
		if (rtouch_compute_calibration((rtouch_calibration_point_t *) &g_state.gs_calibration_points) == 0) {
			g_state.calib_state = CALIB_DONE;
			write_state();
			lcd_start_command(CMD_CALIB_LBL_FADE_OUT);
		}
		else {
			lcd_start_command(CMD_CALIB_POINT_1_FADE_IN);
		}
		break;
		case CMD_FADE_IN:
		if (lcd_get_view() == VIEW_LOGO) {
			lcd_start_command(CMD_FADE_OUT);
		}
		break;
		case CMD_FADE_OUT:
		set_next_view();
		break;
	}

}

// callback from UI that passphrase OK button was clicked
void lcd_passphrase_ok(void);
void lcd_passphrase_ok(void) {
	g_state.passphrase_set = 1;
	decrypt_ok = decrypt_file();
	decrypt_attempted = 1;
	if (decrypt_ok) {
		lcd_start_command(CMD_FADE_OUT);
	}
	else {
		lcd_set_passphrase_msg(MSG_WRONG_PASSPHRASE);
	}
}

// callback from UI that PIN OK button was clicked
void lcd_pin_ok(void);
void lcd_pin_ok(void) {
	if (!g_state.pin_set) {
		// requested to set PIN - write new PIN to NVRAM
		pin_entered = 1;
		g_state.pin_set = 1;
		strcpy(pin, g_state.pin);
		write_state();
		lcd_start_command(CMD_FADE_OUT);
		return;
	}
	if (strcmp(pin, g_state.pin) == 0) {
		// correct PIN entered
		pin_entered = 1;
		lcd_start_command(CMD_FADE_OUT);
	}
	else {
		// wrong PIN
		pin_attempts--;
		switch (pin_attempts) {
			case 2:
			lcd_set_pin_msg(MSG_WRONG_PIN_1);
			break;
			case 1:
			lcd_set_pin_msg(MSG_WRONG_PIN_2);
			break;
			default:
			g_state.passphrase_set = 0;
			decrypt_attempted = 0;
			decrypt_ok = 0;
			pin_ok = 0;
			pin_entered = 0;
			g_state.pin_set = 0;
			write_state();
			lcd_start_command(CMD_FADE_OUT);
			break;
		}
	}
}

static bool my_flag_autorize_keyboard_events = false;
bool my_callback_keyboard_enable(void)
{
	my_flag_autorize_keyboard_events = true;
	return true;
}
void my_callback_keyboard_disable(void)
{
	my_flag_autorize_keyboard_events = false;
}

#define KEY_DELAY 5

// callback from UI that key item was long pressed
void lcd_item_long_press(char *s);
void lcd_item_long_press(char *s) {
	
	send_keys(s);
}

void usb_init(void);
void usb_init(void)
{
	udc_start();
}

int main (void)
{
	sd_mmc_err_t last_mmc_err;
	sd_mmc_err_t new_mmc_err;
	
	KEY_FILE_NAME[0] = LUN_ID_SD_MMC_0_MEM + '0';
	
	// initialization
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	board_init();
	//sleepmgr_init(); // Optional
	sd_mmc_init();
	nvm_init(INT_FLASH);
	udc_start();
	lcd_init();
	
	SysTick_Config(sysclk_get_cpu_hz() / 1000);	
	
	// initialize touch panel driver
	rtouch_init(LCD_WIDTH, LCD_HEIGHT);
	rtouch_enable();
	rtouch_set_event_handler(event_handler);
	
	// read persistent state
	read_state();
	
	// initialize SD inserted state
	last_mmc_err = sd_mmc_check(0);
	sd_card_inserted = (last_mmc_err != CTRL_NO_PRESENT);
	
	lcd_set_view(VIEW_LOGO);
	lcd_start_command(CMD_FADE_IN);
	
	while (1) {

		// cycle through calibration points
		while (g_state.calib_state != CALIB_DONE) {
			switch (g_state.calib_state) {
			case CALIB_P1:
				get_calib_point(0);
				break;
			case CALIB_P2:
				get_calib_point(1);
				break;
			case CALIB_P3:
				get_calib_point(2);
				break;
			case CALIB_P4:
				get_calib_point(3);
				break;
			case CALIB_P5:
				get_calib_point(4);
				break;
			default:
				break;
			}
		}
		
		// check if SD card is inserted
		new_mmc_err = sd_mmc_check(0);
		// a change in SD inserted state triggers a fade out
		// and switch to appropriate view
		if (new_mmc_err != last_mmc_err) {
			sd_card_inserted = (new_mmc_err != CTRL_NO_PRESENT);
			last_mmc_err = new_mmc_err;
			lcd_start_command(CMD_FADE_OUT);
		}
		
		wdt_restart(WDT);
	}
}
