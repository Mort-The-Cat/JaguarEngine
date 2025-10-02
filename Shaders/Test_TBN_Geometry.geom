#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec4 Position;
out vec3 Normal;

out vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
out vec3 Texture_Tangent;		// Tangent, based on texture space
out vec3 Texture_Bitangent;		// Bitangent, based on texture space
out vec2 Texture_Coordinates;
out vec2 Lightmap_Coordinates;

in DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 Lightmap_Coordinates;

	vec2 UV;
} Vertex[];

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
		Lightmap_Coordinates = Vertex[W].Lightmap_Coordinates;

		EmitVertex();
	}

	EndPrimitive();
}