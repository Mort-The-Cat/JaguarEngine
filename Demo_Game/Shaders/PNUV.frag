#version 440

out vec4 Out_Colour;

// Really simple program, this is just for testing purposes

uniform sampler2D Texture_Test;

uniform vec3 Camera_Position;

in vec3 Normal;
in vec3 Position;
in vec2 Texture_Coordinates;

void main()
{
	float Phong = 0.5 + 0.5 * dot(Normal, normalize(Camera_Position - Position));
	

	Out_Colour = vec4(Phong * texture(Texture_Test, Texture_Coordinates).xyz, 1);
	
	//vec4(0.707, 0.606, 0.707, 0.5) * Phong;
}