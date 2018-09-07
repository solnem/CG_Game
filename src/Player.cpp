#include "Player.h"

#define M_PI 3.14159265358979323846

Player::Player() : playerModel(NULL)
{
	this->speed = 1.5f;
	this->leftRight = 0.f;
	
	// animation related values
	this->animationPosition = 0.f;
	this->animationProgress = 1.f;
	this->animationSpeed = 2.f;

	// toggle to decide if player model is jumping up or falling down in animation
	this->forwardAnimation = true;

	//this->attackPower = 5.f;
	//this->jumpPower = 2.f;
	//this->glideTime = 2.f;
	//this->isGrounded = true;
	//this->isThrowing = false;
	//this->target = Vector(0, 0, 0);
	//this->dodgeDirection = 0.f;
}

Player::~Player()
{
	delete(this->playerModel);
	//delete(this->mWeapon);
}

bool Player::loadModels(const char* CharacterFile, const char* WeaponFile)
{
	playerModel = new Model(CharacterFile, false);
	playerModel->shader(this->pShader, true);
	//mWeapon = new Model(WeaponFile, false);
	//mWeapon->shader(this->pShader, true);

	// to be consistent, set model type for player aswell
	this->playerModel->setModelType("Player");

	Matrix m;
	m = this->playerModel->transform();
	m.translation(0, 0, 28);
	this->playerModel->transform(m);
	this->transform(m);

	return true;
}

void Player::move(float LeftRight)
{
	this->leftRight = LeftRight;
}

// returns player position for camera adjustment
float Player::update(float dtime, bool isDodging)
{
	// continue animation each update
	this->animate(dtime);

	float adjustedSpeed = this->speed;

	// speed up if dodging
	if (isDodging)
	{
		this->animationSpeed = 4.f;
		adjustedSpeed *= 2.f;

		//this->dodgeAnimation += dtime;
		// spin player character
		//rotationMatrix.rotationZ(this->dodgeAnimation * M_PI);
	}
	else if (!isDodging && this->animationSpeed > 2.001f)
		this->animationSpeed = 2.f;

	// reset dodge animaton progress
	//if (this->dodgeAnimation >= 2.f)
	//	this->dodgeAnimation = 0.f;

	Matrix transformMatrix = this->playerModel->transform();
	Matrix translationMatrix, rotationMatrix;

	Vector leftRight = Vector(-this->leftRight, 0, 0);

	// translation
	// check if in allowed move space
	if (this->playerModel->transform().m03 <= -4.25f)
		translationMatrix.translation(Vector(0.01f, 0, 0));
	else if (this->playerModel->transform().m03 >= 4.25f)
		translationMatrix.translation(Vector(-0.01f, 0, 0));
	else
	{
		leftRight = leftRight * dtime * adjustedSpeed;
		translationMatrix.translation(leftRight);
	}

	// apply player transformation
	transformMatrix *= translationMatrix;
	this->playerModel->transform(transformMatrix);
	this->transform(transformMatrix);

	// return player position in x for camera movement
	return this->playerModel->transform().m03;
}

