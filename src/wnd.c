#define WIN32_LEAN_AND_MEAN
#define STRICT
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>

#include "base.h"
#include "misc.h"
#include "wave.h"

void set_result(HWND hwnd);
void set_initfailure(HWND hwnd);

int point;
extern HWND main_window;
extern int snd_playing;
extern int snd_accept;

static HBITMAP vDIB;
static HBITMAP vBackupBMP;
static HFONT vFont, vBackupFont;
HDC vDC;
DWORD *vVRAM;
DWORD *v_backVRAM;

static DWORD sound_tick;

static int i_ticks;
static DWORD ticks[4] = {0xffff, 0xffff, 0xffff, 0xffff};

#define FPS 60
#define F_CX 1024
#define F_CY  768
#define TIME (60 * FPS)

static DWORD pre_tick;

static struct pcm_data *se1;

static HFONT create_font(HWND hwnd)
{
	LOGFONT lf;
	HFONT f;

	zeromem(&lf, sizeof(lf));
	lf.lfHeight = 40;
	lf.lfWidth = 0;
	lf.lfCharSet = DEFAULT_CHARSET;

	f = CreateFontIndirect(&lf);

	return f;
}


static int init_vram(HWND hwnd, int cx, int cy)
{
	HDC dc;
	BITMAPINFO *bmi;
	DWORD *p;

	bmi = m_alloc(sizeof(BITMAPINFOHEADER));
	if (!bmi) {
		return 1;
	}
	zeromem(bmi, sizeof(BITMAPINFOHEADER));

	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biBitCount = 32;
	bmi->bmiHeader.biCompression = BI_RGB;/*BITFIELDS;*/
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biWidth = cx;
	bmi->bmiHeader.biHeight = cy;
	p = (DWORD *)&bmi->bmiColors[0];

	dc = GetDC(hwnd);
	vDIB = CreateDIBSection(dc, bmi, DIB_RGB_COLORS, (void **)&vVRAM, NULL, 0);
	m_free(bmi);
	if (!vDIB) {
		ReleaseDC(hwnd, dc);
		return 1;
	}
	vDC = CreateCompatibleDC(dc);
	ReleaseDC(hwnd, dc);

	v_backVRAM = m_alloc(F_CX * F_CY * 4);
	if (!v_backVRAM) {
		DeleteObject(vDIB);
		return 1;
	}

	vBackupBMP = SelectObject(vDC, vDIB);
	m_set32(vVRAM, RGB(63, 63, 63), F_CX * F_CY * 4);
	m_set32(v_backVRAM, RGB(63, 63, 63), F_CX * F_CY * 4);
	m_set32(v_backVRAM + F_CX * (F_CY - 2) / 2, RGB(192, 192, 192), F_CX * 4 * 4);

	vFont = create_font(hwnd);
	vBackupFont = SelectObject(vDC, vFont);

	return 0;
}

static short int *store_pulse(short int *d, unsigned short int v, int f, int len)
{
	int n;
	int s;

	s = f / 2;
	n = 0;
	while (--len) {
		d[0] = v;
		d[1] = v;
		d += 2;
		if (n == s) {
			v = -v;
		}
		if (n == f) {
			v = -v;
			n = 0;
		}
		n++;
	}
	return d;
}

static struct pcm_data *make_pcm_beep(void)
{
	struct pcm_data *r;
	short int *d;
	int f;

	r = m_alloc(sizeof(*r) + (sizeof(short int) * 2 * ((WAVE_RATE / 4))));
	if (!r) {
		return NULL;
	}
	r->len = (WAVE_RATE / 4);

	r->data = r + 1;
	d = r->data;

	f = WAVE_RATE / 441;

	d = store_pulse(d, 4096, f, WAVE_RATE / 4);

	return r;
}

static int on_create(HWND hwnd)
{
	se1 = make_pcm_beep();
	if (!se1) {
		return -1;
	}

	if(init_vram(hwnd, 1024, 768)) {
		return -1;
	}

	disable_ime(hwnd);
	return 0;
}


