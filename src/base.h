#define WindowClass "BASE"
#define APP_NAME "Delay"

enum phase {
	PH_PLAY,
	PH_RESULT,
	PH_DONE,
	PH_ERROR,
};

extern enum phase phase;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern HINSTANCE vhInst;

void set_play(HWND hwnd);
void disable_ime(HWND hwnd);
