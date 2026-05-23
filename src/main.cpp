#define UNICODE
#define _UNICODE

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

#pragma comment (lib, "d2d1")
#pragma comment (lib, "dwrite")

using Microsoft::WRL::ComPtr;

ComPtr<ID2D1Factory> g_d2dFactory;
ComPtr<ID2D1HwndRenderTarget> g_renderTarget;
ComPtr<ID2D1SolidColorBrush> g_brush;

HRESULT CreateD2DResources(HWND hwnd)
{
	HRESULT hr = S_OK;

	if (!g_renderTarget) 
	{
		if (!g_d2dFactory) {
			hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				g_d2dFactory.GetAddressOf()
			);
			if (FAILED(hr)) return hr;
		}

		RECT rc;
		GetClientRect(hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = g_d2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			g_renderTarget.GetAddressOf()
		);
		if (FAILED(hr)) return hr;

		hr = g_renderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			g_brush.GetAddressOf()
		);
	}

	return hr;
}

void DiscardD2DResources()
{
	g_brush.Reset();
	g_renderTarget.Reset();
}

void OnRender(HWND hwnd)
{
	HRESULT hr = CreateD2DResources(hwnd);
	if (FAILED(hr))
	{
		return;
	}

	g_renderTarget->BeginDraw();

	g_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	RECT rc;
	GetClientRect(hwnd, &rc);

	float cx = (rc.right - rc.left) / 2.0f;
	float cy = (rc.bottom - rc.top) / 2.0f;

	g_brush->SetColor(D2D1::ColorF(D2D1::ColorF::YellowGreen));
	g_renderTarget->DrawEllipse(
		D2D1::Ellipse(D2D1::Point2F(cx, cy), 40.0f, 40.0f),
		g_brush.Get(),
		2.0f
	);

	hr = g_renderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardD2DResources();
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		{
			OnRender(hwnd);
			ValidateRect(hwnd, nullptr);
			return 0;
		}

		case WM_SIZE:
		{
			if (g_renderTarget)
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				g_renderTarget->Resize(D2D1::SizeU(width, height));
			}

			InvalidateRect(hwnd, nullptr, FALSE);
			return 0;
		}

		case WM_DESTROY:
		{
			DiscardD2DResources();
			g_d2dFactory.Reset();
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCommandShow)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wc.hbrBackground = nullptr;
	wc.lpszClassName = L"Mathboard Window";

	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		0, 
		L"Mathboard Window",
		L"Mathboard",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1400, 900,
		nullptr, nullptr,
		hInstance, nullptr
	);

	ShowWindow(hwnd, nCommandShow);
	UpdateWindow(hwnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}