#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_Texture_Coordinate;

uniform mat3 Transform;	// This matrix elegantly includes origin, size, and right/down vectors
float Layer;			// just which layer this UI element is on

void main()
{
	vec2 Position = (Transform * vec3(In_Position, 1.0f)).xy;

	Position.y *= -1.0f;

	gl_Position = vec4(vec3(Position, Layer), 1.0f);	//			'Layer' is for the depth buffer
}