#include "Asteroid.h"

#include<random>
#include "DieRoller.h"

std::mt19937 rng;
std::uniform_real_distribution<float> distPerturb(-0.25f, 0.25f);

std::vector<Sprite*> asteroidSpriteList; //creating a new global variable

float baseVelocity = 200.f;

extern Blit3D* blit3D; //linking to a global variable var from another file

Asteroid::Asteroid(AsteroidSize size, glm::vec2 position, glm::vec2 velocity, float rotationSpeed, float angle) {
	DiceRoller die = DiceRoller();
	this->size = size;
	this->lives = (int)this->size;
	switch (size) {
	case AsteroidSize::LARGE: {
		this->radius = 80;
		this->height = 150;
		this->width = 100;
		this->lives = die.Roll4D6DropLowest();
		break;
	}
	case AsteroidSize::MEDIUM: {
		this->radius = 40;
		this->height = 40;
		this->width = 40;
		this->lives = die.Roll3D6() - die.Roll1DN(3);
		break;
	}
	case AsteroidSize::SMALL: {
		this->radius = 30;
		this->height = 30;
		this->width = 40;
		this->lives = die.Roll1DN(3) + die.Roll1DN(3);
		break;
	}
	}

	this->position = position;
	this->velocity = velocity;
	this->rotationSpeed = rotationSpeed;
	this->angle = angle;
}

int Asteroid::Update(float seconds, std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>>& explosionPositionsList, std::vector<Shot>& shotList)
{
	for (Shot& s : shotList)
		CollideAsteroidWithShot(explosionPositionsList, s);

	position += velocity * seconds;
	angle += rotationSpeed * seconds;
	if (hitTimer > 0)
		hitTimer -= seconds;
	if (lives <= 0) {
		return -1;
	}

	if (onTheScreen) {
		if (position.x < 0) position.x += blit3D->screenWidth;
		if (position.x > blit3D->screenWidth) position.x -= blit3D->screenWidth;
		if (position.y < 0) position.y += blit3D->screenHeight;
		if (position.y > blit3D->screenHeight) position.y -= blit3D->screenHeight;
	}
	else if (position.x > radius && position.x < blit3D->screenWidth - radius && position.y > radius && position.y < blit3D->screenHeight - radius) {
		onTheScreen = true;
	}
	else {
		offScreenTimeToLive -= seconds;
		if ((offScreenTimeToLive <= 0) && !(position.x > (-radius * 2) && position.x < (blit3D->screenWidth + radius * 2) && position.y >(-radius * 2) && position.y < (blit3D->screenHeight + radius * 2))) {
			return 0;
		}
	}
	return 1;
}

void Asteroid::Draw()
{
	if (hitTimer > 0.01f) {
		asteroidSpriteList[(int)size + 3]->angle = angle;
		asteroidSpriteList[(int)size + 3]->Blit(position.x, position.y);
	}
	else {
		asteroidSpriteList[(int)size]->angle = angle;
		asteroidSpriteList[(int)size]->Blit(position.x, position.y);
	}

	if (onTheScreen) {
		//redraw if too close to an edge
		//left
		if (position.x < radius + 100.f) asteroidSpriteList[(int)size]->Blit(position.x + 1920.f, position.y);
		//right
		if (position.x > 1920.f - (radius + 100.f)) asteroidSpriteList[(int)size]->Blit(position.x - 1920.f, position.y);
		//down
		if (position.y < radius + 100.f) asteroidSpriteList[(int)size]->Blit(position.x, position.y + 1080.f);
		//up
		if (position.y > 1080.f - (radius + 100.f)) asteroidSpriteList[(int)size]->Blit(position.x, position.y - 1080.f);
	}
}

void CreateAsteroid(std::vector<Asteroid>& asteroidList, glm::vec2 position, AsteroidSize size, float angle) {
	if ((int)size > 0) {
		DiceRoller die = DiceRoller();
		glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * ((float)die.Roll1DN(50) + (float)die.Roll1DN(100));
		float rotationSpeed = (float)(die.Roll1DN(2) - die.Roll1DN(2)) * (float)size * 0.1;
		printf("\n\t\t%f - %f - %f", position.x, position.y, angle);

		angle = die.Roll1DN(360);
		Asteroid newAsteroid = Asteroid(AsteroidSize((int)size - 1), position, velocity, rotationSpeed, angle);
		asteroidList.push_back(newAsteroid);
	}
}

