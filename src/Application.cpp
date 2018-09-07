//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "terrainshader.h"
#include "ShaderLightmapper.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

// ----------------------------------------------------------------------
// Adjustable settings
// ----------------------------------------------------------------------
#define CURRENT_DIFFICULTY 1
#define MAX_DIFFICULTY 20
#define INCREASE_PER_DIFFICULTY 0.1f
#define DIFFICULTY_CHANGE_INTERVAL 10.0f

#define INITIAL_TRUCK_SPEED 4.0f
#define INITIAL_COIN_SPEED 3.0f
// ----------------------------------------------------------------------

// irrKlang sound engine added to project to provide audio to the game
// source: https://www.ambiera.com/irrklang/index.html
#include <irrKlang.h>
using namespace irrklang;
ISoundEngine* soundEngine;

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048)
{
	// additional player controlled values
	this->isDodging = false;
	this->dodgeCooldown = 2.f;

	// limit difficulty changes so trucks don't drive at the speed of sound
	this->maxDifficultyChanges = MAX_DIFFICULTY;
	this->currentDifficulty = CURRENT_DIFFICULTY;

	// difficulty update interval in seconds
	this->difficultyChangeInterval = DIFFICULTY_CHANGE_INTERVAL;

	// amount by which the difficulty (speed of trucks) will be adjusted
	this->difficultyIncreaseAmount = INCREASE_PER_DIFFICULTY;

	// collected coins counter
	this->currentCollectedCoins = 0;
	this->stackableCoinsCount = 20;

	// set all stack progress to 0
	this->stackProgress[0] = 0;
	this->stackProgress[1] = 0;
	this->stackProgress[2] = 0;
	this->stackProgress[3] = 0;
	// ----------------------------------------------------------------------

	// randomize seed for truck light colors and truck/coin spawn
	srand(static_cast<unsigned>(time(0)));

	createBaseScene();
	createForrestLighting();
	createForrestScene();

	// create irrKlang sound engine
	soundEngine = createIrrKlangDevice();

	// check for errors
	if (!soundEngine)
		return;
}

void Application::start()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// start moving all trucks
	for (auto const& i : this->TruckPool)
	{
		i->start(INITIAL_TRUCK_SPEED);
	}

	// start moving all coins
	for (auto const& i : this->CoinPool)
	{
		i->start(INITIAL_COIN_SPEED);
	}

	// set volume and start playing track on repeat
	soundEngine->setSoundVolume(0.25f);
	soundEngine->play2D(ASSET_DIRECTORY "audio/bgm_02.mp3", true);
}

