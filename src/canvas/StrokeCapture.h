#pragma once
#include <Windows.h>
#include <functional>
#include "../model/Types.h"

namespace mb
{
	class Canvas;

	class StrokeCapture
	{
		public:
			explicit StrokeCapture(Canvas *canvas);

			void onPointerDown(WPARAM wParam, LPARAM lParam);
			void onPointerMove(WPARAM wParam, LPARAM lParam);
			void onPointerUp(WPARAM wParam, LPARAM lParam);

		private:
			Point makePoint(HWND hwnd, LPARAM lParam, float pressure) const;
			std::string makeId() const;

			Canvas *m_canvas;
			Stroke m_current;
			bool m_drawing = false;
	};
}