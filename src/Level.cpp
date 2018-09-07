#include "Level.h"

#define M_PI 3.14159265358979323846

Level::Level()
{
	//this->target = Vector(0, 0, 0);
}

Level::~Level()
{
	delete(this->mLevel);
}

bool Level::loadModels(const char* LevelFile)
{
	mLevel = new Model(LevelFile, false);

	mLevel->shader(this->pShader, true);

	//Matrix rotate = rotate.rotationY(0.5f * M_PI);
	mLevel->transform();

	return true;
}

void Level::update(float dtime)
{
	Matrix transformMatrix = this->mLevel->transform();
	Matrix translationMatrix, rotationMatrix;

}

void Level::draw(const BaseCamera & Cam)
{
	this->mLevel->draw(Cam);
}
