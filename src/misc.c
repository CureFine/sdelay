#define WIN32_LEAN_AND_MEAN
#define STRICT
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <mmsystem.h>

void *m_alloc(unsigned long int size)
{
	HANDLE h;
	void *p;
	DWORD e;

	h = GetProcessHeap();

	p= HeapAlloc(h, 0, size);
	if (!p) {
		e = GetLastError();
	}
	return p;
//  return HeapAlloc(GetProcessHeap(), 0, size);
}

void m_free(void *p)
{
	if (p)
		HeapFree(GetProcessHeap(), 0, p);
}

void mcopy(void *dst, void *src, int size)
{
  int c = size & 0x03;
  char *s = src;
  char *d = dst;

  while (c) {
    *d = *s;
    d++;
    s++;
    c--;
  }

  c = size >> 2;
  while (c) {
    *((long int *)d) = *((long int *)s);
    d += 4;
    s += 4;
    c--;
  }
}


void m_add(void *dst, void *src, int size)
{
  unsigned short int *s = src;
  unsigned short int *d = dst;

  if (size == 0) {
    return;
  }

  size /= 2;
  while (--size) {
    *d += *s;
    s++;
    d++;
  }
}



int m_cmp(void *dst, void *src, int size)
{
  int c = size & 0x03;
  char *s = src;
  char *d = dst;

  while (c) {
    if (*d != *s) {
      return *d - *s;
    }
    d++;
    s++;
    c--;
  }

  c = size >> 2;
  while (c) {
    if (*((long int *)d) != *((long int *)s)) {
      return *((long int *)d) - *((long int *)s);
    }
    d += 4;
    s += 4;
    c--;
  }
  return 0;
}


void zeromem(void *ptr, int size)
{
  int c = size & 0x03;
  char *p = ptr;

  while (c) {
    *p = 0x00;
    p++;
    c--;
  }

  c = size >> 2;
  while (c) {
    *((long int *)p) = 0L;
    p += 4;
    c--;
  }
}


void m_set32(void *ptr, unsigned long int c, int size)
{
	unsigned long int *p = ptr;

	size >>= 2;
	while (size) {
		*p = c;
		p++;
		size--;
	}
}


void disable_ime(HWND hwnd)
{
  HINSTANCE hInstImm;
  BOOL (FAR WINAPI *ImmFnc)(HWND, HANDLE);

  if ((hInstImm=GetModuleHandle("IMM32.DLL"))
     && (ImmFnc=(void*)GetProcAddress(hInstImm, "ImmAssociateContext"))) {
    /*  disable IME  */
    ImmFnc(hwnd, NULL);
  }
}

