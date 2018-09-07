#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;

void main()
{
	Position = (ModelMat * VertexPos).xyz;

	// scale model position in world
	Position.x *= Scaling.x;
	Position.y *= Scaling.y;
	Position.z *= Scaling.z;
	
	Normal = (ModelMat * vec4(VertexNormal.xyz/Scaling,0)).xyz;
	
	Texcoord = VertexTexcoord;
	
	// scale model vertices in world
	vec4 vertexPosScaled = vec4(VertexPos);
	vertexPosScaled.x *= Scaling.x;
	vertexPosScaled.y *= Scaling.y;
	vertexPosScaled.z *= Scaling.z;
	
	gl_Position = ModelViewProjMat * vertexPosScaled;
}
