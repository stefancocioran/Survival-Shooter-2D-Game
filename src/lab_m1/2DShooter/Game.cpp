#include "lab_m1/2DShooter/Game.h"

#include <vector>
#include <iostream>

#include "lab_m1/2DShooter/transform2D.h"
#include "lab_m1/2DShooter/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Game::Game()
{
}


Game::~Game()
{
}


void Game::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	width = window->GetResolution().x;
	height = window->GetResolution().y;

	float mapScale = 2.f;
	logicSpace.x = 0;		// logic x
	logicSpace.y = 0;		// logic y
	logicSpace.width = 160 * mapScale;	// logic width
	logicSpace.height = 90 * mapScale;	// logic height

	// Variables used for mesh sizes
	squareSide = 10.f;
	eyeSide = 2.5f;
	edgeSize = 0.75f;
	mapEdgeSize = 10;
	projectileSize = 2.f;
	obstacleSize = 10;
	eyeDistance = eyeSide / 2;
	eyeDistanceEnemy = -eyeSide / 2;
	healthbarLength = 15.f * mapScale;
	healthbarWidth = 3.5f * mapScale;
	safeDistance = 20.f;
	mapWidth = logicSpace.width;
	mapHeight = logicSpace.height;
	powerupSize = 10.f;
	logicSpaceMinimapX = logicSpace.x;
	logicSpaceMinimapY = logicSpace.y;


	circleDiameter = 10.f;
	cx = circleDiameter / 2;
	cy = circleDiameter / 2;
	cxEye = eyeSide / 2;
	cyEye = eyeSide / 2;

	fmouseX = 0;
	fmouseY = 0;

	nextFire = 0;
	projectileTravelDistance = 45;

	// Initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// Initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// Movement speed
	moveSpeed = 40.f;
	moveSpeedEnemy = 40.f;

	healthbars = 5;		// healthbar scale   
	lives = healthbars; // initial health

	killPoints = 25;
	spawnTimer = 3.f;
	printTimer = 5.f;
	powerupTimer = 5.f;

	fireCooldown = 1.f;
	rapidfireTimer = 0;
	powerupFireCooldown = 0.2f;
	normalFireCooldown = 1.f;
	rapidfireCountdown = 5.f;

	nextPrint = 0;
	nextPrint = 0;
	nextPowerup = powerupTimer;
	nextSpawn = 0;

	score = 0;
	markKill = false;
	printGO = true;
	fireProjectile = false;

	// Set initial coordinates 
	startPosX = logicSpace.width / 2 - cx;
	startPosY = logicSpace.height / 2 - cy;
	playerPosX = startPosX;
	playerPosY = startPosY;


	// Set obstacle location on map
	SetObstacles();

	polygonMode = GL_FILL;

	// Create map
	Mesh* map = object2D::Rectangle("map", glm::vec3(0), mapWidth, mapHeight, glm::vec3(0.678f, 0.854f, 1.f), true);
	AddMeshToList(map);

	// Create obstacle
	Mesh* obstacle = object2D::Square("obstacle", glm::vec3(0), obstacleSize, glm::vec3(0.094f, 0.592f, 0.054f), true);
	AddMeshToList(obstacle);

	// Create healthbar 
	Mesh* healthbarEdge = object2D::Rectangle("healthbarEdge", glm::vec3(0), healthbarLength, healthbarWidth, glm::vec3(0.79f, 0.011f, 0.011f));
	AddMeshToList(healthbarEdge);
	Mesh* healthbar = object2D::Rectangle("healthbar", glm::vec3(0), healthbarLength, healthbarWidth, glm::vec3(0.79f, 0.011f, 0.011f), true);
	AddMeshToList(healthbar);

	// Create projectile
	Mesh* projectile = object2D::Square("projectile", glm::vec3(0), projectileSize, glm::vec3(0, 0, 0), true);
	AddMeshToList(projectile);

	// Create health powerup
	Mesh* healthPowerup = object2D::Circle("healthPowerup", powerupSize / 2, powerupSize / 2, powerupSize / 2, glm::vec3(0.396f, 0.905f, 0.713f));
	AddMeshToList(healthPowerup);

	// Create rapid fire powerup
	Mesh* rapidfirePowerup = object2D::Circle("rapidfirePowerup", powerupSize / 2, powerupSize / 2, powerupSize / 2, glm::vec3(0.839f, 0.133f, 0.274f));
	Mesh* squarePowerupPurple = object2D::Square("squarePurple", glm::vec3(0), powerupSize / 4, glm::vec3(0.141f, 0.062f, 0.137f), true);
	AddMeshToList(rapidfirePowerup);
	AddMeshToList(squarePowerupPurple);

	// Create bonus score powerup
	Mesh* scorePowerup = object2D::Circle("scorePowerup", powerupSize / 2, powerupSize / 2, powerupSize / 2, glm::vec3(1.f, 0.984f, 0.121f));
	Mesh* squarePowerupGreen = object2D::Square("squareGreen", glm::vec3(0), powerupSize / 4, glm::vec3(0.278f, 0.627f, 0.145f), true);
	AddMeshToList(scorePowerup);
	AddMeshToList(squarePowerupGreen);

	// Create player circle
	Mesh* player = object2D::CircleWithEdge("player", cx, cy, circleDiameter / 2, glm::vec3(1.f, 0.819f, 0.4f));
	AddMeshToList(player);

	// Create enemy mesh
	{
		vector<VertexFormat> verticesEnemy
		{
			// body
			VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(0, squareSide,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(squareSide, 0,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(squareSide, squareSide,  0), glm::vec3(0.513f, 0.082f, 0.003f)),

			// left eye
			VertexFormat(glm::vec3(eyeDistanceEnemy, -eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(eyeDistanceEnemy, eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(eyeDistanceEnemy + eyeSide, -eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(eyeDistanceEnemy + eyeSide, eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),

			// right eye
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy, -eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy, eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy - eyeSide, -eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy - eyeSide, eyeSide / 2,  0), glm::vec3(0.513f, 0.082f, 0.003f)),

			// edge for body shape
			VertexFormat(glm::vec3(-edgeSize, -edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(-edgeSize, squareSide + edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(squareSide + edgeSize, -edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(squareSide + edgeSize, squareSide + edgeSize,  0), glm::vec3(0, 0, 0)),

			// edge for left eye
			VertexFormat(glm::vec3(eyeDistanceEnemy - edgeSize, -eyeSide / 2 - edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(eyeDistanceEnemy - edgeSize, eyeSide / 2 + edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(eyeDistanceEnemy + eyeSide + edgeSize, -eyeSide / 2 - edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(eyeDistanceEnemy + eyeSide + edgeSize, eyeSide / 2 + edgeSize,  0), glm::vec3(0, 0, 0)),

			// edge for right eye
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy + edgeSize, -eyeSide / 2 - edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy + edgeSize, eyeSide / 2 + edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy - eyeSide - edgeSize, -eyeSide / 2 - edgeSize,  0), glm::vec3(0, 0, 0)),
			VertexFormat(glm::vec3(squareSide - eyeDistanceEnemy - eyeSide - edgeSize, eyeSide / 2 + edgeSize,  0), glm::vec3(0, 0, 0))
		};

		vector<unsigned int> indicesEnemy =
		{
			4, 6, 5,
			5, 6, 7,
			8, 9, 11,
			8, 11, 10,
			16, 18, 17,
			17, 18, 19,
			20, 21, 23,
			20, 23, 22,
			0, 2, 1,
			1, 2, 3,
			12, 14, 13,
			13, 14, 15
		};

		meshes["enemy"] = new Mesh("enemy");
		meshes["enemy"]->InitFromData(verticesEnemy, indicesEnemy);
	}

	// Cross 
	{
		vector<VertexFormat> verticesCross
		{
			VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(0, powerupSize,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(powerupSize / 8, 0,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(powerupSize / 8, powerupSize, 0), glm::vec3(1)),

			VertexFormat(glm::vec3(-powerupSize / 2 + powerupSize / 8, powerupSize / 2 - powerupSize / 16,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(-powerupSize / 2 + powerupSize / 8, powerupSize / 2 + powerupSize / 16,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(powerupSize / 2, powerupSize / 2 - powerupSize / 16,  0), glm::vec3(1)),
			VertexFormat(glm::vec3(powerupSize / 2, powerupSize / 2 + powerupSize / 16, 0), glm::vec3(1)),
		};

		vector<unsigned int> indicesCross =
		{
			4, 6, 5,
			5, 6, 7,
			0, 2, 1,
			1, 2, 3
		};


		meshes["cross"] = new Mesh("cross");
		meshes["cross"]->InitFromData(verticesCross, indicesCross);
	}
}

void Game::SetObstacles() {

	float transX, transY;

	scaleX = 5.5f, scaleY = 1.5f;
	transX = 125, transY = 5;
	obstacle obst(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst);

	scaleX = 1.25f, scaleY = 1.25f;
	transX = 30, transY = 70;
	obstacle obst2(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst2);

	scaleX = 2.5f, scaleY = 2.5f;
	transX = 60, transY = 60;
	obstacle obst3(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst3);

	scaleX = 1.5f, scaleY = 5.5f;
	transX = 120, transY = 55;
	obstacle obst4(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst4);

	scaleX = 2.f, scaleY = 0.33f;
	transX = 15, transY = 15;
	obstacle obst5(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst5);

	scaleX = 0.33f, scaleY = 4.f;
	transX = 15, transY = 15;
	obstacle obst6(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst6);

	scaleX = 12.f, scaleY = 0.5f;
	transX = 175, transY = 45;
	obstacle obst7(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst7);


	scaleX = 12.f, scaleY = 1.5f;
	transX = 75, transY = 135;
	obstacle obst8(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst8);

	scaleX = 0.75f, scaleY = 3.25f;
	transX = 55, transY = 135;
	obstacle obst9(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst9);

	scaleX = 0.75f, scaleY = 1.25f;
	transX = 55, transY = 110;
	obstacle obst10(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst10);

	scaleX = 0.75f, scaleY = 7.5f;
	transX = 175, transY = 75;
	obstacle obst11(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst11);


	scaleX = 0.75f, scaleY = 7.5f;
	transX = 255, transY = 80;
	obstacle obst12(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst12);

	scaleX = 7.5f, scaleY = 0.75f;
	transX = 225, transY = 100;
	obstacle obst13(obstacleSize, scaleX, scaleY, transX, transY);
	obstacles.push_back(obst13);
}


void Game::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

// 2D visualization matrix
glm::mat3 Game::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}


void Game::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Game::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}


// Create random spawn coordinates for enemies
// each enemy is spawned in a quadrant different from the player's
// at a safe distance (to not be spawned ON/near the player) and not 
// on obstacles 
void Game::RandomSpawnCoordinates() {

	int minX = 0, minY = 0, maxX = mapWidth, maxY = mapHeight;
	int r;
	if (playerPosX < logicSpace.width / 2) {
		if (playerPosY < logicSpace.height / 2) {  // down left
			r = rand() % 2;
			switch (r) {
			case(0):
				minX = logicSpace.width / 2 + safeDistance;
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = squareSide * sqrt(2);
				maxY = logicSpace.height / 2;
				break;
			case(1):
				minX = squareSide * sqrt(2);
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = logicSpace.height / 2 + safeDistance;
				maxY = logicSpace.height - squareSide * sqrt(2);
				break;
			}
		}
		else if (playerPosY > logicSpace.height / 2) { // top left
			r = rand() % 2;
			switch (r) {
			case(0):
				minX = logicSpace.width / 2 + safeDistance;
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = logicSpace.height / 2;
				maxY = logicSpace.height - squareSide * sqrt(2);
				break;
			case(1):
				minX = squareSide * sqrt(2);
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = squareSide * sqrt(2);
				maxY = logicSpace.height / 2 - safeDistance;
				break;
			}
		}
	}
	else if (playerPosX > logicSpace.width / 2) {

		if (playerPosY < logicSpace.height / 2) { // down right
			r = rand() % 2;
			switch (r) {
			case(0):
				minX = squareSide * sqrt(2);
				maxX = logicSpace.width / 2 - safeDistance;

				minY = squareSide * sqrt(2);
				maxY = logicSpace.height / 2;
				break;
			case(1):

				minX = squareSide * sqrt(2);
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = logicSpace.height / 2 + safeDistance;
				maxY = logicSpace.height - squareSide * sqrt(2);
				break;
			}
		}
		else if (playerPosY > logicSpace.height / 2) { // top right
			r = rand() % 2;
			switch (r) {
			case(0):
				minX = squareSide * sqrt(2);
				maxX = logicSpace.width / 2 - safeDistance;

				minY = logicSpace.height / 2;
				maxY = logicSpace.height - squareSide * sqrt(2);
				break;
			case(1):
				minX = squareSide * sqrt(2);
				maxX = logicSpace.width - squareSide * sqrt(2);

				minY = squareSide * sqrt(2) / 2;
				maxY = logicSpace.height / 2 - safeDistance;
				break;
			}
		}
	}

	int rangeX = maxX - minX + 1;
	int rangeY = maxY - minY + 1;

	numX = (rand() % rangeX) + minX;
	numY = (rand() % rangeY) + minY;
}

bool Game::CircleMapCollision(float circleX, float circleY, float radius)
{
	if (circleX - radius <= 0 || circleX + radius >= mapWidth
		|| circleY - radius <= 0 || circleY + radius >= mapHeight) {
		return true;
	}
	else {
		return false;
	}
}

bool Game::CircleToRectangleCollision(float cx, float cy, float r, obstacle obst)
{
	float circleDistanceX = abs(cx - obst.x);
	float circleDistanceY = abs(cy - obst.y);

	if (circleDistanceX > (obst.width / 2 + r)) { return false; }
	if (circleDistanceY > (obst.height / 2 + r)) { return false; }

	if (circleDistanceX <= (obst.width / 2)) { return true; }
	if (circleDistanceY <= (obst.height / 2)) { return true; }

	float cornerDistance_sq = (circleDistanceX - obst.width / 2) * (circleDistanceX - obst.width / 2) +
		(circleDistanceY - obst.height / 2) * (circleDistanceY - obst.height / 2);

	return (cornerDistance_sq <= (r * r));
}



bool Game::CircleToCircleCollision(float cx1, float cy1, float r1, float cx2, float cy2, float r2) {
	float dx = (cx1 + r1) - (cx2 + r2);
	float dy = (cy1 + r1) - (cy2 + r2);
	float distance = sqrt(dx * dx + dy * dy);

	if (distance < r1 + r2) {
		// collision detected
		return true;
	}

	return false;
}

void Game::DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds, bool minimap)
{
	playerPosX = startPosX + translateX;
	playerPosY = startPosY + translateY;


	bool ok = true;
	float maxX = mapWidth - powerupSize;
	float maxY = mapHeight - powerupSize;
	int rangeX = maxX - powerupSize + 1;
	int rangeY = maxY - powerupSize + 1;

	while (ok && Engine::GetElapsedTime() > nextPowerup) {

		float numX = (rand() % rangeX + powerupSize);
		float numY = (rand() % rangeY + powerupSize);

		bool collided = false;
		for (obstacle& obstacle : obstacles) {
			if (CircleToRectangleCollision(numX, numY, powerupSize, obstacle)) {
				collided = true;
				break;
			}
		}

		if (!collided) {
			nextPowerup = Engine::GetElapsedTime() + powerupTimer;
			powerup powerup;
			powerup.x = numX;
			powerup.y = numY;
			powerup.type = rand() % 3;
			powerups.push_back(powerup);
			ok = !ok;
		}
	}

	if (!minimap) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= visMatrix *
			transform2D::Translate(logicSpace.width / 2 + logicSpace.width / 2 / 1.5f + translateX, +logicSpace.height / 2 + logicSpace.height / 2 / 1.5f + translateY) *
			transform2D::Scale(lives * 1 / healthbars, 1) *
			transform2D::Rotate(glm::pi<float>());
		RenderMesh2D(meshes["healthbar"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= visMatrix *
			transform2D::Translate(logicSpace.width / 2 + logicSpace.width / 2 / 1.5f - healthbarLength + translateX, +logicSpace.height / 2 + logicSpace.height / 2 / 1.5f - healthbarWidth + translateY);
		RenderMesh2D(meshes["healthbarEdge"], shaders["VertexColor"], modelMatrix);
	}

	glm::mat3 player = glm::mat3(1);
	player *= visMatrix *
		transform2D::Translate(playerPosX, playerPosY) *
		transform2D::Translate(cx, cy) *
		transform2D::Rotate(angleMouse) *
		transform2D::Translate(-cx, -cy);

	glm::mat3 leftEye = glm::mat3(1);
	leftEye *= player *
		transform2D::Translate(-cxEye + eyeDistance, -cyEye + eyeDistance) *
		transform2D::Scale(0.25f, 0.25f);

	glm::mat3 rightEye = glm::mat3(1);
	rightEye *= player *
		transform2D::Translate(cxEye + eyeDistance + cx, -cyEye + eyeDistance) *
		transform2D::Scale(0.25f, 0.25f);


	if (fireProjectile && Engine::GetElapsedTime() > nextFire)
	{
		nextFire = Engine::GetElapsedTime() + fireCooldown;

		projectile newProjectile;
		newProjectile.angle = angleMouse;
		newProjectile.offsetX = 0;
		newProjectile.offsetY = 0;
		newProjectile.translateX = translateX;
		newProjectile.translateY = translateY;
		newProjectile.x = playerPosX + cx;
		newProjectile.y = playerPosY + cy;

		projectiles.push_back(newProjectile);
	}

	fireProjectile = false;
	int count = 0;

	for (projectile& proj : projectiles) {
		proj.x += sin(proj.angle) * deltaTimeSeconds * moveSpeed; // actualize position
		proj.y -= cos(proj.angle) * deltaTimeSeconds * moveSpeed;
		proj.offsetX += sin(proj.angle) * deltaTimeSeconds * moveSpeed;
		proj.offsetY -= cos(proj.angle) * deltaTimeSeconds * moveSpeed;

		// Check if projectile collieded with map edge and if needs to be distroyed
		if (sqrt(proj.offsetX * proj.offsetX + proj.offsetY * proj.offsetY) > projectileTravelDistance ||
			CircleMapCollision(proj.x - sin(proj.angle) * deltaTimeSeconds * moveSpeed, proj.y + cos(proj.angle) * deltaTimeSeconds * moveSpeed, sqrt(2) * projectileSize / 2)) {
			projectiles.erase(projectiles.begin() + count);
			continue;
		}


		// Check enemy collision
		int enemyCount = 0;
		for (enemy& enemy : enemies) {
			if (CircleToCircleCollision(proj.x, proj.y, sqrt(2) * projectileSize / 2, enemy.x, enemy.y, sqrt(2) * squareSide / 2)) {
				projectiles.erase(projectiles.begin() + count);
				score += enemy.score;
				enemies.erase(enemies.begin() + enemyCount);
				count--;
				break;
			}
			enemyCount++;
		}


		// Check obstacle collision
		for (obstacle& obstacle : obstacles) {
			if (CircleToRectangleCollision(proj.x, proj.y, sqrt(2) * projectileSize / 2 - 0.5f, obstacle)) {
				projectiles.erase(projectiles.begin() + count);
				count--;
				break;
			}
		}

		count++;
	}

	RenderMesh2D(meshes["player"], shaders["VertexColor"], leftEye);
	RenderMesh2D(meshes["player"], shaders["VertexColor"], rightEye);
	RenderMesh2D(meshes["player"], shaders["VertexColor"], player);


	for (projectile& proj : projectiles) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= visMatrix *
			transform2D::Translate(startPosX + cx - projectileSize / 2 + proj.translateX + proj.offsetX, startPosY + cy - projectileSize / 2 + proj.translateY + proj.offsetY) *
			transform2D::Translate(projectileSize / 2, projectileSize / 2) *
			transform2D::Rotate(proj.angle) *
			transform2D::Translate(-projectileSize / 2, -projectileSize / 2);

		RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
	}


	if (Engine::GetElapsedTime() > nextSpawn) {
		nextSpawn = Engine::GetElapsedTime() + spawnTimer;

		RandomSpawnCoordinates();

		// If enemy moves faster, has a bigger score
		float speed = float(rand() % 7 + 4);
		float score = 400 / speed;

		enemy newEnemy;
		newEnemy.x = numX;
		newEnemy.y = numY;
		newEnemy.offsetX = 0;
		newEnemy.offsetY = 0;
		newEnemy.angle = glm::atan(newEnemy.y - playerPosY, newEnemy.x - playerPosX) - (glm::pi<float>() / 2);
		newEnemy.score = score;
		newEnemy.speed = speed;

		enemies.push_back(newEnemy);
	}

	count = 0;
	for (enemy& enemy : enemies) {
		float dirX = playerPosX - enemy.x;
		float dirY = playerPosY - enemy.y;

		float hyp = sqrt(dirX * dirX + dirY * dirY);
		dirX /= hyp;
		dirY /= hyp;


		// Enemy collision with player
		if (CircleToCircleCollision(enemy.x + dirX * deltaTimeSeconds, enemy.y + dirY * deltaTimeSeconds * enemy.speed, sqrt(2) * squareSide / 2,
			playerPosX, playerPosY, cx)) {
			enemies.erase(enemies.begin() + count);
			lives--;
			continue;
		}


		// Enemy collision with map
		if (enemy.x + dirX * deltaTimeSeconds * enemy.speed + squareSide / 2 - sqrt(2) * squareSide / 2 <= 0 ||
			enemy.x + dirX * deltaTimeSeconds * enemy.speed + squareSide / 2 + sqrt(2) * squareSide / 2 >= mapWidth) {
			enemy.y += dirY * deltaTimeSeconds * enemy.speed;
		}
		else if (enemy.y + dirY * deltaTimeSeconds * enemy.speed + squareSide / 2 - sqrt(2) * squareSide / 2 <= 0 ||
			enemy.y + dirY * deltaTimeSeconds * enemy.speed + squareSide / 2 + sqrt(2) * squareSide / 2 >= mapWidth) {
			enemy.x += dirX * deltaTimeSeconds * enemy.speed;
		}
		else {
			enemy.x += dirX * deltaTimeSeconds * enemy.speed;
			enemy.y += dirY * deltaTimeSeconds * enemy.speed;
		}

		enemy.angle = glm::atan(enemy.y - playerPosY, enemy.x - playerPosX) - (glm::pi<float>() / 2);

		modelMatrix = glm::mat3(1);
		modelMatrix *= visMatrix *
			transform2D::Translate(enemy.x + enemy.offsetX, enemy.y + enemy.offsetY) *
			transform2D::Translate(squareSide / 2, squareSide / 2) *
			transform2D::Rotate(enemy.angle) *
			transform2D::Translate(-squareSide / 2, -squareSide / 2);

		RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
		count++;
	}


	for (obstacle& obstacle : obstacles) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= visMatrix *
			transform2D::Translate(obstacle.transX, obstacle.transY) *
			transform2D::Scale(obstacle.scaleX, obstacle.scaleY);
		RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
	}

	count = 0;
	for (powerup& powerup : powerups) {
		if (CircleToCircleCollision(powerup.x, powerup.y, powerupSize / 2, playerPosX, playerPosY, circleDiameter / 2)) {
			// apply powerup
			switch (powerup.type) {
			case 0:
				// HEAL
				if (lives != healthbars)
					lives++;
				break;
			case 1:
				// INCREASED FIRE RATE
				rapidfireTimer = Engine::GetElapsedTime();
				nextFire = 0;
				fireCooldown = powerupFireCooldown;
				break;
			case 2:
				// BONUS SCORE
				score += 200;
			}
			powerups.erase(powerups.begin() + count);
			continue;
		}
		count++;
	}

	// Return to normal fire cooldown after powerup expires
	if (Engine::GetElapsedTime() > rapidfireTimer + rapidfireCountdown) {
		fireCooldown = normalFireCooldown;
	}

	for (powerup& powerup : powerups) {
		switch (powerup.type) {
		case 0:
			modelMatrix = visMatrix * transform2D::Translate(powerup.x + powerupSize / 2 - powerupSize / 16, powerup.y);
			RenderMesh2D(meshes["cross"], shaders["VertexColor"], modelMatrix);
			modelMatrix = modelMatrix * transform2D::Translate(-powerupSize / 2 + powerupSize / 16, 0);
			RenderMesh2D(meshes["healthPowerup"], shaders["VertexColor"], modelMatrix);
			break;
		case 1:
			modelMatrix = visMatrix * transform2D::Translate(powerup.x + powerupSize / 4 + powerupSize / 8, powerup.y + powerupSize / 4 + powerupSize / 8);
			RenderMesh2D(meshes["squarePurple"], shaders["VertexColor"], modelMatrix);
			modelMatrix = modelMatrix * transform2D::Translate(-powerupSize / 4 - powerupSize / 8, -powerupSize / 4 - powerupSize / 8);
			RenderMesh2D(meshes["rapidfirePowerup"], shaders["VertexColor"], modelMatrix);
			break;

		case 2:
			modelMatrix = visMatrix * transform2D::Translate(powerup.x + powerupSize / 4 + powerupSize / 8, powerup.y + powerupSize / 4 + powerupSize / 8);
			RenderMesh2D(meshes["squareGreen"], shaders["VertexColor"], modelMatrix);
			modelMatrix = modelMatrix * transform2D::Translate(-powerupSize / 4 - powerupSize / 8, -powerupSize / 4 - powerupSize / 8);
			RenderMesh2D(meshes["scorePowerup"], shaders["VertexColor"], modelMatrix);
			break;
		}
	}

	modelMatrix = visMatrix;
	RenderMesh2D(meshes["map"], shaders["VertexColor"], modelMatrix);
}


void Game::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	glm::ivec2 resolution = window->GetResolution();
	height = resolution.y;
	width = resolution.x;

	// Sets the screen area where to draw
	float viewSpaceWidth = resolution.x + resolution.x / 2, viewSpaceHeight = resolution.y + resolution.y / 2;
	viewSpace = ViewportSpace(-resolution.x / 4, -resolution.y / 4, viewSpaceWidth, viewSpaceHeight);

	SetViewportArea(viewSpace, glm::vec3(0.168f, 0.572f, 0.890f), true);

	// Compute the 2D visualization matrix
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);


	DrawScene(visMatrix, deltaTimeSeconds, false);

	// The viewport is now the minimap
	viewSpace = ViewportSpace(0, 0, viewSpaceWidth / 8, viewSpaceHeight / 8);
	SetViewportArea(viewSpace, glm::vec3(0), true);

	/// Compute uniform 2D visualization matrix
	visMatrix = glm::mat3(1);
	float tmpX = logicSpace.x;
	float tmpY = logicSpace.y;
	// Offsets used for crating minmap border
	logicSpace.x = logicSpaceMinimapX - 1.75f;
	logicSpace.y = logicSpaceMinimapY - 1.75f;
	logicSpace.width += 1.75f;
	logicSpace.height += 2.75f;

	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
	DrawScene(visMatrix, deltaTimeSeconds, true);

	logicSpace.x = tmpX;
	logicSpace.y = tmpY;
	logicSpace.width -= 1.75f;
	logicSpace.height -= 2.75f;


	if (lives == 0) {
		if (printGO) {
			printf("\n"
				"   #######     #    #     # #######      ####### #     # ####### ######\n"
				"   #     #    # #   ##   ## #            #     # #     # #       #     #\n"
				"   #         #   #  # # # # #            #     # #     # #       #     #\n"
				"   #  ####  #     # #  #  # #####        #     # #     # #####   ###### \n"
				"   #     #  ####### #     # #            #     #  #   #  #       #   #  \n"
				"   #     #  #     # #     # #            #     #   # #   #       #    # \n"
				"   #######  #     # #     # #######      #######    #    ####### #     #\n\n");
			printGO = !printGO;
			printf("FINAL SCORE: %d\n", score);
			window->Close();
		}
	}
	else {
		if (Engine::GetElapsedTime() > nextPrint) {
			nextPrint = Engine::GetElapsedTime() + printTimer;
			printf("\nSCORE: %d\n", score);
		}
	}
}


void Game::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Game::OnInputUpdate(float deltaTime, int mods)
{
	// Check player collision with map and obstacles
	if (window->KeyHold(GLFW_KEY_W)) {
		if (startPosY + translateY + circleDiameter + deltaTime * moveSpeed < mapHeight) {
			bool ok = true;
			for (obstacle& obst : obstacles) {
				if (CircleToRectangleCollision(playerPosX + cx, playerPosY + deltaTime * moveSpeed + cy, cx, obst)) {
					ok = false;
				}
			}
			if (ok) {
				logicSpace.y += deltaTime * moveSpeed;
				translateY += deltaTime * moveSpeed;
			}
		}
	}
	else if (window->KeyHold(GLFW_KEY_S)) {
		if (startPosY + translateY - deltaTime * moveSpeed > 0) {
			bool ok = true;
			for (obstacle& obst : obstacles) {
				if (CircleToRectangleCollision(playerPosX + cx, playerPosY - deltaTime * moveSpeed + cy, cx, obst)) {
					ok = false;
				}
			}
			if (ok) {
				logicSpace.y -= deltaTime * moveSpeed;
				translateY -= deltaTime * moveSpeed;
			}
		}
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		if (startPosX + translateX - deltaTime * moveSpeed > 0) {
			bool ok = true;
			for (obstacle& obst : obstacles) {
				if (CircleToRectangleCollision(playerPosX - deltaTime * moveSpeed + cx, playerPosY + cy, cx, obst)) {
					ok = false;
				}
			}
			if (ok) {
				logicSpace.x -= deltaTime * moveSpeed;
				translateX -= deltaTime * moveSpeed;
			}
		}
	}
	else if (window->KeyHold(GLFW_KEY_D)) {
		if (startPosX + translateX + circleDiameter + deltaTime * moveSpeed < mapWidth) {

			bool ok = true;
			for (obstacle& obst : obstacles) {
				if (CircleToRectangleCollision(playerPosX + deltaTime * moveSpeed + cx, playerPosY + cy, cx, obst)) {
					ok = false;
				}
			}
			if (ok) {
				logicSpace.x += deltaTime * moveSpeed;
				translateX += deltaTime * moveSpeed;
			}
		}
	}
}


void Game::OnKeyPress(int key, int mods)
{
	// Add key press event

	if (key == GLFW_KEY_SPACE)
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
}


void Game::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Game::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
	fmouseX = mouseX, fmouseY = mouseY;
	fmouseY = window->GetResolution().y - fmouseY;
	fmouseY /= window->GetResolution().y / logicSpace.height;
	fmouseX /= window->GetResolution().x / logicSpace.width;

	angleMouse = glm::atan(startPosY - fmouseY + cy, startPosX - fmouseX + cy) - (glm::pi<float>() / 2);
}


void Game::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
	if (button == GLFW_MOUSE_BUTTON_2) {
		fireProjectile = true;
	}

}


void Game::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Game::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Game::OnWindowResize(int width, int height)
{
}
