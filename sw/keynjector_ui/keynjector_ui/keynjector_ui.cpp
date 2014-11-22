// keynjector_ui.cpp : Defines the entry point for the application.
//

#include <assert.h>
#include "stdafx.h"
#include "keynjector_ui.h"
#include "ui.h"

#define CALIB_INIT 0
#define CALIB_P1 1
#define CALIB_P2 2
#define CALIB_P3 3
#define CALIB_P4 4
#define CALIB_P5 5
#define CALIB_DONE 6

static int calib_success = 1;
static int sd_valid = 1;
static int file_exists = 1;
static int file_valid = 1;
static int file_format_valid = 1;
static int passphrase_correct = 1;
static int pin_correct = 1;

#define PASSPHRASE_BUFSIZE 21
#define PIN_BUFSIZE 6

static char test_items[] = "Entry1\tP1\nEntry2\tP2\nEntry3\tP3\nEntry4\tP4\nEntry5\tP5\nEntry6\tP6\nEntry7\tP7\nEntry8\tP8\nEntry9\tP9\nEntry10\tP10";

struct {
	int calib_state;
	int sd_card_inserted;
	char passphrase[PASSPHRASE_BUFSIZE];
	char pin[PIN_BUFSIZE];
	int pin_ok;
	int pin_entered;
	int pin_attempts;
	int pin_set;
	int decrypt_attempted;
	int decrypt_ok;
	int passphrase_set;
} g_state;

void init_state() {
	int i;

	g_state.calib_state = CALIB_DONE;
	g_state.sd_card_inserted = 1;
	for (i = 0; i < PASSPHRASE_BUFSIZE; i++) g_state.passphrase[i] = 0;
	g_state.pin_ok = 0;
	g_state.pin_set = 0;
	g_state.pin_entered = 0;
	g_state.pin_attempts = 0;
	g_state.decrypt_attempted = 0;
	g_state.decrypt_ok = 0;
	g_state.passphrase_set = 0;
}

static char const *MSG_INSERT_SD = "Insert SD card";
static char const *MSG_SD_INVALID = "SD card cannot be read";
static char const *MSG_NO_FILE = "Key file not found";
static char const *MSG_FILE_INVALID = "Key file invalid";
static char const *MSG_FILE_BAD_FORMAT = "Unknown key file format";
static char const *MSG_ENTER_PASSPHRASE = "Enter passphrase to unlock";
static char const *MSG_WRONG_PASSPHRASE = "Wrong passphrase - try again";
static char const *MSG_ASSIGN_PIN = "Assign PIN";
static char const *MSG_ENTER_PIN = "Enter PIN";
static char const *MSG_WRONG_PIN_1 = "Wrong PIN - 2 attempts left";
static char const *MSG_WRONG_PIN_2 = "Wrong PIN - 1 attempt left";

