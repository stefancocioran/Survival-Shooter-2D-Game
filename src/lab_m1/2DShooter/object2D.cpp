#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::Square(
	const std::string& name,
	glm::vec3 leftBottomCorner,
	float length,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}



Mesh* object2D::Rectangle(
	const std::string& name,
	glm::vec3 leftBottomCorner,
	float width, float height,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;


	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		rectangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}


Mesh* object2D::CircleWithEdge(
	const std::string& name,
	float cx, float cy,
	float radius,
	glm::vec3 color)
{
	Mesh* circle = new Mesh(name);

	int i;
	int triangleAmount = 30; //# of triangles used to draw circle

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(cx, cy,  0), color)
	};

	std::vector<unsigned int> indices = { 0 };

	int count = 0;

	// Body
	for (i = 0; i <= triangleAmount; i++) {
		vertices.push_back(VertexFormat(glm::vec3(
			cx + (radius * cos(i * 2 * glm::pi<float>() / triangleAmount)),
			cy + (radius * sin(i * 2 * glm::pi<float>() / triangleAmount)), 0), color));
		indices.push_back(i + 1);
	}

	count += triangleAmount;

	// Body edge
	indices.push_back(count);
	radius += 0.5f;

	for (i = count; i <= count + triangleAmount + 1; i++) {
		vertices.push_back(VertexFormat(glm::vec3(
			cx + (radius * cos(i * 2 * glm::pi<float>() / triangleAmount)),
			cy + (radius * sin(i * 2 * glm::pi<float>() / triangleAmount)), 0), glm::vec3(0)));
		indices.push_back(i + 1);
	}

	circle->SetDrawMode(GL_TRIANGLE_FAN);
	circle->InitFromData(vertices, indices);
	return circle;
}



Mesh* object2D::Circle(
	const std::string& name,
	float cx, float cy,
	float radius,
	glm::vec3 color)
{
	Mesh* circle = new Mesh(name);

	int i;
	int triangleAmount = 30; //# of triangles used to draw circle

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(cx, cy,  0), color)
	};

	std::vector<unsigned int> indices = { 0 };

	for (i = 0; i <= triangleAmount; i++) {
		vertices.push_back(VertexFormat(glm::vec3(
			cx + (radius * cos(i * 2 * glm::pi<float>() / triangleAmount)),
			cy + (radius * sin(i * 2 * glm::pi<float>() / triangleAmount)), 0), color));
		indices.push_back(i + 1);
	}

	circle->SetDrawMode(GL_TRIANGLE_FAN);
	circle->InitFromData(vertices, indices);
	return circle;
}