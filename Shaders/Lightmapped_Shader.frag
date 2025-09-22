#version 440

uniform sampler2D Albedo_Texture;
uniform sampler2D Lightmap_Texture;

uniform float Inverse_Lightmap_Size;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec2 Lightmap_Coordinates;

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	vec2 LM_Coordinates = Lightmap_Coordinates * vec2(Inverse_Lightmap_Size);
	// LM_Coordinates.y = 1 - LM_Coordinates.y;

	gl_FragColor = 
				vec4(
					texture(Lightmap_Texture, LM_Coordinates).rgb, 
				1) * 
				vec4(
					texture(Albedo_Texture, Texture_Coordinates).rgb, 
				1);

	//gl_FragColor = vec4(texture(Albedo_Texture, Texture_Coordinates).rgb, 1) * 
	//	vec4(
	//		vec3(
	//			(0.3 + 0.8 * max(0, dot(Position * Inverse_Length, -Normal)))		//		* Inverse_Length
	//		),
	//		1
	//	);

	// gl_FragColor = texture(Lightmap_Texture, LM_Coordinates);

	//vec4(Normal * 0.5 + 0.5, 1);
}