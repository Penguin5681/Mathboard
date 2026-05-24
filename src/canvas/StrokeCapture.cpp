#include "StrokeCapture.h"
#include "Canvas.h"
#include <Windows.h>
#include <windowsx.h>
#include <chrono>
#include <sstream>
#include <iomanip>

#ifndef GET_POINTERID_WPARAM
#define GET_POINTERID_WPARAM(wParam) (LOWORD(wParam))
#endif

namespace mb
{
	StrokeCapture::StrokeCapture(Canvas *canvas) : m_canvas(canvas) {}

	Point StrokeCapture::makePoint(HWND hwnd, LPARAM lParam, float pressure) const
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hwnd, &pt);

		Point p = m_canvas->screenToWorld((float)pt.x, (float)pt.y);
		p.pressure = pressure;
		p.t = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();

		return p;
	}

	std::string StrokeCapture::makeId() const
	{
		auto now = std::chrono::steady_clock::now().time_since_epoch().count();
		std::ostringstream ss;
		ss << "s_" << std::hex << now;
		return ss.str();
	}

	void StrokeCapture::onPointerDown(WPARAM wParam, LPARAM lParam)
	{
		UINT pointerId = GET_POINTERID_WPARAM(wParam);

		float pressure = 1.0f;
		POINTER_PEN_INFO penInfo = {};
		if (GetPointerPenInfo(pointerId, &penInfo))
		{
			pressure = penInfo.pressure / 1024.0f;
		}

		m_current = Stroke{};
		m_current.id = makeId();
		m_current.style.color = 0xFF000000;
		m_current.style.width = 3.0f;
		m_current.createdAtTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();

		m_current.points.push_back(makePoint(m_canvas->hwnd(), lParam, pressure));

		m_drawing = true;

		m_canvas->setLiveStroke(m_current);
		InvalidateRect(m_canvas->hwnd(), nullptr, FALSE);
	}

	void StrokeCapture::onPointerMove(WPARAM wParam, LPARAM lParam)
	{
		if (!m_drawing)
		{
			return;
		}

		UINT pointerId = GET_POINTERID_WPARAM(wParam);

		float pressure = 1.0f;
		POINTER_PEN_INFO penInfo = {};
		if (GetPointerPenInfo(pointerId, &penInfo))
		{
			pressure = penInfo.pressure / 1024.0f;
		}

		m_current.points.push_back(makePoint(m_canvas->hwnd(), lParam, pressure));
		m_canvas->setLiveStroke(m_current);
		InvalidateRect(m_canvas->hwnd(), nullptr, FALSE);
	}

	void StrokeCapture::onPointerUp(WPARAM wParam, LPARAM lParam)
	{
		if (!m_drawing)
		{
			return;
		}

		m_drawing = false;

		if (m_current.points.size() >= 2)
		{
			m_canvas->addStroke(m_current);
		}

		m_canvas->clearLiveStroke();
		InvalidateRect(m_canvas->hwnd(), nullptr, FALSE);
	}
}