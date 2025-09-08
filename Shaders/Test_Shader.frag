#version 440

uniform sampler2D Albedo_Texture;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

void main()
{
	gl_FragColor = vec4(texture(Albedo_Texture, Texture_Coordinates).rgb, 1);

	//vec4(Normal * 0.5 + 0.5, 1);
}