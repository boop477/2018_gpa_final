#version 410

layout(location = 0)in vec2 vv2texcoord;

uniform sampler2D diffuseTexture;

out vec4 fragColor;

void main()
{
	vec4 color = texture(diffuseTexture, vv2texcoord);
	if(color.a < 0.5)
	{
		discard;
	}
    fragColor = color;
}
