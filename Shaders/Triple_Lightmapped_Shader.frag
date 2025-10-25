#version 440

uniform sampler2D Albedo_Texture;
uniform sampler2D Normal_Texture;

uniform sampler2D Lightmap_0_Texture;
uniform sampler2D Lightmap_1_Texture;
uniform sampler2D Lightmap_2_Texture;

uniform float Inverse_Lightmap_Size;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
in vec3 Texture_Tangent;		// Tangent, based on texture space
in vec3 Texture_Bitangent;		// Bitangent, based on texture space

in vec3 Lightmap_Vector_0;
in vec3 Lightmap_Vector_1;
in vec3 Lightmap_Vector_2;

in vec2 Lightmap_Coordinates;

vec3 Final_Normal;

void Get_Final_Normal()
{
	mat3 TBN_Matrix = mat3(Texture_Tangent, Texture_Bitangent, Geometry_Normal);

	vec3 Sampled_Normal = vec3(-2, -2, 1) * texture(Normal_Texture, Texture_Coordinates).xyz + vec3(1, 1, 0);

	Final_Normal = normalize(TBN_Matrix * Sampled_Normal);
}

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	vec2 LM_Coordinates = Lightmap_Coordinates * vec2(Inverse_Lightmap_Size);

	Get_Final_Normal();

	vec3 Sum_Lighting = 
		texture(Lightmap_0_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_0)) + 
		texture(Lightmap_1_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_1)) +
		texture(Lightmap_2_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_2));

	gl_FragColor = 
				vec4(
					pow(Sum_Lighting, vec3(1.0)), 
				1) * 
				vec4(
					texture(Albedo_Texture, Texture_Coordinates).rgb, 
				1);

	//gl_FragColor = texture(Lightmap_Texture, LM_Coordinates);

	//vec4(Normal * 0.5 + 0.5, 1);
}