#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in uint In_Joints;
layout(location = 4) in vec4 In_Weights;

// Really simple program, this is just for testing purposes

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

uniform mat4 Joints[64];

out vec2 Texture_Coordinates;
out vec3 Position;
out vec3 Normal;

void main()
{
	vec3 Local_Position = vec3(0);

	vec3 Local_Normal = vec3(0);

	mat4 Joint_Matrix = mat4(0);

	for(int Bone = 0; Bone < 4; Bone++)	// 4 joints per vertex
	{
		float Weight = In_Weights[Bone];
		uint Index = (In_Joints >> (Bone << 3)) & 0xFFu;

		Joint_Matrix += Weight * Joints[Index];
	}

	Local_Position = (Joint_Matrix * vec4(In_Position, 1)).xyz;
	Local_Normal = mat3(Joint_Matrix) * In_Normal;

	vec4 Point = Model_Matrix * vec4(Local_Position, 1);

	Position = Point.xyz;

	gl_Position = Projection_Matrix * vec4(Point.xyz, 1.0f);

	Normal = normalize(mat3(Model_Matrix) * Local_Normal);
	Texture_Coordinates = In_Texture_Coordinates;
}
