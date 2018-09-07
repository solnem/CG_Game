#include "Coin.h"

#define M_PI 3.14159265358979323846

Coin::Coin()
{
	this->speed = 0.f;
	this->forwardAnimation = true;
	this->animationMin = 0.f;
	this->animationMax = 0.5f;
	this->animationProgress = 0.f;
	this->animationSpeed = 0.05f;

	this->coinNumber = -1;
}

Coin::~Coin()
{
	delete(this->coinModel);
}

bool Coin::loadModels(const char* CoinFile, int coinNumber)
{
	coinModel = new Model(CoinFile, false);
	coinModel->shader(this->pShader, true);

	// set model type for easier collision checks
	this->coinModel->setModelType("Coin");
	this->coinNumber = coinNumber;

	// set position
	if (coinNumber >= 0)
		this->reset(coinNumber);

	return true;
}

void Coin::start(float startingSpeed)
{
	this->speed = startingSpeed;
}

void Coin::update(float dtime)
{
	// if coin reaches the end of a level, move it back to the beginning
	if (this->coinModel->transform().m23 >= 50.f)
		this->reset(this->coinNumber);

	// don't update coin position if the coin is used for display only (stacking at the side of the level)
	if (this->coinNumber == -1)
		return;

	this->animate(dtime);

	Matrix transformMatrix = this->coinModel->transform();
	Matrix translationMatrix, rotationMatrix;

	Vector forward = Vector(0, 0, 1);
	forward = forward * dtime * this->speed;
	translationMatrix.translation(forward);

	//rotation
	//rotationMatrix.rotationY(dtime * M_PI * this->speed);

	// apply player transformation
	transformMatrix *= translationMatrix;
	this->transform(transformMatrix);
	this->coinModel->transform(transformMatrix);
}

void Coin::draw(const BaseCamera & Cam)
{
	this->coinModel->draw(Cam);
}

// duplicate method between coin and truck classes
// TODO: should be added to super class
const Model* Coin::getModel() const
{
	return this->coinModel;
}

const int Coin::getCoinNumber() const
{
	return this->coinNumber;
}

void Coin::stackCoin(int stackNumber, int stackProgress)
{
	// slightly randomize stack position
	float xPosition = -0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (0.05f - (-0.05f))));
	float zPosition = -0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (0.05f - (-0.05f))));

	// add more variety to coin stacks
	Vector stackPosition = Vector(0, 0, 0);
	switch (stackNumber)
	{
		case 0:
			// no change
			break;
		case 1:
			stackPosition = Vector(0.3f, 0, -1.0f);
			break;
		case 2:
			stackPosition = Vector(-1.0f, 0, -0.1f);
			break;
		case 3:
			stackPosition = Vector(-0.1f, 0, 1.1f);
			break;
	}

	Matrix transformMatrix;
	transformMatrix = this->coinModel->transform();
	transformMatrix.translation(Vector(-6 + xPosition, 0.124f + 0.124f * stackProgress, 27 + zPosition) + stackPosition);

	// apply transformationa
	this->coinModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

void Coin::resetStackableCoin()
{
	Matrix transformMatrix = this->coinModel->transform();
	transformMatrix.translation(Vector(-10, 0, 30));

	this->coinModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

void Coin::reset(int coinNumber)
{
	Matrix m;
	m = this->coinModel->transform();
	m.translation(Vector(0, 0, 25));

	// set randomized coin position in x
	float xPosition = -2.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.5f - (-2.5f))));

	// set gap between coin positions in z
	// also use xPosition to further randomize coin position
	float zPosition = -20.f + xPosition + -10.f * coinNumber;

	// apply coin position
	Matrix transformMatrix;
	transformMatrix = this->coinModel->transform();
	transformMatrix.translation(xPosition, 1.f, zPosition);
	this->coinModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

// simple coin animation
void Coin::animate(float dtime)
{
	// decide animation direction
	if (this->animationProgress < this->animationMin)
	{
		forwardAnimation = true;
		this->animationProgress = this->animationMin;
	}
	else if (this->animationProgress > this->animationMax)
	{
		forwardAnimation = false;
		this->animationProgress = this->animationMax;
	}

	// update animation progress based on current direction
	if (forwardAnimation)
		this->animationProgress += dtime;
	else
		this->animationProgress -= dtime;

	Matrix transformMatrix = this->coinModel->transform();
	Matrix translationMatrix;

	if (this->animationProgress < 0.f)
	{
		// subtract models current y position to make it sit on the ground
		translationMatrix.translation(Vector(0, -transformMatrix.m13, 0));

		// apply transformation
		transformMatrix *= translationMatrix;
		this->coinModel->transform(transformMatrix);
		this->transform(transformMatrix);

		// leave method to prevent further changes to transformation
		return;
	}

	// update coins y position
	if (this->forwardAnimation)
		translationMatrix.translation(Vector(0, this->animationProgress * this->animationSpeed, 0));
	else if (!this->forwardAnimation)
		translationMatrix.translation(Vector(0, -this->animationProgress * this->animationSpeed, 0));

	// apply transformation
	transformMatrix *= translationMatrix;
	this->coinModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

