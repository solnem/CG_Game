#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, const char* SandMap, const char* SnowMap) : Size(10, 1, 10)
{
	if (HeightMap && DetailMap1 && DetailMap2 && MixMap && SandMap && SnowMap)
	{
		bool loaded = load(HeightMap, DetailMap1, DetailMap2, MixMap, SandMap, SnowMap);
		if (!loaded)
			throw std::exception();
	}
}

Terrain::~Terrain() {}

bool Terrain::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, const char* Sand, const char* Snow)
{
	if (!HeightTex.load(HeightMap))
		return false;
	if (!DetailTex[0].load(DetailMap1))
		return false;
	if (!DetailTex[1].load(DetailMap2))
		return false;
	if (!MixTex.load(MixMap))
		return false;
	if (!SandTex.load(Sand))
		return false;
	if (!SnowTex.load(Snow))
		return false;

	// get image from heightmap
	const RGBImage& heightMapImg = *this->HeightTex.getRGBImage();

	unsigned int width = heightMapImg.width();
	unsigned int depth = heightMapImg.height();

	// set start position so center of model is world center
	float startX = this->width() * -0.5f;
	float startZ = this->depth() * -0.5f;

	// change step size based on size of heightmap
	float stepX = this->width() / (width);
	float stepZ = this->depth() / (depth);

	// texture coordinates step size
	float texStepU = 1.f / width;
	float texStepV = 1.f / depth;

	VB.begin();
	// iterate through image (XZ-plane)
	for (unsigned int z = 0; z < depth; ++z)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			Color grayscale = heightMapImg.getPixelColor(x, z);
			//float y = (grayscale.R + grayscale.B + grayscale.G) / 3.f; // not needed since R, G and B are all the same value (grayscale)
			float y = grayscale.R;

			float px = startX + x * stepX;
			float py = y * this->height();
			float pz = startZ + z * stepZ;

			// Normal calculation for smooth shading:
			/*
			Idea:

			f1-f4 = faces
			p0-p4 = vertices

			o		x<-p2	o

				f1		f2

			x<-p1	x<-p0	x<-p3

				f4		f3

			o		x<-p4	o


			The smooth normal of a vertex p0 is calculated through all of it's adjacent vertices p1-p4
			We calculate all perpendicular normal vectors for the faces that use p0 through the cross product of two vertices adjacent to p0
			For example:
			v1 = p1 - p0, v2 = p2 - p0
			v12 = v2 x v1

			With all face normals calculated, we add them together and normalize the result:
			nv = normalize(v12 + v23 + v34 + v41)
			*/
			
			// calculate the normal for each face through p1 and the other corresponding vertices that make up the face
			// the perpendicular normal vector of a face is calculated through the cross product of two vectors on the same coplanar
			// by using only the raw x and z values + the grayscale value from the color, the normal should be normalized already

			Color c1, c2, c3, c4;
			Vector p1, p2, p3, p4;
			Vector v1, v2, v3, v4;

			// calculate each face normal through the vector of the adjacent vertex - the current vertex
			// vector pointing left of p0
			if (x == 0)
				v1 = Vector(-1, 0, 0);
			else
			{
				c1 = heightMapImg.getPixelColor(x - 1, z);
				p1 = Vector(px - (1 * stepX), c1.R, pz);
				v1 = Vector(p1.X - px, p1.Y - py, p1.Z - pz);
			}

			// vector pointing forward (z axis) of p0
			if (z == 0)
				v2 = Vector(0, 0, -1);
			else
			{
				c2 = heightMapImg.getPixelColor(x, z - 1);
				p2 = Vector(px, c2.R, pz - (1 * stepZ));
				v2 = Vector(p2.X - px, p2.Y - py, p2.Z - pz);
			}

			// vector pointing right of p0
			if (x == width - 1)
				v3 = Vector(1, 0, 0);
			else
			{
				c3 = heightMapImg.getPixelColor(x + 1, z);
				p3 = Vector(px + (1 * stepX), c3.R, pz);
				v3 = Vector(p3.X - px, p3.Y - py, p3.Z - pz);
			}

			// vector pointing backward (z axis) of p0
			if (z == depth - 1)
				v4 = Vector(0, 0, 1);
			else
			{
				c4 = heightMapImg.getPixelColor(x, z + 1);
				p4 = Vector(px, c4.R, pz + (1 * stepZ));
				v4 = Vector(p4.X - px, p4.Y - py, p4.Z - pz);
			}

			// calculate the cross product of each face to get the perpendicular vector
			Vector v12, v23, v34, v41;

			v12 = v2.cross(v1);
			v23 = v3.cross(v2);
			v34 = v4.cross(v3);
			v41 = v1.cross(v4);

			// add all face normal vectors together and normalize the result
			Vector nv = (v12 + v23 + v34 + v41).normalize();

			VB.addNormal(nv);

			// uv (set to 0)
			//VB.addTexcoord0(0, 0);

			// uv coords set to detailmap textures
			float texCoordU = x * texStepU;
			float texCoordV = z * texStepV;

			VB.addTexcoord0(texCoordU, texCoordV);


			// x = width, y = height, z = depth
			VB.addVertex(px, py, pz);
		}
	}
	VB.end();

	IB.begin();
	for (size_t z = 0; z < depth - 1; ++z)
	{
		for (size_t x = 0; x < width - 1; ++x)
		{
			unsigned int index = z * width + x;

			// first triangle
			IB.addIndex(index + width);
			IB.addIndex(index + 1);
			IB.addIndex(index);

			// second triangle
			IB.addIndex((index + 1) + width);
			IB.addIndex(index + 1);
			IB.addIndex(index + width);
		}
	}
	IB.end();

	RGBImage mixMap = RGBImage(width, depth);
	RGBImage::SobelFilter(mixMap, heightMapImg, 10.0f);
	//mixMap.saveToDisk("../../assets/mixMap3.bmp");

	return true;
}

void Terrain::shader(BaseShader* shader, bool deleteOnDestruction)
{
	BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
	applyShaderParameter(Cam);
	BaseModel::draw(Cam);
	
	// code for drawing index and vertex buffers
	VB.activate();
	IB.activate();
	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
	IB.deactivate();
	VB.deactivate();
}

void Terrain::applyShaderParameter(const BaseCamera& Cam)
{
	TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
	if (!Shader)
		return;

	for (int i = 0; i < 2; ++i)
		Shader->detailTex(i, &DetailTex[i]);

	Shader->mixTex(&MixTex);

	// add sand and snow textures to shader
	Shader->sandTex(&SandTex);
	Shader->snowTex(&SnowTex);

	// set scaling value in shader
	Shader->scaling(this->Size);

	// set texture repeating value in shader
	Shader->textureRepeat(this->TextureRepeat);

	Shader->activate(Cam);
}
