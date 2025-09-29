#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in vec2 In_Lightmap_Coordinates;

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

out vec2 Texture_Coordinates;
out vec3 Position;
out vec3 Normal;
out vec2 Lightmap_Coordinates;

out vec4 Screenspace_Position;

void main()
{
	Lightmap_Coordinates = In_Lightmap_Coordinates;
	Texture_Coordinates = In_Texture_Coordinates;
	Position = vec3(
		Model_Matrix * vec4(In_Position, 1.0f)		// 4d vector is required to apply translation to position
	);

	// Position = vec3(Lightmap_Coordinates, 1);

	Normal = mat3(Model_Matrix) * normalize(In_Normal); // This provides only the ROTATION to the normals (which is what we want)

	Screenspace_Position = Projection_Matrix * vec4(Position, 1);

	gl_Position = Screenspace_Position;
}