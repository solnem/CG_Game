#ifndef Truck_hpp
#define Truck_hpp

#include <stdio.h>
#include "Model.h"
#include "Lights.h"
#include "ShaderLightMapper.h"


class Truck : public BaseModel
{
public:
	Truck();
	virtual ~Truck();
	bool loadModels(const char* TruckFile, int position);
	void steer(float ForwardBackward, float LeftRight);
	void start(int startingSpeed);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	void randomizeLightColor();
	void reset();
	void reset(int truckNumber);
	void changeSpeedModifier(float increaseAmount);
	const Model* getModel() const;
protected:
	Model* truckModel;

	SpotLight* leftLampCone;
	SpotLight* rightLampCone;

	PointLight* leftLamp;
	PointLight* rightLamp;

	float speed;
	float difficultyModifier;
	float forwardBackward;
	int truckNumber;
};

#endif /* Truck_hpp */
