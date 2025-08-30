#version 440

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

void main()
{
	gl_FragColor = vec4(Normal * 0.5 + 0.5, 1);
}