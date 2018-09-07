//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel(float Width, float Height, float Depth)
{
	VB.begin();

	float beginX = -Width / 2.f;
	float beginY = -Height / 2.f;
	float beginZ = -Depth / 2.f;

	for (size_t i = 0; i < 2; ++i)
	{
		// front/back x lines
		for (size_t j = 0; j < 2; ++j)
		{
			VB.addVertex(beginX, beginY + j * Height, beginZ + i * Depth);
			VB.addVertex(-beginX, beginY + j * Height, beginZ + i * Depth);
		}

		// front/back y lines
		for (size_t j = 0; j < 2; ++j)
		{
			VB.addVertex(beginX + j * Width, beginY, beginZ + i * Depth);
			VB.addVertex(beginX + j * Width, -beginY, beginZ + i * Depth);
		}

		// left/right z lines
		for (size_t j = 0; j < 2; ++j)
		{
			VB.addVertex(beginX + i * Width, beginY + j * Height, beginZ);
			VB.addVertex(beginX + i * Width, beginY + j * Height, -beginZ);
		}
	}

	VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}