// simple animation
// TODO: add scaling matrix calculation
void Player::animate(float dtime)
{
	// decide animation direction
	if (this->animationProgress <= 0.f)
		forwardAnimation = true;
	else if (this->animationProgress >= 1.f)
		forwardAnimation = false;

	// update animation progress based on current direction
	if (forwardAnimation)
		this->animationProgress += dtime * this->animationSpeed;
	else
		this->animationProgress -= dtime * this->animationSpeed;

	// move animation according to animation direction
	// animation just starting or highest point almost reached -> slow animation down
	if (this->animationProgress < 0.1f)
		this->animationPosition = this->animationProgress + (this->animationProgress - 0.1f) * 0.1f;
	else if (this->animationProgress < 0.2f)
		this->animationPosition = this->animationProgress + (this->animationProgress - 0.2f) * 0.25f;
	else if (this->animationProgress > 0.8f)
		this->animationPosition = this->animationProgress + (this->animationProgress - 0.8f) * 0.25f;
	else if (this->animationProgress > 0.9f)
		this->animationPosition = this->animationProgress + (this->animationProgress - 0.9f) * 0.1f;
	else
		// mid-animation
		this->animationPosition = this->animationProgress;

	Matrix transformMatrix = this->playerModel->transform();
	Matrix translationMatrix, scaleMatrix;

	if (this->animationProgress > 1.f)
		this->animationProgress = 1.f;
	else if (this->animationProgress < 0.f)
	{
		// prevents model flying off into the air
		// TODO: fix the big jump in y of the model when it's landing
		this->animationProgress = 0.f;

		// subtract models current y position to make it sit on the ground
		translationMatrix.translation(Vector(0, -transformMatrix.m13, 0));

		// apply transformation
		transformMatrix *= translationMatrix;
		this->playerModel->transform(transformMatrix);
		this->transform(transformMatrix);

		// leave method to prevent further changes to transformation
		return;
	}

	// TODO: scaling model

	// update models y position
	if(this->forwardAnimation)
		translationMatrix.translation(Vector(0, this->animationPosition * 0.05f, 0));
	else if (!this->forwardAnimation)
		translationMatrix.translation(Vector(0, -this->animationPosition * 0.05f, 0));

	// apply transformation
	transformMatrix *= translationMatrix;
	this->playerModel->transform(transformMatrix);
	this->transform(transformMatrix);
}

void Player::draw(const BaseCamera & Cam)
{
	this->playerModel->draw(Cam);
	//this->mWeapon->draw(Cam);
}

bool Player::checkCollision(const Model* other, bool& isCoin)
{
	// get player position and bounding box
	float playerX = this->playerModel->transform().m03;
	float playerZ = this->playerModel->transform().m23;
	AABB playerAABB = this->playerModel->boundingBox();

	// get other position and bounding box
	float otherX = other->transform().m03;
	float otherZ = other->transform().m23;
	AABB otherAABB = other->boundingBox();

	// value to make bounding box slightly less big for the other
	float boundingBoxModifier = 0.9f;

	// check if boxes intersect
	bool collisionX = playerX + playerAABB.size().X / 2 >= otherX - (otherAABB.size().X / 2) * boundingBoxModifier
		&& otherX + (otherAABB.size().X / 2) * boundingBoxModifier >= playerX - playerAABB.size().X / 2;
	// in this case, the bounding box y relates to the z axis in our 3D room
	bool collisionZ = playerZ + playerAABB.size().Y / 2 >= otherZ - (otherAABB.size().Y / 2) * boundingBoxModifier
		&& otherZ + (otherAABB.size().Y / 2) * boundingBoxModifier >= playerZ - playerAABB.size().Y / 2;

	//check if it's a coin
	if (collisionX && collisionZ && other->getModelType() == "Coin")
		isCoin = true;
	else
		isCoin = false;

	return collisionX && collisionZ;
}

void Player::reset()
{
	this->leftRight = 0.f;

	// animation related values
	this->animationPosition = 0.f;
	this->animationProgress = 1.f;
	this->animationSpeed = 2.f;

	// toggle to decide if player model is jumping up or falling down in animation
	this->forwardAnimation = true;

	// reset player position
	Matrix m;
	m = this->playerModel->transform();
	m.translation(0, 0, 28);
	this->playerModel->transform(m);
	this->transform(m);
}

int Player::getCoinCount()
{
	return this->coinCount;
}

//void Player::aim(const Vector & Target)
//{
//	this->target = Target;
//}

//void Player::jump(float JumpPower)
//{
//	this->isGrounded = false;
//}

//void Player::melee()
//{
//
//}

//void Player::ranged()
//{
//	this->isThrowing = true;
//}
