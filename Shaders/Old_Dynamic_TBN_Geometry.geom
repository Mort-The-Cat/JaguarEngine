#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec3 Lighting_Node_Uniform_0[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_1[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_2[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_3[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Positions[4];	// these are the positions we'll use to interpolate on a vertex-basis

out vec3 Lighting_Node[6];

out vec3 Position;
out vec3 Normal;

out vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
out vec3 Texture_Tangent;		// Tangent, based on texture space
out vec3 Texture_Bitangent;		// Bitangent, based on texture space
out vec2 Texture_Coordinates;

in DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 UV;
} Vertex[];

void Interpolate_Lighting_Nodes(vec3 Sample_Position)
{
	float Distances[4];
	float Inverse_Distance = 0;

	for(int Index = 0; Index < 4; Index++)
	{
		vec3 Delta = Lighting_Node_Positions[Index] - Sample_Position;
		Distances[Index] = length(Delta);
		Inverse_Distance += Distances[Index];
	}

	Inverse_Distance = 0.25f / Inverse_Distance;

	float A, B, C, D;

	A = (Distances[1] + Distances[2] + Distances[3]) * Inverse_Distance;
	B = (Distances[0] + Distances[2] + Distances[3]) * Inverse_Distance;
	C = (Distances[1] + Distances[0] + Distances[3]) * Inverse_Distance;
	D = (Distances[1] + Distances[2] + Distances[0]) * Inverse_Distance;

	//Lighting_Node = 
	//	A * Lighting_Node_Uniform_0 + 
	//	B * Lighting_Node_Uniform_1 +
	//	C * Lighting_Node_Uniform_2;			// This likely won't work because they're arrays

	for(int Index = 0; Index < 6; Index++)
	{
		Lighting_Node[Index] = 
			A * Lighting_Node_Uniform_0[Index] + 
			B * Lighting_Node_Uniform_1[Index] + 
			C * Lighting_Node_Uniform_2[Index] +
			D * Lighting_Node_Uniform_3[Index];
	}
}

void main()
{
	vec2 UV_A = Vertex[1].UV - Vertex[0].UV;
	vec2 UV_B = Vertex[2].UV - Vertex[0].UV;

	vec3 Edge_A;
	vec3 Edge_B;

	Edge_A = Vertex[1].Position.xyz - Vertex[0].Position.xyz;
	Edge_B = Vertex[2].Position.xyz - Vertex[0].Position.xyz;

	float Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

	vec3 Tangent = -normalize(Inv * (UV_B.y * Edge_A - UV_A.y * Edge_B));

	//

	for(int W = 0; W < 3; W++)
	{
		gl_Position = gl_in[W].gl_Position;
		Position = Vertex[W].Position;
		Normal = Vertex[W].Normal;
		Texture_Coordinates = Vertex[W].UV;
		Texture_Tangent = Tangent;
		Texture_Bitangent = cross(Tangent, Normal);

		Geometry_Normal = Normal;

		Interpolate_Lighting_Nodes(Vertex[W].Position);

		// Lighting_Node = Lighting_Node_Uniform_0;

		EmitVertex();
	}

	EndPrimitive();
}