#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <mmsystem.h>

#include "resource.h"
#include "base.h"
#include "wave.h"

#define FPS 60
enum phase phase;

HINSTANCE vhInst;

void Entry(void);

HWND create_app_window(void);
HWND main_window;

void advance(DWORD tick);

#ifndef RELEASE
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
  Entry();
  return 0;
}
#endif

int init_app(void);


void Entry(void)
{
	MSG msg;
	HWND hwnd;
	DWORD now_time;
	DWORD pre_time;
	int num_frame;
	int adj;
	int n;

	vhInst = GetModuleHandle(NULL);

	if (!init_app()) {
		msg.wParam = 1;
		goto EXIT;
	}

	phase = PH_PLAY;
	hwnd = create_app_window();
	if (!hwnd) {
		msg.wParam = 1;
		goto EXIT;
	}
	main_window = hwnd;
	ShowWindow(hwnd, SW_SHOW);
	set_play(hwnd);

	num_frame = 0;
	adj = 0;

	timeBeginPeriod(1);
	for (;;) {
		if (phase == PH_PLAY) {
			now_time = timeGetTime();
			if (pre_time != now_time) {
				advance(now_time);
				pre_time = now_time;
			}
			n = MsgWaitForMultipleObjects(0, NULL, TRUE, 0, QS_ALLEVENTS | QS_ALLINPUT);
			switch (n) {
			case WAIT_OBJECT_0:
				while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)){
					if (msg.message==WM_QUIT) goto EXIT;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				break;
			case WAIT_TIMEOUT:
				break;
			}
		} else {
			while (phase != PH_PLAY) {
				if (GetMessage(&msg, (HWND)NULL, 0, 0)){
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				} else {
					goto EXIT;
				}
			}
		}
	}
	timeEndPeriod(1);

EXIT:
	sound_close();
	ExitProcess(msg.wParam);
}


HWND create_app_window(void)
{
  HWND hwnd;

  hwnd = CreateWindowEx(
    WS_EX_APPWINDOW,
    WindowClass, APP_NAME,
    WS_SYSMENU,
    0, 0,
    1024, 768,
    NULL, NULL, vhInst, NULL);

  return hwnd;
}


int init_app(void)
{
	WNDCLASS wc;

	/* for hidden window */
	wc.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_BYTEALIGNCLIENT;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = vhInst;
	wc.hIcon         = LoadIcon(vhInst, MAKEINTRESOURCE(IDI_MAIN));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WindowClass;

	if (!RegisterClass(&wc)) {
		return 0;
	}

	sound_open();

	return 1;
}

