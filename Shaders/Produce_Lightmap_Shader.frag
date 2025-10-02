#version 440

layout(location = 0) out vec3 Out_Colour;

uniform sampler2D Albedo_Texture;

uniform vec3 Light_Colour;

uniform vec3 Lightmap_Surface_Normal;
uniform vec3 Camera_Position;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec2 Lightmap_Coordinates;

void main()
{
	vec3 Colour = vec3(max(0, dot(normalize(Position - Camera_Position), Lightmap_Surface_Normal))) * Light_Colour;

	Out_Colour = Colour;
}