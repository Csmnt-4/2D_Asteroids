#pragma once
#include<Blit3D.h>

class Shot
{
public:
	glm::vec2 velocity, position;
	std::vector<Sprite*> spriteList;
	int frameNumber = 0;
	float angle = 0;
	float timeToLive = 2.f; //shots live for 2 seconds
	float timeToLiveMaxValue = 2.f; //shots live for 2 seconds
	void Draw(float angle);
	bool Update(float seconds); //return false if shot dead (timeToLive <= 0)
};

class Ship
{
public:
	glm::vec2 velocity, position;
	std::vector<Sprite*> spriteList;
	float angle = 0;
	float radius = 27.f;

	int lives = 5;
	float hitTimer = 0;

	int frameNumber = 11;

	float thrustTimer = 0;
	bool thrusting = false;
	bool shining = false;
	bool turningLeft = false;
	bool turningRight = false;

	std::vector<Shot> shotList;
	std::vector<Sprite*> shotSpriteList;
	bool shooting = false;
	float shotTimer = 0;
	//value is still hardcoded, but not in the .cpp file
	float shotTimerMaxValue = 0.4;

	void Draw();
	void Update(float seconds, float sinceLastKill);
	bool Shoot();
};