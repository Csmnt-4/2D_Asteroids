#include "Blit3D.h"
#include "Narrative.h"
#include "Asteroid.h"

void bubbleSequence(boolean& dialogPause, boolean& asteroidPause, double& elapsedTimeForDialog, float timeSlice, glm::vec2 position, std::vector<std::vector<Sprite*>> bubbles, int& talkNumber, int& bubble) {
	if (elapsedTimeForDialog >= timeSlice * 5) {
		bubble++;
		elapsedTimeForDialog -= timeSlice * 5;
	}
	switch (talkNumber) {
	case 0: {
		if (!bubbleTalk(position, bubbles[0], talkNumber, bubble))
		{
			talkNumber = 3;
			bubble = 0;
		}
		break;
	}
	case 3: {
		if (!bubbleTalk(position, bubbles[3], talkNumber, bubble))
		{
			asteroidPause = false;
			talkNumber = 4;
			bubble = 0;
		}
		break;
	}
	case 4: {
		if (!bubbleTalk(position, bubbles[4], talkNumber, bubble))
		{
			talkNumber = 2;
			bubble = 0;
		}
		break;
	}
	case 2: {
		if (!bubbleTalk(position, bubbles[2], talkNumber, bubble))
		{
			talkNumber = 1;
			bubble = 0;
			dialogPause = false;
		}
		break;
	}
	case 1: {
		if (!bubbleTalk(position, bubbles[1], talkNumber, bubble))
		{
			talkNumber = 5;
			bubble = 0;
		}
		break;
	}
	case 5: {
		if (!bubbleTalk(position, bubbles[5], talkNumber, bubble))
		{
			talkNumber = 2;
			bubble = 0;
		}
		break;
	}
	default:
		break;
	}
}

boolean bubbleTalk(glm::vec2 position, std::vector<Sprite*> bubbles, int& talkNumber, int& bubble) {
	position += glm::vec2(180.f, 120.f);
	switch (talkNumber) {
	case 0: {
		if (bubble < 21)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 51)
			bubbles[20]->Blit(position.x, position.y);

		else
			return false;
		return true;
	}
	case 1: {
		if (bubble < 17)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 47)
			bubbles[16]->Blit(position.x, position.y);
		else
			return false;
		return true;
	}
	case 2: {
		if (bubble < 17)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 47)
			bubbles[16]->Blit(position.x, position.y);
		else
			return false;
		return true;
	}
	case 3: {
		if (bubble < 39)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 59)
			bubbles[38]->Blit(position.x, position.y);
		else
			return false;
		return true;
	}
	case 4: {
		if (bubble < 40)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 60)
			bubbles[39]->Blit(position.x, position.y);
		else
			return false;
		return true;
	}
	case 5: {
		if (bubble < 35)
			bubbles[bubble]->Blit(position.x, position.y);
		else if (bubble < 75)
			bubbles[34]->Blit(position.x, position.y);
		else
			return false;
		return true;
	}
	default: {
		return false;
	}
	}
	return true;
}