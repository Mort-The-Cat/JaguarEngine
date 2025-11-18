#version 440

uniform sampler2D Albedo_Texture;
uniform sampler2D Normal_Texture;

uniform vec3 Lighting_Node[6];	// Illumination for each cardinal direction of the lighting node
// (we can apply fancy interpolation to this later)

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
in vec3 Texture_Tangent;		// Tangent, based on texture space
in vec3 Texture_Bitangent;		// Bitangent, based on texture space

vec3 Final_Normal;

void Get_Final_Normal()
{
	mat3 TBN_Matrix = mat3(Texture_Tangent, Texture_Bitangent, Geometry_Normal);

	vec3 Sampled_Normal = vec3(-2, -2, 1) * texture(Normal_Texture, Texture_Coordinates).xyz + vec3(1, 1, 0);

	Final_Normal = normalize(TBN_Matrix * Sampled_Normal);
}

vec3 Lighting()
{
	// Gets lighting for all 3 axes

	vec3 Light = vec3(0);

	for(int Face = 0; Face < 3; Face++)	// for all 3 faces
	{
		int Index = Face + 3 * int(Final_Normal[Face] < 0);			// If negative, we use the lighting on that axis' negative direction
		Light += Lighting_Node[Index] * abs(Final_Normal[Face]);	// Simpler than dot-product
	}

	// We'll add dynamic light sources later

	return Light;
}

void main()
{
	Get_Final_Normal();	// This just calculates the normal-mapping

	vec3 Illumination = Lighting();

	gl_FragColor = vec4(
		texture(Albedo_Texture, Texture_Coordinates).rgb * Illumination,
	1);

	//vec4(Normal * 0.5 + 0.5, 1);
}