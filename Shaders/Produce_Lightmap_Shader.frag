#version 440

uniform sampler2D Albedo_Texture;

uniform vec3 Light_Colour;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec2 Lightmap_Coordinates;

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	gl_FragColor = vec4(Texture_Coordinates, 1, 1); //texture(Albedo_Texture, Texture_Coordinates); // For now, do this

	//vec4(Normal * 0.5 + 0.5, 1);
}