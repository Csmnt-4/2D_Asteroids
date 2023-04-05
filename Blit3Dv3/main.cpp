//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include "Ship.h"
#include "Asteroid.h"
#include "DieRoller.h"
#include "Narrative.h"

Blit3D* blit3D = NULL;

//GLOBAL DATA
double elapsedTime = 0;
double elapsedTimeDummy = 0;
double totalTime = 0;
double score = 0;
int visibleScore = 0;
int asteroidsDestroyed = 0;
int dialog = 0;
int extra = 0;
int bubble = 0;
float timeSlice = 1.f / 120.f;
float spawnTimer;
float sinceLastKillTime = 0;
boolean dialogPause = true;
boolean asteroidPause = true;
int gameStage = 1;

Sprite* backgroundSprite = NULL; //a pointer to a background sprite
Sprite* heartSprite = NULL;
Ship* ship = NULL;

std::vector<std::tuple<glm::vec2, float, int, AsteroidSize>> explosionPositionsList;

std::vector<Sprite*> explosionSheet;
std::vector<Sprite*> spriteList;
std::vector<std::vector<Sprite*>> talkList;

std::vector<Asteroid> asteroidList;

AngelcodeFont* spaceFont = NULL;

void Init()
{
	//these is a font. which i had to borrow, because none of mine work =(
	spaceFont = blit3D->MakeAngelcodeFontFromBinary32("Media\\Font\\CaviarDreams57.bin");

	//lives sprite
	heartSprite = blit3D->MakeSprite(0, 0, 32, 32, "Media\\Heart\\Silverheart_Asteroids.png");

	//these are speech bubble sprites
	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 21; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 585, 180, "Media\\SpeachBubbles\\BubbleBzz0.png"));
	}
	talkList.push_back(spriteList);

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 17; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 477, 180, "Media\\SpeachBubbles\\BubbleBzz1.png"));
	}
	talkList.push_back(spriteList);

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 17; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 585, 180, "Media\\SpeachBubbles\\BubbleBzz2.png"));
	}
	talkList.push_back(spriteList);

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 39; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 873, 180, "Media\\SpeachBubbles\\BubbleIntro0.png"));
	}
	talkList.push_back(spriteList);

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 40; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 837, 180, "Media\\SpeachBubbles\\BubbleIntro1.png"));
	}
	talkList.push_back(spriteList);

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 35; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 180, 801, 180, "Media\\SpeachBubbles\\BubbleCleared.png"));
	}
	talkList.push_back(spriteList);

	explosionSheet = std::vector<Sprite*>();
	for (int i = 0; i < 7; i++) {
		explosionSheet.push_back(blit3D->MakeSprite(i * 240, 0, 240, 320, "Media\\Explosion\\explosionSpritesheet.png"));
	}

	spriteList = std::vector<Sprite*>();
	for (int i = 0; i < 8; ++i) {
		spriteList.push_back(blit3D->MakeSprite(0, i * 1080, 1920, 1080, "Media\\front.png"));
	}

	InitPerturb();

	//these are asteroid sprites
	for (int i = 0; i < 2; ++i) {
		asteroidSpriteList.push_back(blit3D->MakeSprite(i * 50, 0, 50, 40, "Media\\Asteroids\\asteroidSize1_1.png"));
		asteroidSpriteList.push_back(blit3D->MakeSprite(i * 70, 0, 70, 70, "Media\\Asteroids\\asteroidSize2_1.png"));
		asteroidSpriteList.push_back(blit3D->MakeSprite(i * 150, 0, 150, 100, "Media\\Asteroids\\asteroidSize3_0.png"));
	}

	//turn cursor off
	blit3D->ShowCursor(false);

	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\back.png");

	//create a ship
	ship = new Ship;
	//load a sprite off of a spritesheet
	for (int i = 0; i < 21; ++i) {
		ship->spriteList.push_back(blit3D->MakeSprite(0, i * 70, 150, 70, "Media\\Ship\\SpritesheetSpaceship.png"));
	}
	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);

	//load the shot graphic
	for (int i = 0; i < 5; ++i)
		ship->shotSpriteList.push_back(blit3D->MakeSprite(i * 8, 0, 8, 8, "Media\\Ship\\shotsGreen.png"));

	//set the clear colour
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//clear colour: r,g,b,a
}

