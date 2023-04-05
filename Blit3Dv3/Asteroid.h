#pragma once

#include "Blit3D.h"
#include "Ship.h"
#include <vector>

//include Shot.h / Ship.h as needed

extern std::vector<Sprite*> asteroidSpriteList;

enum class AsteroidSize { SMALL = 0, MEDIUM, LARGE };

extern float baseVelocity;

class Asteroid
{
public:
	glm::vec2 position, velocity;
	float radius = 0.f; //could be radiusSquared
	float width = 0.f;
	float height = 0.f;

	//for rotating the asteroid
	float angle = 0.f;
	float rotationSpeed = 0.f;
	float hitTimer = 0.f;
	float offScreenTimeToLive = 15.f;

	AsteroidSize size; //how big this asteroid is
	int lives;	//how much hits an asteroid needs

	int Update(float seconds, std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>>& explosionPositionsList, std::vector<Shot>& shotList);
	void Draw();

	bool CollideAsteroidWithShot(std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>>& explosionPositionsList, Shot& shot);

	// For making the asteroids "appear" from the curtains
	boolean onTheScreen = false;

	Asteroid(AsteroidSize size, glm::vec2 position, glm::vec2 velocity, float rotationSpeed, float angle);
};

void applyExplosionImpulse(std::vector<Asteroid>& asteroidList, std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>> explosionsList);
void CreateAsteroid(std::vector<Asteroid>& asteroidList, glm::vec2 position, AsteroidSize size, float angle);
void RainAsteroids(float seconds, float& spawnTimer, float spawnTimerMinValue, float spawnTimerMaxValue, glm::vec2 shipPosition, std::vector<Asteroid>& asteroidList, int stage);
void CollideAsteroids(std::vector<Asteroid>& asteroids);
void CollideAsteroidsWithShip(float seconds, Ship* s, std::vector<Asteroid>& asteroids);

void DrawExplosion(std::tuple<glm::vec2, float, int, AsteroidSize>explosion, std::vector<Sprite*> spriteList);
void InitPerturb();
glm::vec2 Perturb(glm::vec2 V);