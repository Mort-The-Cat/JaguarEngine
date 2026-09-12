#version 440

out vec4 Out_Colour;

// Really simple program, this is just for testing purposes

uniform sampler2D Texture_Test;

uniform vec4 Colour;

void main()
{
	Out_Colour = Colour;
}