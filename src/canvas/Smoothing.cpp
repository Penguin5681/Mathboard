#include "Smoothing.h"
#include <algorithm>

namespace mb
{
	static Point lerp(const Point& a, const Point& b, float t)
	{
		return Point{
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.pressure + (b.pressure - a.pressure) * t,
			a.t
		};
	}

	static Point add(const Point &a, const Point &b) 
	{
		return {a.x + b.x, a.y + b.y, a.pressure, a.t};
	}

	static Point sub(const Point& a, const Point& b)
	{
		return {a.x - b.x, a.y - b.y, a.pressure, a.t};
	}

	static Point scale(const Point& p, float scaleFactor)
	{
		return {p.x * scaleFactor, p.y * scaleFactor, p.pressure, p.t};
	}

	std::vector<CubicSegment> catmullRomSegments(
		const std::vector<Point>& pts,
		float tension
	)
	{
		std::vector<CubicSegment> result;

		if (pts.size() < 2)
		{
			return result;
		}

		if (pts.size() == 2)
		{
			result.push_back({pts[0], pts[0], pts[1], pts[1]});
			return result;
		}

		const size_t n = pts.size();

		std::vector<Point> p;
		p.reserve(n + 2);

		Point phantomStart = add(pts[0], sub(pts[0], pts[1]));
		Point phantomEnd = add(pts[n - 1], sub(pts[n - 1], pts[n - 2]));

		p.push_back(phantomStart);

		for (const auto& pt : pts)
		{
			p.push_back(pt);
		}

		p.push_back(phantomEnd);

		const float k = tension / 6.0f;

		for (size_t i = 0; i < n - 1; ++i)
		{
			const Point &p0 = p[i];
			const Point &p1 = p[i + 1];
			const Point &p2 = p[i + 2];
			const Point	&p3 = p[i + 3];

			Point cp1 = add(p1, scale(sub(p2, p0), k));
			Point cp2 = sub(p2, scale(sub(p3, p1), k));
			
			result.push_back({p1, cp1, cp2, p2});
		}

		return result;
	}
}