void RainAsteroids(float seconds, float& spawnTimer, float spawnTimerMinValue, float spawnTimerMaxValue, glm::vec2 shipPosition, std::vector<Asteroid>& asteroidList, int stage) {
	if (spawnTimer <= 0) {
		DiceRoller die = DiceRoller();
		AsteroidSize size = AsteroidSize(die.Roll1DN(3) - 1);

		float x = (float)(die.Roll1DN(5760) - 2880);
		float y = (float)(die.Roll1DN(3240) - 1620);

		while (x <= 2200.f && x >= 0.f && y <= 1200.f && y >= 0.f) {
			x = (float)(die.Roll1DN(5760) - 2880);
			y = (float)(die.Roll1DN(3240) - 1620);

			if (x <= 2200.f && x >= 0.f && y <= 1200.f && y >= 0.f)
				switch (die.Roll1DN(3)) {
				case 1: {
					x += (float)(2100.f * ((-1) ^ die.Roll1DN(2)));
					break;
				}
				case 2: {
					y += (float)(1800.f * ((-1) ^ die.Roll1DN(2)));
					break;
				}
				case 3:
				default:
					x += (float)(2100.f * ((-1) ^ die.Roll1DN(2)));
					y += (float)(1800.f * ((-1) ^ die.Roll1DN(2)));
					break;
				}
		}
		printf("\n\t%f, %f", x, y);

		glm::vec2 position = glm::vec2(x, y);

		float xVel = (float)(die.Roll1DN(200) * ((-1) ^ die.Roll1DN(2)));
		float yVel = (float)(die.Roll1DN(200) * ((-1) ^ die.Roll1DN(2)));

		glm::vec2 velocity = glm::normalize(shipPosition - position) * ((float)die.Roll1DN(100) + 50.f + (float)die.Roll1DN(stage) * die.Roll1DN(stage));

		float rotationSpeed = (float)(die.Roll1DN(2) - die.Roll1DN(2)) * (float)size * 0.1;
		float angle = die.Roll1DN(360);
		Asteroid newAsteroid = Asteroid(size, position, velocity, rotationSpeed, angle);
		asteroidList.push_back(newAsteroid);
		spawnTimer = die.Roll1DN((int)spawnTimerMaxValue - spawnTimerMinValue - stage / 2) + spawnTimerMinValue;
	}
	else {
		spawnTimer -= seconds;
	}
}

bool Asteroid::CollideAsteroidWithShot(std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>>& explosionPositionsList, Shot& s)
{
	glm::vec2 delta = position - s.position;

	float distance = pow(delta.x, 2) + pow(delta.y, 2);
	if (distance <= pow(radius, 2)) {				// check for collision
		lives--;
		s.timeToLive = 0;
		if (lives == 0)
			explosionPositionsList.push_back({ position, -180 / 3.14 * atan2((position - s.position).x, (position - s.position).y), 0, size });
		else {
			hitTimer = 0.3;
			asteroidSpriteList[(int)size + 3]->Blit(position.x, position.y);
		}
		return true;
	}
	return false;
}

void CollideAsteroids(std::vector<Asteroid>& asteroids) {
	for (size_t i = 0; i < asteroids.size(); ++i) {
		for (size_t j = i + 1; j < asteroids.size(); ++j) {
			float minDistX = asteroids[i].width / 2.0f + asteroids[j].width / 2.0f;
			float minDistY = asteroids[i].height / 2.0f + asteroids[j].height / 2.0f;
			const glm::vec2 delta = asteroids[j].position - asteroids[i].position;
			if (glm::abs(delta.x) < minDistX && glm::abs(delta.y) < minDistY) {
				// Collision detected, objects[i] and objects[j] are overlapping
				if (asteroids[i].lives > 1)
					asteroids[i].lives--;
				if (asteroids[j].lives > 1)
					asteroids[j].lives--;
				asteroidSpriteList[(int)asteroids[i].size + 3]->Blit(asteroids[i].position.x, asteroids[j].position.y);
				asteroidSpriteList[(int)asteroids[j].size + 3]->Blit(asteroids[j].position.x, asteroids[j].position.y);
				// Calculate the unit normal and tangent vectors
				const glm::vec2 normalized = glm::normalize(delta);
				const glm::vec2 tangent = glm::vec2(-normalized.y, normalized.x);

				// Calculate the velocities along the normal and tangent vectors
				const float v1n = glm::dot(asteroids[i].velocity, normalized);
				const float v1t = glm::dot(asteroids[i].velocity, tangent);
				const float v2n = glm::dot(asteroids[j].velocity, normalized);
				const float v2t = glm::dot(asteroids[j].velocity, tangent);

				if (v2n - v1n < 0) {
					// Calculate the new velocities along the normal vector
					const float m1 = asteroids[i].radius * asteroids[i].radius;
					const float m2 = asteroids[j].radius * asteroids[j].radius;
					const float M = m1 + m2;
					const float v1n_new = 0.7f * (v1n * (m1 - m2) + 2 * m2 * v2n) / M;
					const float v2n_new = 0.7f * (v2n * (m2 - m1) + 2 * m1 * v1n) / M;

					// Update the velocities of the objects
					asteroids[i].velocity = v1n_new * normalized + v1t * tangent;
					asteroids[j].velocity = v2n_new * normalized + v2t * tangent;
				}
			}
		}
	}
}

