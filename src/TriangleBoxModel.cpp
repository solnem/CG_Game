//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"

TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth)
{
	float beginX = Width * -0.5f;
	float beginY = Height * -0.5f;
	float beginZ = Depth * -0.5f;

	/*
		
		z seems to be "flipped" in the opengl app
		so:

	   +y
		^	   __ -z
		|       /|
		|     /
		|   /
		| /
		o----------> +x

					   ____________
					  /|		  /|
					 / |		 / |
					/  |		/  |
				   /   |	   /   |
				  /	   |______/____|
				 /	   /	 /	   /			//
				/_____/_____/     /			   //
		||		|    /	    |    /			  //   
		||		|   /	    |   /			 //		
		||		|  /	    |  /			//
		||		| /		    | /			   //
		||		|/__________|/			  //
	  Height							Depth
				
				============
				   Width
	*/

	// When UV mapping, the V-axis needs to be "flipped"!
	// s = u;
	// t = 1 - v;

	VB.begin();

	// XY plane: front + back face calculation:
	// relative depth for each pair of faces
	for (size_t zPosRelative = 0; zPosRelative < 2; ++zPosRelative)
	{
		// relative height loop
		for (size_t yPosRelative = 0; yPosRelative < 2; ++yPosRelative)
		{
			// relative width loop
			for (size_t xPosRelative = 0; xPosRelative < 2; ++xPosRelative)
			{
				float px = beginX + (float)xPosRelative * Width;
				float py = beginY + (float)yPosRelative * Height;
				float pz = beginZ + (float)zPosRelative * Depth;
				
				float s0, t0, s1, t1;

				if (zPosRelative == 0)
				{
					// back face
					// vertex normal
					VB.addNormal(Vector(0, 0, -1));

					// s0 starts in the other corner here
					// hence, 1 - xPosRelative;
					// t0 doesn't change here

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = 1 - (float)xPosRelative;
					t0 = 1 - (float)yPosRelative;
				}
				else // zPosRelative == 1
				{
					// front face
					// vertex normal
					VB.addNormal(Vector(0, 0, 1));

					// s0, t0 doesn't change here

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = (float)xPosRelative;
					t0 = 1 - (float)yPosRelative;
				}

				// UV texture coordinate (1,1)
				s1 = s0 + 1;
				t1 = t0 + 1;

				VB.addTexcoord0(s0, t0);
				VB.addTexcoord1(s1, t1);

				VB.addVertex(px, py, pz);
			}
		}
	}

	// ZY plane: left + right face calculation:
	// relative depth for each pair of faces
	for (size_t zPosRelative = 0; zPosRelative < 2; ++zPosRelative)
	{
		// relative height loop
		for (size_t yPosRelative = 0; yPosRelative < 2; ++yPosRelative)
		{
			// relative width loop
			for (size_t xPosRelative = 0; xPosRelative < 2; ++xPosRelative)
			{
				// flip zPosRelative and xPosRelative, because ZY plane
				float px = beginX + (float)zPosRelative * Width;
				float py = beginY + (float)yPosRelative * Height;
				float pz = beginZ + (float)xPosRelative * Depth;

				float s0, t0, s1, t1;

				if (zPosRelative == 0)
				{
					// left face
					// vertex normal
					VB.addNormal(Vector(-1, 0, 0));

					// for zPos, yPos and xPos == 0
					// the first vertex is at (beginX, beginY, beginZ)
					// when looking onto the face from the left side, that would translate into XY coordinates of (0,0)
					// hence, no need to change s0/t0 for left side

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = (float)xPosRelative;
					t0 = 1 - (float)yPosRelative;
				}
				else // zPosRelative == 1
				{
					// right face
					// vertex normal
					VB.addNormal(Vector(1, 0, 0));

					// for zPos == 1, yPos and xPos == 0
					// the first vertex is at (beginX + 1(for loop iterator) * Width, beginY, beginZ)
					// when looking onto the face from the right side, that would translate into XY coordinates of (1,0)
					// hence, when zPosRelative is 1, s0 is 1 - xPosRelative
					// t0 doesn't change here

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = 1 - (float)xPosRelative;
					t0 = 1 - (float)yPosRelative;
				}

				// UV texture coordinate (1,1)
				s1 = s0 + 1;
				t1 = t0 + 1;

				VB.addTexcoord0(s0, t0);
				VB.addTexcoord1(s1, t1);

				VB.addVertex(px, py, pz);
			}
		}
	}

	// XZ plane: bottom + top face calculation:
	// relative depth for each pair of faces
	for (size_t zPosRelative = 0; zPosRelative < 2; ++zPosRelative)
	{
		// relative height loop
		for (size_t yPosRelative = 0; yPosRelative < 2; ++yPosRelative)
		{
			// relative width loop
			for (size_t xPosRelative = 0; xPosRelative < 2; ++xPosRelative)
			{
				// flip zPosRelative and yPosRelative, because XZ plane
				float px = beginX + (float)xPosRelative * Width;
				float py = beginY + (float)zPosRelative * Height;
				float pz = beginZ + (float)yPosRelative * Depth;

				float s0, t0, s1, t1;

				if (zPosRelative == 0)
				{
					// bottom face
					// vertex normal
					VB.addNormal(Vector(0, -1, 0));

					// for zPos, yPos and xPos == 0
					// the first vertex is at (beginX, beginY, beginZ)
					// when looking from the bottom side, that would translate into XY coordinates of (0,0)
					// hence, no need to change s0/t0 for bottom side

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = (float)xPosRelative;
					t0 = 1 - (float)yPosRelative;
				}
				else // zPosRelative == 1
				{
					// top face
					// vertex normal
					VB.addNormal(Vector(0, 1, 0));

					// for zPos == 1, yPos and xPos == 0
					// the first vertex is at (beginX, beginY + 1(for loop iterator) * Height, beginZ)
					// when looking from the top side, that would translate into XY coordinates of (0,1)
					// hence, when zPosRelative is 1, t0 is 1 - yPosRelative
					// that results in t0 being double flipped (because of t = 1 - v)
					// hence, t0 = yPosRelative
					// s0 doesn't change here

					// UV texture coordinate (0,0) (remember: t = 1 - v!)
					s0 = 1 - (float)xPosRelative;
					t0 = (float)yPosRelative;
				}

				// UV texture coordinate (1,1)
				s1 = s0 + 1;
				t1 = t0 + 1;

				VB.addTexcoord0(s0, t0);
				VB.addTexcoord1(s1, t1);

				VB.addVertex(px, py, pz);
			}
		}
	}

	VB.end();

	IB.begin();

	//We need to index CCW, since glFrontFace() is not set and defaults to GL_CCW being the front face
	// CCW index buffering!
	// 0, 1, 3, 0, 3, 2, ...

	// faces loop
	for (size_t face = 0; face < 6; ++face)
	{
		/*
		  v2	   v3
			.-----.
			|    /|
			|   / |
			|  /  |
			| /   |
			|/    |
			.-----.
		  v0	    v1

		Vertex array:
		v0(0,0) v1(0,1) v2(1,0) v3(1,1)

		Index array (CCW):
		0, 1, 3, 0, 3, 2
		*/

		unsigned int index = face * 4;

		if (face == 0 || face == 3)
		{
			// face looking in -z/+x direction
			// has v0 in different corner, so we need to keep that in mind

			// first triangle
			IB.addIndex(index);
			IB.addIndex(index + 2);
			IB.addIndex(index + 3);

			// 0 2 3

			// second triangle
			IB.addIndex(index);
			IB.addIndex(index + 3);
			IB.addIndex(index + 1);

			// 0 3 1
		}
		else if (face == 5)
		{
			// face looking in +y direction
			// has v0 in different corner, so we need to keep that in mind

			// first triangle
			IB.addIndex(index + 3);
			IB.addIndex(index + 1);
			IB.addIndex(index);

			// 3 1 0

			// second triangle
			IB.addIndex(index + 3);
			IB.addIndex(index);
			IB.addIndex(index + 2);

			// 3 0 2
		}
		else
		{
			// first triangle
			IB.addIndex(index);
			IB.addIndex(index + 1);
			IB.addIndex(index + 3);

			// 0 1 3

			// second triangle
			IB.addIndex(index);
			IB.addIndex(index + 3);
			IB.addIndex(index + 2);

			// 0 3 2
		}
	}
	
	IB.end();
}

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
	BaseModel::draw(Cam);

	VB.activate();
	IB.activate();
	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
	IB.deactivate();
	VB.deactivate();
}
