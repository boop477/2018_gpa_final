#version 410 core

uniform sampler2D tex;

out vec4 color;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	// TODO: Change the output color by computing the Red-Blue Stereo
	float offset = 0.005;
	vec4 texture_color_Left = texture(tex, fs_in.texcoord);
	vec4 texture_color_Right = texture(tex, fs_in.texcoord);
    
    float color_r = texture_color_Left.r * 0.299 +
                    texture_color_Left.g * 0.587 +
                    texture_color_Left.b * 0.114;
    float color_g = texture_color_Right.g;
    float color_b = texture_color_Right.b;
    
    //vec4 texture_color = vec4(color_r, color_g, color_b, 1.0);
    vec4 texture_color = texture(tex, fs_in.texcoord);
    color = texture_color;
}
