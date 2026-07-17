#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;

// Really simple program, this is just for testing purposes



// vec4 Vertices[] = { 
// 	vec4(-0.3f, 0.3f, 1.0f, 1.0f),
// 	vec4(3.0f, 0.3f, 1.0f, 1.0f),
// 	vec4(-0.0f, -0.3f, 1.0f, 1.0f)
// };



uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

out vec2 Texture_Coordinates;
out vec3 Position;
out vec3 Normal;

void main()
{
	vec4 Point = Model_Matrix * vec4(In_Position, 1);
	//Model_Matrix * Vertices[gl_VertexID];

	Position = Point.xyz;

	gl_Position = Projection_Matrix * vec4(Point.xyz, 1.0f);

	Normal = mat3(Model_Matrix) * In_Normal;
	Texture_Coordinates = In_Texture_Coordinates;
}
