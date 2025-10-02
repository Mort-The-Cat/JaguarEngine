#version 440

layout(location = 0) out vec4 Out_Colour;

uniform sampler2D Albedo_Texture;

uniform vec3 Light_Colour;

uniform vec3 Lightmap_Surface_Normal;
uniform vec3 Camera_Position;

in vec4 Screenspace_Position;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec2 Lightmap_Coordinates;

float Fix_Distortion(float Coord)
{
	return atan(Coord) / Coord;
}

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	vec3 Colour = vec3(max(0, dot(normalize(Position - Camera_Position), Lightmap_Surface_Normal))) * Light_Colour;

	// Colour *= abs(atan(gl_FragCoord.y) * atan(gl_FragCoord.x));

	// Colour *= Fix_Distortion(abs(Screenspace_Position.x / Screenspace_Position.w)) * Fix_Distortion(abs(Screenspace_Position.y / Screenspace_Position.w));

	Out_Colour = vec4(Colour, 1);

	//gl_FragColor = vec4(Texture_Coordinates, 1, 1); //texture(Albedo_Texture, Texture_Coordinates); // For now, do this

	//gl_FragColor = vec4(0);

	//vec4(Normal * 0.5 + 0.5, 1);
}