void on_destroy(HWND hwnd)
{
	phase = PH_DONE;
	sound_stop();
	m_free(se1);

	SelectObject(vDC, vBackupBMP);
	SelectObject(vDC, vBackupFont);
	
	DeleteObject(vFont);
	DeleteObject(vDIB);
	DeleteObject(vDC);

	PostQuitMessage(0);
}

static void on_paint(HWND hwnd)
{
	HDC dc;
	PAINTSTRUCT ps;

	dc = BeginPaint(hwnd, &ps);
	BitBlt(dc, ps.rcPaint.left, ps.rcPaint.top,  ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, vDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
	EndPaint(hwnd, &ps);
}


static void update_tick(void)
{
	DWORD tick;
	int  n;
	int m;
	tick = timeGetTime();
	n = tick - pre_tick;
	m = 1000 - n;

	ticks[i_ticks] = n < m ? n : -m;
	i_ticks++;
	if (4 <= i_ticks) {
		i_ticks = 0;
	}

	return;
}

static void on_lbuttondown(HWND hwnd, int x, int y)
{
	update_tick();
	return;
}

static void on_keydown(HWND hwnd, WPARAM w, LPARAM l)
{
	if (l & 0x40000000) {
		return;
	}
	update_tick();
	return;
}


static void on_mousemove(HWND hwnd, int x, int y)
{
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){/*  WM_xxxx  */
	case WM_CREATE:
		return on_create(hwnd);
		break;
	case WM_DESTROY:
		on_destroy(hwnd);
		break;
	case WM_ACTIVATE:
		break;
	case WM_PAINT:
		on_paint(hwnd);
		break;
	case WM_TIMER:
		break;
	case WM_LBUTTONDOWN:
		on_lbuttondown(hwnd, LOWORD(lParam), F_CY - HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		on_mousemove(hwnd, LOWORD(lParam), F_CY - HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		break;
	case WM_KEYDOWN:
		on_keydown(hwnd, wParam, lParam);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0L;
}

void advance(DWORD tick)
{
	HDC dc;
	DWORD diff;
	int off_y;
	int i;
	int n;
	char buf[64];

	diff = (tick - pre_tick);

	if (1000 <= (tick - pre_tick)) {
		sound_tick = tick - pre_tick;
		pcm_set(se1);
		sound_run();
		pre_tick = tick;
	}
	/*
	m_set32(vVRAM, RGB(63, 63, 63), F_CX * F_CY * 4);
	  */
	off_y = 0;
	mcopy(vVRAM, v_backVRAM, F_CX * F_CY * 4);
	if ((1000 - F_CY / 2) < diff) {
		off_y = 1000 - (signed int)diff + (F_CY / 2);
	}
	if (diff < (F_CY / 2)) {
		off_y = (F_CY / 2) - diff;
	}
	if (0 <= off_y && off_y < 768) {
		m_set32(vVRAM + off_y * F_CX, RGB(242, 128, 128), F_CX * 4);
	}
	SetTextColor(vDC, RGB(242, 242, 242));
	SetBkMode(vDC, TRANSPARENT);
	for (i = 0; i < 4; i++) {
		if (ticks[i] == 0xffff) {
			continue;
		}
		n = wsprintf(buf, "%dms", ticks[i]);
		TextOut(vDC, (F_CX / 4) * i, 300, buf, n);
		off_y = (F_CY / 2 - ticks[i]);
		if (0 <= off_y && off_y < 768) {
			m_set32(vVRAM + (off_y * F_CX) + ((F_CX / 4) * i), RGB(242, 242, 242), F_CX / 4 * 4);
		}
	}

	dc = GetDC(main_window);
	BitBlt(dc, 0, 0, F_CX, F_CY, vDC, 0, 0, SRCCOPY);
	ReleaseDC(main_window, dc);
}

void set_play(HWND hwnd)
{
	phase = PH_PLAY;

	point = 0;
	i_ticks = 0;
	pre_tick = timeGetTime();

	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)WndProc);
}