void DeInit(void)
{
	if (ship != NULL) delete ship;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	//only update time to a maximun amount - prevents big jumps in
	//the simulation if the computer "hiccups"
	if (seconds < 0.15) {
		if (ship->lives > 0)
			score += seconds;
		elapsedTime += seconds;
		elapsedTimeDummy += seconds;
		if (sinceLastKillTime < 10)
			sinceLastKillTime += seconds;
	}
	else {
		elapsedTime += 0.15;
		elapsedTimeDummy += 0.15;
		sinceLastKillTime += 0;
	}

	//update by a full timeslice when it's time
	while (elapsedTime >= timeSlice)
	{
		elapsedTime -= timeSlice;

		CollideAsteroids(asteroidList);
		CollideAsteroidsWithShip(seconds, ship, asteroidList);
		ship->Update(timeSlice, sinceLastKillTime);
		if (ship->shooting) ship->Shoot();
		for (int i = ship->shotList.size() - 1; i >= 0; --i)
		{
			//shot Update() returns false when the bullet should be killed off
			if (!ship->shotList[i].Update(timeSlice))
				ship->shotList.erase(ship->shotList.begin() + i);
		}

		if (!asteroidPause)
			if (asteroidsDestroyed >= 10)
			{
				if (asteroidList.size() <= asteroidsDestroyed / gameStage) {
					RainAsteroids(seconds, spawnTimer, 2.0, 10.0 - timeSlice, ship->position, asteroidList, gameStage);
				}
				else {
					asteroidPause = true;
				}
			}
			else if (asteroidList.size() <= 12) {
				RainAsteroids(seconds, spawnTimer, 2.0, 10.0, ship->position, asteroidList, gameStage);
			}

		if (asteroidList.empty() || asteroidList.size() <= 5) {
			asteroidPause = false;
		}

		if (asteroidsDestroyed >= 10 * gameStage) {
			gameStage++;
			asteroidPause = true;
			dialogPause = true;
		}

		std::vector<int> deleteList(asteroidList.size());
		std::vector<int> createList(asteroidList.size());
		for (auto& d : deleteList) {
			d = -1;
		}
		for (auto& c : createList) {
			c = -1;
		}

		int aPos = 0;
		for (auto& as : asteroidList) {
			int i = as.Update(seconds, explosionPositionsList, ship->shotList);
			if (i == -1) {
				deleteList.push_back(aPos);
				createList.push_back(aPos);
				if (sinceLastKillTime <= 5)
					score += ((int)as.size + 1) * 20;
				else
					score += ((int)as.size + 1) * 10;
				asteroidsDestroyed++;
				sinceLastKillTime -= 5;
			}
			else if (i == 0) {
				spawnTimer = 0.f;
				deleteList.push_back(aPos);
			}
			aPos++;
		}

		applyExplosionImpulse(asteroidList, explosionPositionsList);

		//delete bad asteroids!
		for (auto& c : createList) {
			if (c != -1) {
				CreateAsteroid(asteroidList, asteroidList.at(c).position + glm::normalize(asteroidList.at(c).position) * 30.f, asteroidList.at(c).size, 0.2 + atan2((asteroidList.at(c).position - ship->position).y, (asteroidList.at(c).position - ship->position).x));
				CreateAsteroid(asteroidList, asteroidList.at(c).position - glm::normalize(asteroidList.at(c).position) * 40.f, asteroidList.at(c).size, -0.2 + atan2((asteroidList.at(c).position - ship->position).y, (asteroidList.at(c).position - ship->position).x));
				c = -1;
			}
		}

		for (auto& d : deleteList) {
			if (d != -1) {
				if (asteroidList.size() >= d + 1)
					asteroidList.erase(asteroidList.begin() + d);
				d = -1;
			}
		}

		if (ship->lives <= 0) {
			ship->shooting = false;

			if (visibleScore <= score * 10) {
				visibleScore++;
				if (visibleScore <= score * 9.5) {
					visibleScore += 10;
					if (visibleScore <= score * 9) {
						visibleScore += 10;
						if (visibleScore <= score * 8)
							visibleScore += 10;
					}
				}
			}
		}

		//delete bad explosions!
		//
		//uh... so.... something went wrong, so I'm not deleting them for the time being
		//
		// if (explosionPositionsList.size() >= 1)
		//	for (int i = explosionPositionsList.size(); i >= 0; i--)
		//		if (std::get<2>(explosionPositionsList.at(i)) == 6)
		//			explosionPositionsList.erase(explosionPositionsList.begin() + i);
	}
}

void Draw(void)
{
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here

	//draw the background in the middle of the screen
	//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at,
	//starting as 0,0 in the bottom-left corner.
	if (ship->lives > 0) {
		backgroundSprite->Blit(1920.f / 2, 1080.f / 2);

		//draw the asteroids
		for (auto& as : asteroidList) as.Draw();

		//draw the explosions
		for (auto& ex : explosionPositionsList) {
			if (std::get<2>(ex) <= 6) {
				DrawExplosion(ex, explosionSheet);
				auto newTuple = std::make_tuple(std::get<0>(ex), std::get<1>(ex), std::get<2>(ex) + 1, std::get<3>(ex));
				ex.swap(newTuple);
			}
		}

		//draw the shots
		for (auto& s : ship->shotList) s.Draw(s.angle);

		//draw the ship
		ship->Draw();

		//draw the ship lives
		for (int i = 1; i <= ship->lives; i++) {
			heartSprite->Blit(1920.f / 1.25 + i * 48, 1080.f / 1.1);
		}

		//draw the indication of the difficulty going up
		if (dialogPause) {
			bubbleSequence(dialogPause, asteroidPause, elapsedTimeDummy, timeSlice, ship->position, talkList, dialog, bubble);
		}
	}
	else {
		spaceFont->BlitText(1920.f / 4, 1080.f / 2, "Game Over            Your score: " + std::to_string(visibleScore));
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (ship->lives > 0) {
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
			ship->turningLeft = true;

		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			ship->turningLeft = false;

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
			ship->turningRight = true;

		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			ship->turningRight = false;

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
			ship->thrusting = true;

		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			ship->thrusting = false;

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			ship->shooting = true;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		ship->shooting = false;
}

int main(int argc, char* argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}