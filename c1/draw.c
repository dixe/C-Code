#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdbool.h>
#include <stdint.h> 
#include <stdio.h>


COLORREF getGusorColor()
{

  POINT pt;

  GetCursorPos(&pt);
  HDC hDC = GetDC(NULL);
  COLORREF color = GetPixel(hDC, pt.x, pt.y);

 
  ReleaseDC(GetDesktopWindow(), hDC);

  return color;  
}



int main(int argc, TCHAR* argv[])
{


  bool success = RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 'C');

  if (!success)
  {
    printf("Failed to register ALT + C\n");
    return 1;
  }

  printf("Registered ALT + C for cursor color\n");

  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0)
  {
    if (msg.message == WM_HOTKEY)
    {
      printf("Got hotkey ALT + C\n");

      COLORREF color = getGusorColor();
      
      printf("Color at cursor is (%i, %i, %i)\n", GetRValue(color), GetGValue(color), GetBValue(color));

    }
  }

  return 0;
}