void Application::update(float dtime)
{
	float leftRight = 0.f;

	if (glfwGetKey(this->pWindow, GLFW_KEY_A) == GLFW_PRESS)
		leftRight = 1.f;
	else if (glfwGetKey(this->pWindow, GLFW_KEY_D) == GLFW_PRESS)
		leftRight = -1.f;

	// -------------------------------------------------------------
	// check for player dodge command
	// -------------------------------------------------------------

	if (this->dodgeCooldown >= 0.f && this->dodgeCooldown <= 1.f)
	{
		this->dodgeCooldown += dtime;
	}
	else if (this->dodgeCooldown > 1.f && this->dodgeCooldown < 2.f)
	{
		this->isDodging = false;
		this->dodgeCooldown += dtime;
	}
	else if (this->dodgeCooldown > 2.01f)
		this->dodgeCooldown = 2.f;

	// player movement actions
	if (this->dodgeCooldown >= 2.f && glfwGetKey(this->pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		this->dodgeCooldown = 0.f;
		this->isDodging = true;
	}

	// -------------------------------------------------------------
	
	//// get mouse position
	//double xPos, yPos;
	//glfwGetCursorPos(this->pWindow, &xPos, &yPos);

	//Vector cameraPos = this->Cam.position();
	//Vector direction = this->calc3DRay(xPos, yPos, cameraPos);

	//// from triangleIntersection()
	//Vector normal(0, 1, 0);
	//float scalar = 0.f;
	//scalar = (0 - normal.dot(cameraPos)) / normal.dot(direction);
	//Vector target = cameraPos + direction * scalar;

	//this->mPlayer->aim(target);
	this->player->move(leftRight);
	float camPosition = this->player->update(dtime, this->isDodging);

	// set player position for camera movement
	this->Cam.setPlayerPosition(camPosition);

	bool isCoin = false;

	// update all trucks
	for (auto const& i : this->TruckPool)
	{
		i->update(dtime);

		// check player collision with truck
		if (this->player->checkCollision(i->getModel(), isCoin))
			this->resetLevel();

		// update speed of trucks according to difficulty increase
		if (this->difficultyChangeInterval <= 0.f)
		{
			i->changeSpeedModifier(this->difficultyIncreaseAmount);
		}
	}

	// update all coins
	for (auto const& i : this->CoinPool)
	{
		i->update(dtime);

		// check player collision with truck
		if (this->player->checkCollision(i->getModel(), isCoin))
			i->reset(i->getCoinNumber());

		if (isCoin)
			this->addCoin();
	}

	// reset difficulty update interval
	if (this->difficultyChangeInterval <= 0.f)
	{
		this->difficultyChangeInterval = 1.f;
		this->currentDifficulty++;
	}

	// update time until difficulty changes, if max difficulty is not reached yet
	if (this->currentDifficulty < this->maxDifficultyChanges)
	{
		this->difficultyChangeInterval -= dtime;
	}

	Cam.update();
}

void Application::draw()
{
	ShadowGenerator.generate(Models);

	// 1. clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderLightMapper::instance().activate();
	// 2. setup shaders and draw models
	for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
	{
		(*it)->draw(Cam);
	}
	ShaderLightMapper::instance().deactivate();

	// 3. check once per frame for opengl errors
	GLenum Error = glGetError();
	assert(Error == 0);
}

void Application::end()
{
	for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
		delete *it;

	Models.clear();
}

void Application::resetLevel()
{
	ISound* lose = soundEngine->play2D(ASSET_DIRECTORY "audio/lose.mp3", false);

	// reset player
	this->player->reset();

	// reset trucks
	int truckNumber = 0;

	for (auto const& i : this->TruckPool)
	{
		i->reset(truckNumber++);

		// reduce speed back to start value if difficulty changed beforehand
		if (this->currentDifficulty > 1)
			i->changeSpeedModifier(-this->difficultyIncreaseAmount * this->currentDifficulty);
	}

	// reset coins
	int coinNumber = 0;

	for (auto const& i : this->CoinPool)
	{
		i->reset(coinNumber++);
	}

	// reset position of all coins in this pool to "hide" them from the players eyes
	for (auto const& i : this->CollectedCoinsPool)
	{
		i->resetStackableCoin();
	}

	// reset coin count
	this->currentCollectedCoins = 0;

	// reset all coin stack progress
	this->stackProgress[0] = 0;
	this->stackProgress[1] = 0;
	this->stackProgress[2] = 0;
	this->stackProgress[3] = 0;

	// reset global values
	this->isDodging = false;
	this->dodgeCooldown = 2.f;
	this->currentDifficulty = 1;
}

// add coin to coin count when picked up
void Application::addCoin()
{
	ISound* coinGet = soundEngine->play2D(ASSET_DIRECTORY "audio/coin_get.mp3", false);

	// limit because of performance, still play audio though
	if (this->currentCollectedCoins >= 40)
		return;

	// if all coins from coin pool for stackable coins are used, increase size
	if (this->currentCollectedCoins >= this->stackableCoinsCount)
	{
		for (size_t i = this->stackableCoinsCount; i < this->stackableCoinsCount + 10; ++i)
		{
			this->coin = new Coin();
			this->coin->shader(new PhongShader(), true);
			this->coin->loadModels(ASSET_DIRECTORY "stackCoin.dae", -1);
			this->coin->shadowCaster(false);
			this->CollectedCoinsPool.push_back(this->coin);
			Models.push_back(this->coin);

			this->coin->resetStackableCoin();
		}
		this->coin = NULL;

		this->stackableCoinsCount += 10;
	}

	int stackNumber = (int)(this->currentCollectedCoins / 10);

	// find next coin in list
	std::list<Coin*>::iterator it = this->CollectedCoinsPool.begin();
	std::advance(it, this->currentCollectedCoins++);

	// add to stack
	(*it)->stackCoin(stackNumber, this->stackProgress[stackNumber]++);
}

void Application::createBaseScene()
{
	Matrix m;

	// environment
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);

	// objects
	this->player = new Player();
	this->player->shader(new PhongShader(), true);
	this->player->loadModels(ASSET_DIRECTORY "player2.dae", ASSET_DIRECTORY "pan.dae");
	this->player->shadowCaster(true);
	Models.push_back(this->player);

	// truck object pool
	for (size_t i = 0; i < 5; ++i)
	{
		this->truck = new Truck();
		this->truck->shader(new PhongShader(), true);
		this->truck->loadModels(ASSET_DIRECTORY "truck.dae", i);
		this->truck->shadowCaster(true);
		this->TruckPool.push_back(this->truck);
		Models.push_back(this->truck);
	}
	this->truck = NULL;

	// coin object pool
	for (size_t i = 0; i < 20; ++i)
	{
		this->coin = new Coin();
		this->coin->shader(new PhongShader(), true);
		this->coin->loadModels(ASSET_DIRECTORY "coin.dae", i);
		this->coin->shadowCaster(false);
		this->CoinPool.push_back(this->coin);
		Models.push_back(this->coin);
	}
	this->coin = NULL;

	// coin object pool for stacked coins
	for (size_t i = 0; i < this->stackableCoinsCount; ++i)
	{
		this->coin = new Coin();
		this->coin->shader(new PhongShader(), true);
		this->coin->loadModels(ASSET_DIRECTORY "stackCoin.dae", -1);
		this->coin->shadowCaster(false);
		this->CollectedCoinsPool.push_back(this->coin);
		Models.push_back(this->coin);

		this->coin->resetStackableCoin();
	}
	this->coin = NULL;
}

