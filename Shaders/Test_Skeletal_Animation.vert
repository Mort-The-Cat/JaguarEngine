#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in float Joint_Weight;
layout(location = 4) in uint Joint_Index;

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

uniform mat4 Joint_Matrices[64];

out vec2 Texture_Coordinates;
out vec3 Position;
out vec3 Normal;

void main()
{
	Texture_Coordinates = In_Texture_Coordinates;
	Position = vec3(
		Model_Matrix * Joint_Matrices[Joint_Index] * vec4(In_Position, 1.0f)		// 4d vector is required to apply translation to position
	);

	Normal = mat3(Model_Matrix) * (mat3(Joint_Matrices[Joint_Index]) * normalize(In_Normal)); // This provides only the ROTATION to the normals (which is what we want)

	gl_Position = Projection_Matrix * vec4(Position, 1);
}