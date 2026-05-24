#define UNICODE
#define _UNICODE

#include <windows.h>
#include "canvas/Canvas.h"
#include "canvas/StrokeCapture.h"

static mb::Canvas        g_canvas;
static mb::StrokeCapture* g_capture = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
        g_canvas.render();
        ValidateRect(hwnd, nullptr);
        return 0;

    case WM_SIZE:
        g_canvas.resize(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_POINTERDOWN:
        g_capture->onPointerDown(wParam, lParam);
        return 0;

    case WM_POINTERUPDATE:
        g_capture->onPointerMove(wParam, lParam);
        return 0;

    case WM_POINTERUP:
        g_capture->onPointerUp(wParam, lParam);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    EnableMouseInPointer(TRUE);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"MathBoardWindow";
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        L"MathBoardWindow",
        L"MathBoard",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1400, 900,
        nullptr, nullptr,
        hInstance, nullptr
    );

    g_canvas.init(hwnd);
    g_capture = new mb::StrokeCapture(&g_canvas);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete g_capture;
    return (int)msg.wParam;
}