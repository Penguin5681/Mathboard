#include "Canvas.h"
#include "Smoothing.h"
#include <d2d1helper.h>

namespace mb
{
	HRESULT Canvas::init(HWND hwnd)
	{
		m_hwnd = hwnd;
		return createResource();
	}

	HRESULT Canvas::createResource()
	{
		if (m_renderTarget)
		{
			return S_OK;
		}

		HRESULT hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			m_factory.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return hr;
		}

		RECT rc;
		GetClientRect(m_hwnd, &rc);

		hr = m_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				m_hwnd,
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
			),
			m_renderTarget.GetAddressOf()
		);

		if (FAILED(hr))
		{
			return hr;
		}

		hr = m_renderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			m_brush.GetAddressOf()
		);

		return hr;
	}

	void Canvas::discardResource()
	{
		m_brush.Reset();
		m_renderTarget.Reset();
	}

	void Canvas::resize(UINT width, UINT height)
	{
		if (m_renderTarget)
		{
			m_renderTarget->Resize(D2D1::SizeU(width, height));
		}
	}

	void Canvas::render()
	{
		if (FAILED(createResource()))
		{
			return;
		}

		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		for (const auto& stroke : m_strokes)
		{
			drawStroke(stroke);
		}

		if (m_hasLive)
		{
			drawStroke(m_liveStroke);
		}

		HRESULT hr = m_renderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			discardResource();
		}
	}

	void Canvas::drawStroke(const Stroke& stroke)
	{
		if (stroke.points.size() < 2) return;

		float r = ((stroke.style.color >> 16) & 0xFF) / 255.0f;
		float g = ((stroke.style.color >> 8) & 0xFF) / 255.0f;
		float b = ((stroke.style.color >> 0) & 0xFF) / 255.0f;
		m_brush->SetColor(D2D1::ColorF(r, g, b));

		auto segments = catmullRomSegments(stroke.points, 0.3f);
		if (segments.empty()) return;

		Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
		m_factory->CreatePathGeometry(path.GetAddressOf());

		Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
		path->Open(sink.GetAddressOf());

		sink->BeginFigure(
			D2D1::Point2F(segments[0].p0.x, segments[0].p0.y),
			D2D1_FIGURE_BEGIN_HOLLOW
		);

		for (const auto& seg : segments)
		{
			sink->AddBezier(D2D1::BezierSegment(
				D2D1::Point2F(seg.cp1.x, seg.cp1.y),
				D2D1::Point2F(seg.cp2.x, seg.cp2.y),
				D2D1::Point2F(seg.p1.x, seg.p1.y)
			));
		}

		sink->EndFigure(D2D1_FIGURE_END_OPEN);
		sink->Close();

		float totalPressure = 0.0f;
		for (const auto& pt : stroke.points)
			totalPressure += pt.pressure;
		float avgPressure = totalPressure / stroke.points.size();
		float width = stroke.style.width * (0.4f + avgPressure * 0.6f);

		m_renderTarget->DrawGeometry(path.Get(), m_brush.Get(), width);
	}

	void Canvas::addStroke(const Stroke& stroke)
	{
		m_strokes.push_back(stroke);
		m_hasLive = false;
	}

	void Canvas::setLiveStroke(const Stroke& stroke)
	{
		m_liveStroke = stroke;
		m_hasLive = true;
	}

	void Canvas::clearLiveStroke()
	{
		m_hasLive = false;
	}

	Point Canvas::screenToWorld(float sx, float sy) const
	{
		return Point{
			sx, sy, 1.0f, 0
		};
	}
}