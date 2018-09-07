#version 400
layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;
layout(location=3) in vec2 VertexTexcoord1;
layout(location=4) in vec2 VertexTexcoord2;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;

void main()
{
	Position = (ModelMat * VertexPos).xyz;
	Normal = (ModelMat * vec4(VertexNormal.xyz,0)).xyz;
	Texcoord = VertexTexcoord;

	// Praktikum 6 - Exercise 3

	// Tangent calculation
	// needs to be turned into a vec3
	Tangent = vec3(VertexTexcoord1.x, VertexTexcoord1.y, 1 - abs(VertexTexcoord1.x) - abs(VertexTexcoord1.y));

	// Bitangent calculation
	// Basically the perpendicular vector that sits on N and T?
	// No need to calculate, just get cross product of N and T?
	Bitangent = cross(Normal, Tangent);

	// Scale Tangent and Bitangent accordingly
	Tangent = (ModelMat * vec4(Tangent.xyz, 0)).xyz;
	Bitangent = (ModelMat * vec4(Bitangent.xyz, 0)).xyz;

	gl_Position = ModelViewProjMat * VertexPos;
}

