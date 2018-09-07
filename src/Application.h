//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
//#include "terrain.h"
#include "basemodel.h"
#include "truck.h"
#include "player.h"
#include "coin.h"
#include "Level.h"
#include "ShadowMapGenerator.h"

class Application
{
public:
	typedef std::list<BaseModel*> ModelList;

	Application(GLFWwindow* pWin);
	void start();
	void update(float dtime);
	void draw();
	void end();
	void resetLevel();
protected:
	void addCoin();
	void createBaseScene();
	void createForrestScene();
	void createForrestLighting();

	Camera Cam;
	ModelList Models;
	GLFWwindow* pWindow;
	ShadowMapGenerator ShadowGenerator;

	BaseModel* pModel;
	Player* player;
	Truck* truck;
	Coin* coin;

	std::list<Truck*> TruckPool;
	std::list<Coin*> CoinPool;
	std::list<Coin*> CollectedCoinsPool;

	bool isDodging;
	float dodgeCooldown;
	float difficultyChangeInterval;
	float difficultyIncreaseAmount;
	int maxDifficultyChanges;
	int currentDifficulty;
	int currentCollectedCoins;
	int stackableCoinsCount;
	int stackProgress[4];
};

#endif /* Application_hpp */
