#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in float In_Padding;

out vec2 Texture_Coordinates;
out vec3 Position;
out vec3 Normal;

void main()
{
	Texture_Coordinates = In_Texture_Coordinates;
	Position = In_Position;
	Normal = normalize(In_Normal);
	gl_Position = vec4(In_Position/20 - vec3(-0.1, 0.35, 0.5), In_Position.z/20 - 0.5);
}