#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace mb
{
	struct Point
	{
		float	x, y;
		float	pressure; // idk how to test this since i do not have any pres sensitive pen
		int64_t	t; // the milisecond time stamp
	};

	struct StrokeStyle
	{
		uint32_t color = 0xFF000000;
		float width = 3.0f;
	};

	struct Stroke
	{
		std::string id;
		std::vector<Point> points;
		StrokeStyle style;
		int64_t createdAtTime = 0;
	};
} 