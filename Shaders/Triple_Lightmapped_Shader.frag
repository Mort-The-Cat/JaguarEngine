#version 440

uniform sampler2D Albedo_Texture;

uniform sampler2D Lightmap_0_Texture;
uniform sampler2D Lightmap_1_Texture;
uniform sampler2D Lightmap_2_Texture;

uniform float Inverse_Lightmap_Size;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec3 Lightmap_Vector_0;
in vec3 Lightmap_Vector_1;
in vec3 Lightmap_Vector_2;

in vec2 Lightmap_Coordinates;

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	vec2 LM_Coordinates = Lightmap_Coordinates * vec2(Inverse_Lightmap_Size);

	vec3 Sum_Lighting = 
		texture(Lightmap_0_Texture, LM_Coordinates).rgb * max(0, dot(Normal, Lightmap_Vector_0)) + 
		texture(Lightmap_1_Texture, LM_Coordinates).rgb * max(0, dot(Normal, Lightmap_Vector_1)) +
		texture(Lightmap_2_Texture, LM_Coordinates).rgb * max(0, dot(Normal, Lightmap_Vector_2));

	gl_FragColor = 
				vec4(
					pow(Sum_Lighting, vec3(0.5)), 
				1) * 
				vec4(
					texture(Albedo_Texture, Texture_Coordinates).rgb, 
				1);

	// gl_FragColor = texture(Lightmap_Texture, LM_Coordinates);

	//vec4(Normal * 0.5 + 0.5, 1);
}