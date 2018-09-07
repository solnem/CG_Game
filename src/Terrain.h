#ifndef Terrain_hpp
#define Terrain_hpp

#include <stdio.h>
#include "basemodel.h"
#include "texture.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

class Terrain : public BaseModel
{
public:
	Terrain(const char* HeightMap = NULL, const char* DetailMap1 = NULL, const char* DetailMap2 = NULL, const char* MixMap = NULL, const char* Sand = NULL, const char* Snow = NULL);
	virtual ~Terrain();
	bool load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, const char* Sand, const char* Snow);

	virtual void shader(BaseShader* shader, bool deleteOnDestruction = false);
	virtual void draw(const BaseCamera& Cam);

	float width() const { return Size.X; }
	float height() const { return Size.Y; }
	float depth() const { return Size.Z; }

	void width(float v) { Size.X = v; }
	void height(float v) { Size.Y = v; }
	void depth(float v) { Size.Z = v; }

	const Vector& size() const { return Size; }
	void size(const Vector& s) { Size = s; }

	int textureRepeat() { return TextureRepeat; }
	void textureRepeat(int k) { TextureRepeat = k; }

protected:
	void applyShaderParameter(const BaseCamera& Cam);

	VertexBuffer VB;
	IndexBuffer IB;
	Texture DetailTex[2];
	Texture MixTex;
	Texture SandTex;
	Texture SnowTex;
	Texture HeightTex;

	Vector Size;
	int TextureRepeat;
};

#endif /* Terrain_hpp */
