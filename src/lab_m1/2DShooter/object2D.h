#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{

	// Create square with given bottom left corner, length and color
	Mesh* Square(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* Rectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
	Mesh* CircleWithEdge(const std::string& name, float cx, float cy, float radius, glm::vec3 color);
	Mesh* Circle(const std::string& name, float cx, float cy, float radius, glm::vec3 color);
}
