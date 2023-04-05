#include "Ship.h"

void Shot::Draw(float angle)
{
	spriteList[frameNumber]->angle = angle;
	spriteList[frameNumber]->Blit(position.x, position.y);
}

bool Shot::Update(float seconds)
{
	timeToLive -= seconds;

	//if shot is past designed lifetime, return false

	if (frameNumber == 0 && timeToLive < timeToLiveMaxValue * 4 / 5) frameNumber++;
	if (frameNumber == 1 && timeToLive < timeToLiveMaxValue * 3 / 5) frameNumber++;
	if (frameNumber == 2 && timeToLive < timeToLiveMaxValue * 2 / 5) frameNumber++;
	if (frameNumber == 3 && timeToLive < timeToLiveMaxValue * 1 / 5) frameNumber++;
	;
	if (timeToLive < 0) return false;

	//move the shot
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	return true;
}

void Ship::Draw()
{
	//apply angle!
	spriteList[frameNumber]->angle = angle;
	//draw main ship sprite
	spriteList[frameNumber]->Blit(position.x, position.y);

	//redraw if too close to an edge
	//left
	if (position.x < radius + 10.f) spriteList[frameNumber]->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius + 10.f) spriteList[frameNumber]->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x, position.y - 1080.f);

	//copies for 4 diagonal corners
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y - 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y - 1080.f);
}

void Ship::Update(float seconds, float sinceLastKill)
{
	if (sinceLastKill < 4) {
		shotTimerMaxValue = 0.35;
		if (sinceLastKill < 3) {
			shotTimerMaxValue = 0.3;
			if (sinceLastKill < 2) {
				shotTimerMaxValue = 0.2;
			}
		}
	}
	else if (sinceLastKill >= 3) {
		shotTimerMaxValue = 0.25;
	}
	else if (sinceLastKill >= 4) {
		shotTimerMaxValue = 0.35;
	}
	else if (sinceLastKill >= 6) {
		shotTimerMaxValue = 0.4;
	}

	if (hitTimer >= 0) {
		frameNumber = 0;
		hitTimer -= seconds;
	}
	else {
		//handle turning
		if (turningLeft)
		{
			angle += 140.f * seconds;
		}

		if (turningRight)
		{
			angle -= 140.f * seconds;
		}

		if (thrusting)
		{
			//calculate facing vector
			float radians = angle * (M_PI / 180);
			glm::vec2 facing;
			facing.x = std::cos(radians);
			facing.y = std::sin(radians);

			facing *= seconds * 400.f;

			velocity += facing;
			thrustTimer += seconds;

			if (glm::length(velocity) > 600.0)
				velocity = glm::normalize(velocity) * 600.f;

			//printf_s("\n\t%f\t-\t%i", glm::length(velocity), velocity.length());
			//animation timing
			if (thrustTimer >= 0.8f / 20.f && glm::length(velocity) < 300.0) {
				//change frames
				frameNumber--;
				if (glm::length(velocity) > 400.0) {
					if (frameNumber > 20 || frameNumber < 7)
						frameNumber = 20;
				}
				else {
					if (frameNumber > 20 || frameNumber < 7)
						frameNumber = 11;
				}

				thrustTimer -= 0.8f / 20.f;
			}
			else if (thrustTimer >= 1.f / 20.f && glm::length(velocity) > 300.0) {
				frameNumber--;
				if (glm::length(velocity) > 400.0) {
					if (frameNumber > 20 || frameNumber < 7)
						frameNumber = 20;
				}
				else {
					if (frameNumber > 20 || frameNumber < 7)
						frameNumber = 11;
				}

				thrustTimer -= 1.f / 20.f;
			}
		}
		else if (glm::length(velocity) > 300.0) {
			thrustTimer += seconds;
			if (thrustTimer >= 2.f / 20.f) {
				frameNumber++;
				thrustTimer -= 2.f / 20.f;
				if (frameNumber > 6 || frameNumber < 1)
					frameNumber = 1;
			}
		}
		else frameNumber = 1;
	}

	if (lives <= 0)
		frameNumber = 1;
	//update position
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	velocity *= scale;

	//velocity could potentialy get very small: we should
	//cap it to zero when it gets really close,
	//but that is a more advanced topic that
	//requires techniques like std::isnan()
	float length = glm::length(velocity);
	if (std::isnan(length) || length < 0.00001f)
		velocity = glm::vec2(0, 0);

	//handle shot timer
	shotTimer -= seconds;
}

bool Ship::Shoot()
{
	if (shotTimer > 0) return false;

	//reset shot timer
	shotTimer = shotTimerMaxValue;

	//calculating the angle of the ship to shoot in the direction we're looking
	float radians = angle * (M_PI / 180);
	glm::vec2 facing;
	facing.x = std::cos(radians);
	facing.y = std::sin(radians);

	//making a new shot
	//set the shot data using ship's variables
	Shot shot;
	shot.spriteList = shotSpriteList;
	shot.velocity = facing * 1200.f + velocity;
	shot.position = position + facing * 30.f;
	shot.angle = angle;
	//add the shot to the shotList
	shotList.push_back(shot);

	return true;
}