#include "Truck.h"

#define M_PI 3.14159265358979323846

Truck::Truck() : truckModel(NULL)
{
	this->speed = 0.f;
	this->difficultyModifier = 1.f;
	this->forwardBackward = 0.f;
	this->truckNumber = 0;

	leftLampCone = new SpotLight();
	rightLampCone = new SpotLight();
	leftLamp = new PointLight();
	rightLamp = new PointLight();
}

Truck::~Truck()
{
	delete(this->truckModel);
}

bool Truck::loadModels(const char* TruckFile, int truckNumber)
{
	truckModel = new Model(TruckFile, false);
	truckModel->shader(this->pShader, true);

	// set model type for easier collision checks
	this->truckModel->setModelType("Truck");

	// save truckNumber to prevent trucks clipping into each other
	this->truckNumber = truckNumber;

	// create randomized truck position in x
	float xPosition = -2.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.5f - (-2.5f))));

	// create gap between truck positions in z
	float zPosition = -75.f + -20.f * truckNumber;

	// apply truck position
	Matrix transformMatrix;
	transformMatrix = this->truckModel->transform();
	transformMatrix.translation(xPosition, 0, zPosition);
	this->truckModel->transform(transformMatrix);
	this->transform(transformMatrix);

	// --------------------------------------------------------------------------------
	// create car light cones

	leftLampCone->direction(Vector(0, -.75, 1));
	leftLampCone->position(Vector(xPosition, 1, -70.f + -20.f * truckNumber));

	rightLampCone->direction(Vector(0, -.75, 1));
	rightLampCone->position(Vector(xPosition + 0.65f, 1, -70.f + -20.f * truckNumber));

	// randomize car light cone colors
	this->randomizeLightColor();

	float inner = 30;
	float outer = 50;
	leftLampCone->innerRadius(inner);
	rightLampCone->innerRadius(inner);
	leftLampCone->outerRadius(outer);
	rightLampCone->outerRadius(outer);

	Vector attenuation = Vector(.5, .25, 0);
	leftLampCone->attenuation(attenuation);
	rightLampCone->attenuation(attenuation);

	leftLampCone->castShadows(false);
	rightLampCone->castShadows(false);

	ShaderLightMapper::instance().addLight(leftLampCone);
	ShaderLightMapper::instance().addLight(rightLampCone);

	// --------------------------------------------------------------------------------
	// create car lights

	leftLamp->position(Vector(xPosition, 1.15f, -69.f + -20.f * truckNumber));
	leftLamp->attenuation(Vector(.25, .25, .25));

	rightLamp->position(Vector(xPosition + 0.65f, 1.15f, -69.f + -20.f * truckNumber));
	rightLamp->attenuation(Vector(.25, .25, .25));

	ShaderLightMapper::instance().addLight(leftLamp);
	ShaderLightMapper::instance().addLight(rightLamp);

	return true;
}

void Truck::steer(float ForwardBackward, float LeftRight)
{
	this->forwardBackward = ForwardBackward;
}

void Truck::start(int startingSpeed)
{
	this->speed = startingSpeed;
}

void Truck::update(float dtime)
{
	// if truck reaches the end of a level, move it back to the beginning
	if (this->truckModel->transform().m23 >= 50.f)
		this->reset(1);

	Matrix transformMatrix = this->truckModel->transform();
	Matrix translationMatrix;

	// translation
	Vector forward = transformMatrix.forward();
	forward = forward * dtime * this->speed * this->difficultyModifier;

	translationMatrix.translation(forward);
	transformMatrix *= translationMatrix;

	// apply transform to everything
	this->truckModel->transform(transformMatrix);
	this->transform(transformMatrix);
	this->leftLamp->position(this->leftLamp->position() + forward);
	this->rightLamp->position(this->leftLamp->position() + forward);
	this->leftLampCone->position(this->leftLamp->position() + forward);
	this->rightLampCone->position(this->leftLamp->position() + forward);
}

void Truck::draw(const BaseCamera& Cam)
{
	this->truckModel->draw(Cam);
}

void Truck::randomizeLightColor()
{
	Color c;
	float min, max;
	min = 0.5f;
	max = 1.0f;

	c.R = min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	c.G = min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	c.B = min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	
	leftLampCone->color(c);
	rightLampCone->color(c);
	leftLamp->color(c);
	rightLamp->color(c);
}

void Truck::reset()
{
	// reset truck to beginning of level
	Matrix transformMatrix = this->truckModel->transform();
	Matrix translationMatrix;

	// randomize spawn position
	float position = -2.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.5f - (-2.5f))));

	// randomize truck light colors
	this->randomizeLightColor();

	Vector spawn = Vector(position, 0, -50.f * truckNumber);
	translationMatrix.translation(spawn);
	transformMatrix = translationMatrix;

	this->truckModel->transform(transformMatrix);
	this->transform(transformMatrix);
	this->leftLamp->position(Vector(position, 1.f, this->truckModel->transform().m23 + 6.f));
	this->rightLamp->position(Vector(position + 0.65f, 1.f, this->truckModel->transform().m23 + 6.f));
	this->leftLampCone->position(Vector(position, 1.15f, this->truckModel->transform().m23 + 8.f));
	this->rightLampCone->position(Vector(position + 0.65f, 1.15f, this->truckModel->transform().m23 + 8.f));
}

void Truck::reset(int truckNumber)
{
	// set new randomized truck position in x
	float xPosition = -2.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.5f - (-2.5f))));

	// reset gap between truck positions in z
	float zPosition = -40.f + -20.f * truckNumber;

	// apply truck position
	Matrix m;
	m = this->truckModel->transform();
	m.translation(xPosition, 0, zPosition);
	this->truckModel->transform(m);
	this->transform(m);

	// reset car light cones position
	leftLampCone->position(Vector(xPosition, 1, -35.f + -20.f * truckNumber));
	rightLampCone->position(Vector(xPosition + 0.65f, 1, -35.f + -20.f * truckNumber));

	// set new random car light cone colors
	this->randomizeLightColor();

	// reset car lights position
	leftLamp->position(Vector(xPosition, 1.15f, -34.f + -20.f * truckNumber));
	rightLamp->position(Vector(xPosition + 0.65f, 1.15f, -34.f + -20.f * truckNumber));
}

void Truck::changeSpeedModifier(float increaseAmount)
{
	this->difficultyModifier += increaseAmount;
}

const Model* Truck::getModel() const
{
	return this->truckModel;
}
