#version 440

uniform sampler2D Albedo_Texture;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec2 Lightmap_Coordinates;

void main()
{
	float Inverse_Length = inversesqrt(dot(Position, Position));

	gl_FragColor = vec4(texture(Albedo_Texture, Texture_Coordinates).rgb, 1) * 
		vec4(
			vec3(
				(0.3 + 0.8 * max(0, dot(Position * Inverse_Length, -Normal)))		//		* Inverse_Length
			),
			1
		);

	//vec4(Normal * 0.5 + 0.5, 1);
}