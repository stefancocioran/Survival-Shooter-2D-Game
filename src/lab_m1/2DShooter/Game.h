#pragma once

#include "components/simple_scene.h"
#include <vector>
using namespace std;

namespace m1
{
	class Game : public gfxc::SimpleScene
	{

	public:
		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height) {}
			float x;
			float y;
			float width;
			float height;
		};

		struct projectile
		{
			float x, y;
			float offsetX, offsetY;
			float angle;
			float translateX, translateY;
		};

		struct enemy
		{
			float x, y;
			float offsetX, offsetY;
			float angle;
			float translateX, translateY;
			float speed;
			int score;
		};

		struct powerup
		{
			int type;
			float x, y;
		};

		struct obstacle
		{
			float x, y;
			float width;
			float height;
			float transX, transY;
			float scaleX, scaleY;

			obstacle(float size, float scaleX, float scaleY, float transX, float transY) {
				this->x = size / 2 * scaleX + transX;
				this->y = size / 2 * scaleY + transY;
				this->width = size * scaleX;
				this->height = size * scaleY;
				this->transX = transX;
				this->transY = transY;
				this->scaleX = scaleX;
				this->scaleY = scaleY;
			}
		};

	public:
		Game();
		~Game();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;


		void DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds, bool minimap);
		void SetObstacles();
		void RandomSpawnCoordinates();
		bool CircleMapCollision(float circleX, float circleY, float radius);
		bool CircleToCircleCollision(float cx1, float cy1, float r1, float cx2, float cy2, float r2);
		bool CircleToRectangleCollision(float cx1, float cy1, float r, obstacle obst);

		// Sets the logic space and view space
		// logicSpace: { x, y, width, height }
		// viewSpace: { x, y, width, height }
		glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

	protected:
		float cx, cy;
		float cxEnemy, cyEnemy;
		float cxEye, cyEye;

		glm::mat3 modelMatrix, visMatrix;

		float translateX, translateY;
		float scaleX, scaleY;
		float angleMouse;

		GLenum polygonMode;
		ViewportSpace viewSpace;
		LogicSpace logicSpace;

		vector<projectile> projectiles;
		vector<enemy> enemies;
		vector<obstacle> obstacles;
		vector<powerup> powerups;

		float width, height;
		float startPosX, startPosY;
		float playerPosX, playerPosY;
		float fmouseX, fmouseY;
		float circleDiameter;
		float numX, numY; // coordinates of a randomly spawned enemy

		float moveSpeed;				// player and camera movement speed
		float moveSpeedEnemy;			// enemy movement speed
		float projectileTravelDistance; // indicates how far will projectile be fired until its destroyed
		float eyeDistance, eyeDistanceEnemy;
		float safeDistance; // enemies will not spawn in this radius

		float squareSide;
		float eyeSide;
		float edgeSize;
		float mapEdgeSize;
		float projectileSize;
		float obstacleSize;
		float radius;
		float mapWidth, mapHeight;
		float powerupSize;
		float logicSpaceMinimapX;
		float logicSpaceMinimapY;

		int score;
		int killPoints;
		float healthbars, healthbarLength, healthbarWidth;
		float lives;

		bool fireProjectile;
		bool markKill;
		bool printGO;

		// Spawn timers and cooldowns
		double nextFire;
		double nextPrint;
		double fireCooldown;
		double normalFireCooldown;
		double powerupFireCooldown;
		double printTimer;
		double spawnTimer;
		double nextSpawn;
		double nextPowerup;
		double powerupTimer;
		double rapidfireTimer;
		double rapidfireCountdown;
	};
}   // namespace m1
