#include "Enemy.h"

#define M_PI 3.14159265358979323846

Enemy::Enemy()
{

}

Enemy::~Enemy()
{
	delete(this->mEnemy);
}

bool Enemy::loadModels(const char* EnemyFile)
{
	mEnemy = new Model(EnemyFile, false);

	mEnemy->shader(this->pShader, true);

	//Matrix rotate = rotate.rotationY(0.5f * M_PI);
	mEnemy->transform();

	return true;
}

void Enemy::move(float LeftRight)
{
	this->leftRight = LeftRight;
}

void Enemy::aim(const Vector & Target)
{
	this->target = Target;
}

void Enemy::update(float dtime)
{
	Matrix transformMatrix = this->mEnemy->transform();
	Matrix translationMatrix, rotationMatrix;
}

void Enemy::draw(const BaseCamera & Cam)
{
	this->mEnemy->draw(Cam);
}

