#version 440

out vec4 Out_Colour;

// Really simple program, this is just for testing purposes

uniform vec3 Camera_Position;

in vec3 Normal;
in vec3 Position;

void main()
{
	float Phong = 0.5 + 0.5 * dot(Normal, normalize(Camera_Position - Position));
	

	Out_Colour = vec4(0.707, 0.606, 0.707, 0.5) * Phong;
}