void set_next_view() {
	if (g_state.calib_state != CALIB_DONE) {
		g_state.calib_state = CALIB_INIT;
		lcd_set_view(VIEW_CALIB);
		lcd_start_command(CMD_CALIB_LBL_FADE_IN);
		return;
	}
	if (!g_state.sd_card_inserted) {
		g_state.decrypt_attempted = 0;
		g_state.decrypt_ok = 0;
		g_state.pin_ok = 0;
		g_state.pin_entered = 0;
		lcd_set_title_msg(MSG_INSERT_SD);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!sd_valid) {
		lcd_set_title_msg(MSG_SD_INVALID);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!file_exists) {
		lcd_set_title_msg(MSG_NO_FILE);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!file_valid) {
		lcd_set_title_msg(MSG_FILE_INVALID);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!file_format_valid) {
		lcd_set_title_msg(MSG_FILE_BAD_FORMAT);
		lcd_set_view(VIEW_TITLE);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	// attempt decryption
	if ((!g_state.decrypt_attempted) && g_state.passphrase_set) {
		g_state.decrypt_ok = passphrase_correct;
	}
	g_state.decrypt_attempted = 1;
	if (!g_state.decrypt_ok) {
		lcd_set_passphrase_buffer(g_state.passphrase, PASSPHRASE_BUFSIZE);
		g_state.passphrase_set = 0;
		g_state.decrypt_attempted = 0;
		g_state.pin_set = 0;
		lcd_set_passphrase_msg(MSG_ENTER_PASSPHRASE);
		lcd_set_view(VIEW_PASS);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	// attempt to set keys
	if (!lcd_set_keys(test_items)) {
		file_format_valid = 0;
		set_next_view();
		return;
	}
	// verify pin
	if (g_state.pin_entered && g_state.pin_set) {
		g_state.pin_ok = pin_correct;
	}
	if (!g_state.pin_set) {
		lcd_set_pin_msg(MSG_ASSIGN_PIN);
		lcd_set_pin_buffer(g_state.pin, PIN_BUFSIZE);
		g_state.pin_entered = 0;
		g_state.pin_ok = 0;
		lcd_set_view(VIEW_PIN);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	if (!g_state.pin_entered || !g_state.pin_ok) {
		lcd_set_pin_msg(MSG_ENTER_PIN);
		lcd_set_pin_buffer(g_state.pin, PIN_BUFSIZE);
		g_state.pin_entered = 0;
		g_state.pin_attempts = 3;
		lcd_set_view(VIEW_PIN);
		lcd_start_command(CMD_FADE_IN);
		return;
	}
	
	file_format_valid = 1;
	lcd_set_view(VIEW_KEY);
	lcd_start_command(CMD_FADE_IN);
}

void lcd_command_complete(int command) {

	switch (command) {
	case CMD_CALIB_LBL_FADE_IN:
		lcd_start_command(CMD_CALIB_POINT_1_FADE_IN);
		break;
	case CMD_CALIB_LBL_FADE_OUT:
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
		if (calib_success) {
			g_state.calib_state = CALIB_DONE;
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

void lcd_passphrase_ok() {
	g_state.passphrase_set = 1;
	if (passphrase_correct) {
		lcd_start_command(CMD_FADE_OUT);
	}
	else {
		lcd_set_passphrase_msg(MSG_WRONG_PASSPHRASE);
	}
}

void lcd_pin_ok() {
	if (!g_state.pin_set) {
		g_state.pin_entered = 1;
		g_state.pin_set = 1;
		g_state.pin_ok = 1;
		lcd_start_command(CMD_FADE_OUT);
		return;
	}
	if (pin_correct) {
		g_state.pin_entered = 1;
		lcd_start_command(CMD_FADE_OUT);
	}
	else {
		g_state.pin_attempts--;
		switch (g_state.pin_attempts) {
		case 2:
			lcd_set_pin_msg(MSG_WRONG_PIN_1);
			break;
		case 1:
			lcd_set_pin_msg(MSG_WRONG_PIN_2);
			break;
		default:
			g_state.passphrase_set = 0;
			g_state.decrypt_attempted = 0;
			g_state.decrypt_ok = 0;
			g_state.pin_ok = 0;
			g_state.pin_entered = 0;
			g_state.pin_set = 0;
			lcd_start_command(CMD_FADE_OUT);
			break;
		}
	}
}

void lcd_item_long_press(char *value) {
}

void calib_register_point() {

	switch (g_state.calib_state) {
	case CALIB_P1:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_1_FADE_OUT);
		break;
	case CALIB_P2:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_2_FADE_OUT);
		break;
	case CALIB_P3:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_3_FADE_OUT);
		break;
	case CALIB_P4:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_4_FADE_OUT);
		break;
	case CALIB_P5:
		g_state.calib_state = CALIB_INIT;
		lcd_start_command(CMD_CALIB_POINT_5_FADE_OUT);
		break;
	}

}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KEYNJECTOR_UI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEYNJECTOR_UI));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYNJECTOR_UI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KEYNJECTOR_UI);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

