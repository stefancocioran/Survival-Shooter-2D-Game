#pragma once

#include "utils/glm_utils.h"


namespace transform2D
{
	// Translate matrix
	inline glm::mat3 Translate(float translateX, float translateY)
	{
		// translation matrix
		return glm::mat3(
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			translateX, translateY, 1
		);

	}

	// Scale matrix
	inline glm::mat3 Scale(float scaleX, float scaleY)
	{
		// scaling matrix
		return glm::mat3(
			scaleX, 0.f, 0.f,
			0.f, scaleY, 0.f,
			0.f, 0.f, 1.f
		);

	}

	// Rotate matrix
	inline glm::mat3 Rotate(float radians)
	{
		// rotation matrix
		return glm::mat3(
			cos(radians), sin(radians), 0.f,
			-sin(radians), cos(radians), 0.f,
			0.f, 0.f, 1.f
		);

	}
}   // namespace transform2D
