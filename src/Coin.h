#pragma once

#ifndef Coin_hpp
#define Coin_hpp

#include <stdio.h>
#include "Model.h"

class Coin : public BaseModel
{
public:
	Coin();
	virtual ~Coin();
	bool loadModels(const char* CoinFile, int coinNumber);
	void start(float startingSpeed);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	const Model* getModel() const;
	const int getCoinNumber() const;
	void stackCoin(int stackNumber, int stackProgress);
	void resetStackableCoin();
	void reset(int coinNumber);
	void animate(float dtime);

protected:
	Model* coinModel;

	float speed;
	int coinNumber;

	bool forwardAnimation;
	float animationMin;
	float animationMax;
	float animationProgress;
	float animationSpeed;
};

#endif /* Coin_hpp */