DWORD* dwArray;
BITMAPINFO bmInfo;
HBITMAP myBitmap;
HDC myCompatibleDC;

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

DWORD MakeDwordColor(int r, int g, int b)
{
	DWORD dwStuff = (((r & 0xFC) << 16) | ((g & 0xFC) << 8) | (b & 0xFC));

	return dwStuff;
}

int w_left, w_top, w_right, w_bottom;
int w_x, w_y;
HWND g_hWnd;

void lcd_select_window(int left, int top, int right, int bottom)
{
	w_left = left;
	w_top = top;
	w_right = right;
	w_bottom = bottom;
	w_x = left;
	w_y = top;
}

void lcd_write_pixel(int r, int g, int b)
{
	int i;

	i = w_y * LCD_WIDTH + w_x;
	dwArray[i] = MakeDwordColor(r, g, b);
	w_x++;
	if (w_x > w_right) {
		w_x = w_left;
		w_y++;
	}
	InvalidateRect(g_hWnd, NULL, FALSE);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

#define TIMER_ID 1
#define TIMER_PERIOD 50

LPSTR teststring[] = {"Test"};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int i;
	int nResult;
	POINTS p;
	RECT textr;
	char c;

	switch (message)
	{
	case WM_CREATE:
		//convert_font();
		//convert_logo();
		//write_bg_data();
		//write_corner_data();
		g_hWnd = hWnd;
		dwArray = (DWORD*) malloc(LCD_HEIGHT*LCD_WIDTH*sizeof(DWORD));

		//assert(dwArray);

		for(i=0; i<LCD_HEIGHT*LCD_WIDTH; i++)
		dwArray[i] = MakeDwordColor(0xff, 0x00, 0x00);  
		
		//Change this color here to change the bmp color

		ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
		bmInfo.bmiHeader.biBitCount = 32;
		bmInfo.bmiHeader.biHeight = -LCD_HEIGHT;
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = LCD_WIDTH;
		bmInfo.bmiHeader.biCompression = BI_RGB;


		myBitmap = CreateCompatibleBitmap(GetDC(hWnd), LCD_WIDTH, LCD_HEIGHT);
		//assert(myBitmap);

		myCompatibleDC = CreateCompatibleDC(GetDC(hWnd));
		//assert(myCompatibleDC);

		SelectObject(myCompatibleDC, myBitmap);

		//nResult = SetDIBits(myCompatibleDC, myBitmap, 0, LCD_HEIGHT, dwArray, &bmInfo, 0);
		lcd_select_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
		//lcd_write_pixel(0x00,0x00,0x00);

		//assert(nResult != 0);

		//lcd_draw_title_screen(1, 0xFF);
		init_state();

		//lcd_set_passphrase_buffer(g_state.passphrase, PASSPHRASE_BUFSIZE);
		lcd_set_view(VIEW_PIN);
		lcd_start_command(CMD_FADE_IN);
		//convert_logo();
		//lcd_test();
		SetTimer( hWnd, TIMER_ID, TIMER_PERIOD, NULL );

		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		nResult = SetDIBits(myCompatibleDC, myBitmap, 0, LCD_HEIGHT, dwArray, &bmInfo, 0);
		hdc = BeginPaint(hWnd, &ps);
		
		nResult = BitBlt(ps.hdc, 0, 0, LCD_WIDTH, LCD_HEIGHT, myCompatibleDC, 0, 0, SRCCOPY);

		textr.left = 300;
		textr.top = 50;
		textr.right = 500;
		textr.bottom = 100;
		
		if (calib_success) 
			nResult = TextOut(hdc, 300, 50, TEXT("Calibration succeeds"), strlen("Calibration succeeds"));
		else
			nResult = TextOut(hdc, 300, 50, TEXT("Calibration fails"), strlen("Calibration fails"));

		if (g_state.sd_card_inserted) 
			nResult = TextOut(hdc, 300, 70, TEXT("SD inserted"), strlen("SD inserted"));
		else
			nResult = TextOut(hdc, 300, 70, TEXT("SD not inserted"), strlen("SD not inserted"));

		if (sd_valid) 
			nResult = TextOut(hdc, 300, 90, TEXT("SD valid"), strlen("SD valid"));
		else
			nResult = TextOut(hdc, 300, 90, TEXT("SD not valid"), strlen("SD not valid"));

		if (file_exists) 
			nResult = TextOut(hdc, 300, 110, TEXT("File exists"), strlen("File exists"));
		else
			nResult = TextOut(hdc, 300, 110, TEXT("File does not exist"), strlen("File does not exist"));

		if (file_exists) 
			nResult = TextOut(hdc, 300, 130, TEXT("File valid"), strlen("File valid"));
		else
			nResult = TextOut(hdc, 300, 130, TEXT("File not valid"), strlen("File not valid"));

		if (passphrase_correct) 
			nResult = TextOut(hdc, 300, 150, TEXT("Passphrase correct"), strlen("Passphrase correct"));
		else
			nResult = TextOut(hdc, 300, 150, TEXT("Passphrase not correct"), strlen("Passphrase not correct"));

		
		if (pin_correct) 
			nResult = TextOut(hdc, 300, 170, TEXT("PIN correct"), strlen("PIN correct"));
		else
			nResult = TextOut(hdc, 300, 170, TEXT("PIN not correct"), strlen("PIN not correct"));


		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_LBUTTONDOWN:
		if ((g_state.calib_state >= CALIB_P1) && (g_state.calib_state <= CALIB_P5)) {
			calib_register_point();
		}
		p = MAKEPOINTS(lParam);
		view_mouse_down(p.x, p.y);
		nResult = SetDIBits(myCompatibleDC, myBitmap, 0, LCD_HEIGHT, dwArray, &bmInfo, 0);
		hdc = BeginPaint(hWnd, &ps);
		
		nResult = BitBlt(ps.hdc, 0, 0, LCD_WIDTH, LCD_HEIGHT, myCompatibleDC, 0, 0, SRCCOPY);
		break;
	case WM_LBUTTONUP:
		p = MAKEPOINTS(lParam);
		view_mouse_up(p.x, p.y);
		nResult = SetDIBits(myCompatibleDC, myBitmap, 0, LCD_HEIGHT, dwArray, &bmInfo, 0);
		hdc = BeginPaint(hWnd, &ps);
		
		nResult = BitBlt(ps.hdc, 0, 0, LCD_WIDTH, LCD_HEIGHT, myCompatibleDC, 0, 0, SRCCOPY);
		break;
	case WM_MOUSEMOVE:
		p = MAKEPOINTS(lParam);
		view_mouse_move(p.x, p.y);
		nResult = SetDIBits(myCompatibleDC, myBitmap, 0, LCD_HEIGHT, dwArray, &bmInfo, 0);
		hdc = BeginPaint(hWnd, &ps);
		
		nResult = BitBlt(ps.hdc, 0, 0, LCD_WIDTH, LCD_HEIGHT, myCompatibleDC, 0, 0, SRCCOPY);
		break;
	case WM_KEYDOWN:
		c = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
		switch (c) {
		case 'C':
			calib_success = !calib_success;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'S':
			g_state.sd_card_inserted = !g_state.sd_card_inserted;
			lcd_start_command(CMD_FADE_OUT);
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'V':
			sd_valid = !sd_valid;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'E':
			file_exists = !file_exists;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'W':
			file_valid = !file_valid;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'P':
			passphrase_correct = !passphrase_correct;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		case 'I':
			pin_correct = !pin_correct;
			InvalidateRect(g_hWnd, NULL, FALSE);
			break;
		}

		break;
	case WM_TIMER:
		/*if (clicked) {
			lcd_draw_title_screen(0, (0xFF * stage)/5);

			if (stage > 0) {
				stage--;
			}
		}*/
		lcd_timer_tick();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
