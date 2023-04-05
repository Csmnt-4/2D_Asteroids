#pragma once
#include<Blit3D.h>
#include <vector>

void InitializeBubbles(Blit3D* blit3D, std::vector<std::vector<Sprite*>> talkList);
void bubbleSequence(boolean& dialogPause, boolean& asteroidPause, double& elapsedTimeForDialog, float timeSlice, glm::vec2 position, std::vector<std::vector<Sprite*>> bubbles, int& talkNumber, int& bubble);
boolean bubbleTalk(glm::vec2 position, std::vector<Sprite*> bubbles, int& talkNumber, int& bubble);