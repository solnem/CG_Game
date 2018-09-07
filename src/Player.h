#pragma once

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include "Model.h"
#include "TriangleBoxModel.h"
#include "Truck.h"
#include "Coin.h"

class Player : public BaseModel
{
public:
	Player();
	virtual ~Player();
	bool loadModels(const char* CharacterFile, const char* WeaponFile);
	void move(float LeftRight);
	float update(float dtime, bool isDodging);
	void animate(float dtime);
	virtual void draw(const BaseCamera& Cam);
	bool checkCollision(const Model* other, bool& isCoin);

	void reset();
	int getCoinCount();
	//void aim(const Vector& Target);
	//void jump(float JumpPower);
	//void melee();
	//void ranged();

protected:
	Model* playerModel;
	//Model* mWeapon;

	float speed;
	//float forwardBackward;
	float leftRight;
	bool isDodging;

	// animation related variables
	float animationPosition;
	float animationProgress;
	float animationSpeed;
	bool forwardAnimation;
	
	int coinCount;

	// player character related
	//int dodgeDirection;
	//float dodgeAnimation;
	//float attackPower;
	//float jumpPower;
	//bool isGrounded;

	// weapon related
	//bool isThrowing;
	//Vector target;
};

#endif /* Player_hpp */
