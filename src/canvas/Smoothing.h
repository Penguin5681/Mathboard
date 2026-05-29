#pragma once
#include <vector>
#include "../model/Types.h"

namespace mb
{
	struct CubicSegment
	{
		Point p0;
		Point cp1;
		Point cp2;
		Point p1;
	};

	std::vector<CubicSegment> catmullRomSegments(
		const std::vector<Point> &points,
		float tension = 0.5f
	);
}