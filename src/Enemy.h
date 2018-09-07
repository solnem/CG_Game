#pragma once

#ifndef Enemy_hpp
#define Enemy_hpp

#include <stdio.h>
#include "Model.h"

class Enemy : public BaseModel
{
public:
	Enemy();
	virtual ~Enemy();
	bool loadModels(const char* EnemyFile);
	void move(float LeftRight);
	void aim(const Vector& Target);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

protected:
	Model * mEnemy;
	Model* mWeapon;

	//float attackPower;
	//float jumpPower;
	//float speed;
	//float glideTime;
	float forwardBackward;
	float leftRight;

	// player character related
	//bool isGrounded;
	// weapon related
	//bool isThrown;
	Vector target;
};

#endif /* Enemy_hpp */
