#version 440

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

void main()
{
	gl_FragColor = vec4(
		vec3(
			0.1 + 0.8 * dot(-normalize(Position), normalize(Normal)) // Simple light at (0, 0, 0)
		),
		1
	);   

	//vec4(Normal * 0.5 + 0.5, 1);
}