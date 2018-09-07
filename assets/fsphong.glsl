#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

const int MAX_LIGHTS = 14;

struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
	int LightCount;
	Light lights[MAX_LIGHTS];
};

float sat( in float a)
{
	return clamp(a, 0.0, 1.0);
}

void main()
{
	vec4 DiffTex = texture( DiffuseTexture, Texcoord);
	if(DiffTex.a < 0.3f) discard;

	vec3 N = normalize(Normal);
	vec3 E = normalize(EyePos - Position);

	// old
//	vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
//	vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
//	FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);

	// exercise 1
//	vec3 H = normalize(E+L);
//	vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
//	vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
//	FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);

	// exercise 3
	mat3 matrix = mat3(Tangent, Bitangent, Normal);
	vec3 normalMapN = texture(NormalTexture, Texcoord).rgb;

	// X = R*2-1 , Y = G*2-1 , Z = B*2-1
	normalMapN = vec3(normalMapN.r * 2 - 1, normalMapN.g * 2 - 1, normalMapN.b * 2 - 1);
	// normalize to world space
	N = normalize(matrix * normalMapN);

	vec3 DiffuseComponent = vec3(0,0,0);
	vec3 SpecularComponent = vec3(0,0,0);
	
	// exercise 2
	// calculate per light source
	for(int i = 0; i < LightCount; ++i)
	{
		// get distance between light and vertex position
		float dist = length(lights[i].Position - Position);

		// some per light calculations
		vec3 L = normalize(lights[i].Position - Position);
		vec3 R = reflect(-L,N);
		//vec3 H = normalize(E+L);
		
		vec3 Etest = normalize(Position - EyePos);
		vec3 Ltest = normalize(Position - lights[i].Position);
		vec3 H = normalize(Etest+Ltest);
		
//		vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
//		vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
		
		/*
		enum LightType
		{
			POINT = 0,
			DIRECTIONAL,
			SPOT
		};
		*/

		float intensity = 0.f;

		switch(lights[i].Type)
		{
			case 0:
//				float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
				intensity = 1.0f / (lights[i].Attenuation.x + lights[i].Attenuation.y * dist + lights[i].Attenuation.z * dist * dist);
				break;
			case 1:
				//vec3 lightDirection = normalize(-lights[i].Direction);
				intensity = 1.f;
				break;
			case 2:
				float angle = acos(sat(dot(-L, lights[i].Direction)));
				intensity = 1.0f / (lights[i].Attenuation.x + lights[i].Attenuation.y * dist + lights[i].Attenuation.z * dist * dist);
				intensity *= 1 - sat((angle - lights[i].SpotRadius.x) / (lights[i].SpotRadius.y - lights[i].SpotRadius.x));				
				break;
		}

		//Color += lights[i].Color;
		DiffuseComponent += lights[i].Color * intensity * sat(dot(N,L));
		SpecularComponent += lights[i].Color * intensity * pow(sat(dot(N,H)), SpecularExp);
	}

	DiffuseComponent *= DiffuseColor;
	SpecularComponent *= SpecularColor;
	
	FragColor = vec4((DiffuseComponent + AmbientColor) * DiffTex.rgb + SpecularComponent, DiffTex.a);
	//FragColor = vec4(N, 1);
}