void Application::createForrestScene()
{
	Matrix m;

	pModel = new Model(ASSET_DIRECTORY "forrest.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(0, 0, 0);
	pModel->shadowCaster(true);
	pModel->transform(m);
	Models.push_back(pModel);

	//Matrix rotate = rotate.rotationY(-0.5f * M_PI);
	//pTank->transform(rotate);
}

void Application::createForrestLighting()
{
	// 31 active light sources total if all activated
	// issues with more light sources, so some are deactivated
	// adjusted the max count in Lights.h and fslightdummy.glsl

	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.3, 0.3, 0.399));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);

	//dl = new DirectionalLight();
	//dl->direction(Vector(-0.2f, -1, -1));
	//dl->color(Color(0.2, 0.2, 0.266));
	//dl->castShadows(true);
	//ShaderLightMapper::instance().addLight(dl);

	// point lights
	Color c = Color(1.0f, 0.7f, 1.0f);

	// left side
	PointLight* pl = new PointLight();
	//pl->position(Vector(-5, 2.13f, 45));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-4, 2.13f, 45.5));
	//pl->attenuation(Vector(.1, 1, 1));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-5, 2.13f, 33));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-4, 2.13f, 33.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-5, 2.13f, 21));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-4, 2.13f, 21.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-5, 2.13f, 9));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-4, 2.13f, 9.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-5, 2.13f, -3));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-4, 2.13f, -2.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-5, 2.13f, -15));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-4, 2.13f, -14.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-5, 2.13f, -27));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-4, 2.13f, -26.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-5, 2.13f, -39));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(-4, 2.13f, -38.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	// right side
	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, 39));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, 39.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 27));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 27.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 15));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 15.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 3));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, 3.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, -9));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5, 2.13f, -8.5));
	pl->attenuation(Vector(.25, .25, .25));
	pl->color(c);
	ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, -21));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, -20.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, -33));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);

	//pl = new PointLight();
	//pl->position(Vector(5, 2.13f, -32.5));
	//pl->attenuation(Vector(.25, .25, .25));
	//pl->color(c);
	//ShaderLightMapper::instance().addLight(pl);
}
