#version 440

layout(location = 0) out vec3 Out_Component_0;

uniform sampler2D Lightmap_0_Texture;
void main()
{
	Out_Component_0 = textureLod(Lightmap_0_Texture, vec2(0.0), 8).xyz / 5.5;
}