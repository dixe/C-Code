//#define _UNICODE
//#include <windows.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <stdio.h>
//#include "window.h"
//
//// based on code from https://www.youtube.com/watch?v=q1fMa8Hufmg
//
//
//static bool quit = false;
//
//COLORREF getCusorColor()
//{
//  POINT pt;






//
//  GetCursorPos(&pt);
//  HDC hDC = GetDC(NULL);
//  COLORREF color = GetPixel(hDC, pt.x, pt.y);
//  ReleaseDC(GetDesktopWindow(), hDC);
//  return color;
//}
//
//static int reduce = 10000;
//
//void VmPaint(HWND window_handle);
//void OnQuit() {
//  quit = true;
//}
//
//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
//
//  // WINDOW SETUP
//  static HWND window_handle;
//  window_handle = NewWindow(hInstance, OnQuit, VmPaint);
//
//  while (!quit) {
//    static MSG message = { 0 };
//    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }
//
//    reduce -= 1;    
//    InvalidateRect(window_handle, NULL, TRUE);
//    UpdateWindow(window_handle);
//  }
//
//  return 0;
//}
//
//void DrawCursorColor(HDC hdc) {
//  COLORREF color = getCusorColor();
//
//  int r = GetRValue(color);
//  int g = GetGValue(color);
//  int b = GetBValue(color);
//  TCHAR text[256];
//  int c = swprintf_s(text, 256, L"Points: (%i, %i, %i)", r, g, b);
//  TextOutW(hdc, 40, 100, text, c);
//}
//
//void VmPaint(HWND window_handle) {
//  PAINTSTRUCT ps;
//  HDC hdc = BeginPaint(window_handle, &ps);
//  // Create a memory DC compatible with the screen
//  HDC memDC = CreateCompatibleDC(hdc);
//  RECT clientRect;
//  GetClientRect(window_handle, &clientRect);
//
//  // Create a bitmap compatible with the window DC
//  HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
//  HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
//
//  // Fill background
//  HBRUSH bgBrush = (HBRUSH)(COLOR_WINDOW + 1);
//  FillRect(memDC, &clientRect, bgBrush);
//
//  // Draw text into the memory DC
//  if (reduce > 0)
//  {
//    TextOutW(memDC, 0, 0, L"Hello, Windows !", 15);
//  }
//  else
//  {
//    TextOutW(memDC, 0, 0, L"JHello !", 8); // Correct length
//  }
//
//  // Blit the memory DC to the screen
//  BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
//
//  // Clean up
//  SelectObject(memDC, oldBitmap);
//  DeleteObject(memBitmap);
//  DeleteDC(memDC);
//
//  EndPaint(window_handle, &ps);
//}
//
