#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <wrl/client.h>
#include <vector>
#include "../model/Types.h"

namespace mb
{
	class Canvas
	{
		public:
			HRESULT init (HWND hwnd);
			void resize(UINT width, UINT height);

			void render();

			void addStroke(const Stroke &stroke);

			void setLiveStroke(const Stroke &stroke);
			
			void clearLiveStroke();

			Point screenToWorld(float sx, float sy) const;

			HWND hwnd() const
			{
				return m_hwnd;
			}

		private:
			HRESULT createResource();
			void discardResource();
			void drawStroke(const Stroke &stroke);

			HWND m_hwnd = nullptr;

			Microsoft::WRL::ComPtr<ID2D1Factory> m_factory;
			Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_renderTarget;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;

			std::vector<Stroke> m_strokes;
			Stroke m_liveStroke;
			bool m_hasLive = false;
	};
}

