#include "TerrainShader.h"
#include <string>

TerrainShader::TerrainShader(const std::string& AssetDirectory) : PhongShader(false), Scaling(1, 1, 1), MixTex(NULL)
{
	std::string VSFile = AssetDirectory + "vsterrain.glsl";
	std::string FSFile = AssetDirectory + "fsterrain.glsl";
	if (!load(VSFile.c_str(), FSFile.c_str()))
		throw std::exception();
	PhongShader::assignLocations();
	specularColor(Color(0, 0, 0));

	// get location (ID) of variables in shader
	MixTexLoc = getParameterID("MixTex");
	SandTexLoc = getParameterID("SandTex");
	SnowTexLoc = getParameterID("SnowTex");
	
	ScalingLoc = getParameterID("Scaling");
	TextureRepeatLoc = getParameterID("TextureRepeat");

	for (int i = 0; i < DETAILTEX_COUNT; ++i)
	{
		DetailTex[i] = NULL;
		std::string s;
		s += "DetailTex[" + std::to_string(i) + "]";
		DetailTexLoc[i] = getParameterID(s.c_str());
	}
}

void TerrainShader::activate(const BaseCamera& Cam) const
{
	PhongShader::activate(Cam);

	// assign values to variables in shader
	int slot = 0;
	activateTex(MixTex, MixTexLoc, slot++);

	for (int i = 0; i < DETAILTEX_COUNT; ++i)
		activateTex(DetailTex[i], DetailTexLoc[i], slot++);

	activateTex(SandTex, SandTexLoc, slot++);
	activateTex(SnowTex, SnowTexLoc, slot++);

	setParameter(ScalingLoc, Scaling);
	setParameter(TextureRepeatLoc, TextureRepeat);
}

void TerrainShader::deactivate() const
{
	PhongShader::deactivate();
	for (int i = DETAILTEX_COUNT - 1; i >= 0; --i)
		if (DetailTex[i] && DetailTexLoc[i] >= 0) DetailTex[i]->deactivate();
	if (MixTex) MixTex->deactivate();

	if (SandTex) SandTex->deactivate();
	if (SnowTex) SnowTex->deactivate();
}

void TerrainShader::activateTex(const Texture* pTex, GLint Loc, int slot) const
{
	if (pTex && Loc >= 0)
	{
		pTex->activate(slot);
		setParameter(Loc, slot);
	}
}
