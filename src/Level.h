#pragma once

#ifndef Level_hpp
#define Level_hpp

#include <stdio.h>
#include "Model.h"

class Level : public BaseModel
{
public:
	Level();
	virtual ~Level();
	bool loadModels(const char* LevelFile);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

protected:
	Model* mLevel;
	Vector target;
};

#endif /* Level_hpp */