void CollideAsteroidsWithShip(float seconds, Ship* s, std::vector<Asteroid>& asteroids) {
	for (size_t i = 0; i < asteroids.size(); ++i) {
		float minDistX = asteroids[i].width / 2.0f + 150 / 2.0f;
		float minDistY = asteroids[i].height / 2.0f + 70 / 2.0f;
		const glm::vec2 delta = asteroids[i].position - s->position;
		if (glm::abs(delta.x) < minDistX && glm::abs(delta.y) < minDistY) {
			// Collision detected, objects[i] and objects[j] are overlapping

			// Calculate the unit normal and tangent vectors
			const glm::vec2 normalized = glm::normalize(delta);
			const glm::vec2 tangent = glm::vec2(-normalized.y, normalized.x);

			// Calculate the velocities along the normal and tangent vectors
			const float v1n = glm::dot(asteroids[i].velocity, normalized);
			const float v1t = glm::dot(asteroids[i].velocity, tangent);
			const float v2n = glm::dot(s->velocity, normalized);
			const float v2t = glm::dot(s->velocity, tangent);

			if (v1n - v2n < 0) {
				// Calculate the new velocities along the normal vector
				const float m1 = asteroids[i].radius * asteroids[i].radius;
				const float m2 = s->radius * s->radius;
				const float M = m1 + m2;
				const float v1n_new = 0.7f * (v1n * (m1 - m2) + 2 * m2 * v2n) / M;
				const float v2n_new = 0.7f * (v2n * (m2 - m1) + 2 * m1 * v1n) / M;

				// Update the velocities of the objects
				asteroids[i].velocity = v1n_new * normalized + v1t * tangent;
				s->velocity = v2n_new * normalized + v2t * tangent;
				s->lives--;
				s->frameNumber = 0;
				s->hitTimer = 0.1;
				s->spriteList[0]->Blit(s->position.x, s->position.y);
			}
		}
	}
}

glm::vec2 Perturb(glm::vec2 V)
{
	float length = glm::length(V);
	float angle = atan2f(V.y, V.x);
	angle += distPerturb(rng);

	V.x = cos(angle);
	V.y = sin(angle);

	V *= length;
	return V;
}

void InitPerturb()
{
	std::random_device rd;
	rng.seed(rd());
}

void applyExplosionImpulse(std::vector<Asteroid>& asteroids, std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>> explosionPositionsList) {
	for (int i = 0; i < asteroids.size(); ++i) {
		for (int j = 0; j < explosionPositionsList.size(); ++j) {
			if (std::get<2>(explosionPositionsList.at(j)) == 4) {
				float distance = glm::length(asteroids.at(i).position - std::get<0>(explosionPositionsList.at(j)));
				if (distance < asteroids.at(i).radius + 200 && distance > asteroids.at(i).radius * 1.5) {
					float distanceFactor = 1.0f - (distance / asteroids.at(i).radius * 2); // ranges from 0 to 1
					float impulseFactor = 0.0f;
					if ((int)std::get<3>(explosionPositionsList.at(j)) == 2) {
						impulseFactor = 30.0f;
					}
					else if ((int)std::get<3>(explosionPositionsList.at(j)) == 1) {
						impulseFactor = 20.0f;
					}
					else if ((int)std::get<3>(explosionPositionsList.at(j)) == 0) {
						impulseFactor = 10.0f;
					}
					asteroids.at(i).velocity -= glm::normalize(asteroids.at(i).position - std::get<0>(explosionPositionsList.at(j))) * impulseFactor * distanceFactor;
				}
			}
		}
	}
}

void DrawExplosion(std::tuple<glm::vec2, float, int, AsteroidSize>pointAngleSprite, std::vector<Sprite*> spriteList) {
	Sprite* explosion = spriteList[std::get<2>(pointAngleSprite)];
	explosion->angle = std::get<1>(pointAngleSprite);

	explosion->Blit(std::get<0>(pointAngleSprite).x, std::get<0>(pointAngleSprite).y);
	//redraw if too close to an edge
	//left
	if (std::get<0>(pointAngleSprite).x < 200.f) explosion->Blit(std::get<0>(pointAngleSprite).x + 1920.f, std::get<0>(pointAngleSprite).y);
	//right
	if (std::get<0>(pointAngleSprite).x > 1920.f - 200.f) explosion->Blit(std::get<0>(pointAngleSprite).x - 1920.f, std::get<0>(pointAngleSprite).y);
	//down
	if (std::get<0>(pointAngleSprite).y < 200.f) explosion->Blit(std::get<0>(pointAngleSprite).x, std::get<0>(pointAngleSprite).y + 1080.f);
	//up
	if (std::get<0>(pointAngleSprite).y > 1080.f - 200.f) explosion->Blit(std::get<0>(pointAngleSprite).x, std::get<0>(pointAngleSprite).y - 1080.f);
}