#version 400
uniform vec3 EyePos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D MixTex; // for exercise 3
uniform sampler2D DetailTex[2]; // for exercise 3

uniform sampler2D SandTex;
uniform sampler2D SnowTex;

uniform vec3 Scaling;
uniform int TextureRepeat;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
out vec4 FragColor;

float sat( in float a)
{
	return clamp(a, 0.0, 1.0);
}


void main()
{
	vec3 N      = normalize(Normal);
	vec3 L      = normalize(LightPos); // light is treated as directional source
	vec3 D      = EyePos-Position;
	float Dist  = length(D);
	vec3 E      = D/Dist;
	vec3 R      = reflect(-L,N);

	// old:
	/*
	vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
	vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
	
	// Exercise 3
	// TODO: Add texture blending code here..

	// Texel_final = (Texel_Mixtextur * Texel_Steintextur + (1-Texel_Mixtextur)*Texel_Rasentextur)*Reflexionsergebnisfarbe
	
	FragColor = vec4(((DiffuseComponent + AmbientColor) + SpecularComponent),1);
	*/
	
	// get grayscale (height) value from mixmap
	// this will basically be the percentage with which the textures will be mixed
	float grayscale = texture2D(MixTex, Texcoord).r;

	// mix detail textures based on texture repeating value and grayscale/percentage
	vec4 TextureColor = mix(texture2D(DetailTex[0], Texcoord * TextureRepeat), texture2D(DetailTex[1], Texcoord * TextureRepeat), grayscale);

	// calculate snow and sand texture values
	// get current pixels height
	float pixelHeight = Position.y;

	float BeginSand_Y = 0;
	float EndSand_Y = 1;
	float BeginSnow_Y = 4;
	float EndSnow_Y = 6;

	// mix texture with sand until pixelHeight is greater than EndSand_Y
	if (pixelHeight <= EndSand_Y)
	{
		// interpolate from sand texture (opacity 1) to nothing (opacity 0)
		float interpolate = (EndSand_Y - pixelHeight) / (EndSand_Y - BeginSand_Y);
		TextureColor = mix(TextureColor, texture2D(SandTex, Texcoord * TextureRepeat), interpolate);
	}
	// mix texture with snow when pixelHeight is greater or equal to BeginSnow_Y
	else if (pixelHeight >= BeginSnow_Y)
	{
		// interpolate from sand texture (opacity 1) to nothing (opacity 0)
		float interpolate = (pixelHeight - BeginSnow_Y) / (EndSnow_Y - BeginSnow_Y);
		TextureColor = mix(TextureColor, texture2D(SnowTex, Texcoord * TextureRepeat), interpolate);
	}
	// ------------------------------------------------------------------------------------------------

	vec3 DiffuseComponent = LightColor * TextureColor.xyz * sat(dot(N,L));
	vec3 SpecularComponent = LightColor * SpecularColor * pow(sat(dot(R,E)), SpecularExp);
	
	// define fog values
	float dMin = 0;
	float dMax = 50;
	int a = 1; // 1 = linear, >= 2 = exponential
	vec3 fogColor = vec3(0.95, 0.95, 1);

	// get vector from eye to pixel coord
	vec3 _dist = (EyePos - Position);

	// get length of vector
	float d = length(_dist);
	
	// calculate fog
	// if the distance lies between dMin and dMax, adjust the scattering between 0 and 1
	// clamp the value between 0 and 1
	// s = 0 => 0% fog, s = 1 => 100% fog

	// s = clamp((d-dMin) / (dMax - dMin))^a, 0, 1)
	float scattering = clamp(pow(((d - dMin) / (dMax - dMin)), a), 0, 1);
	// ------------------------------------------------------------------------------------------------

	// add fog to diffuse component
	DiffuseComponent = (1 - scattering) * DiffuseComponent + scattering * fogColor;

	// finally, add all colors together
	FragColor = vec4(((DiffuseComponent + AmbientColor) + SpecularComponent), 1);

	// for debugging
	//FragColor = vec4(Normal.x ,Normal.y,Normal.z,1);
}
