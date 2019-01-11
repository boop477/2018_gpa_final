/*
 * Draw a fbo object to screen.
 *   is_using_df: A flag to show that we should use multiple textures(differential rendering)
 *                or a single texture.
 *   tex        : sampler2D we used while drawing without differential rendering
 *   (tex_sobj
 *    tex_snoobj
 *    tex_sb)   : sampler2Ds we used while drawing with differential rendering.
 */
#version 410 core

/*uniform sampler2D tex_sobj;
uniform sampler2D tex_snoobj;
uniform sampler2D tex_sb;*/
uniform sampler2D tex;
uniform int is_using_df;
uniform int filter_index;
uniform float time;

out vec4 color;

in VS_OUT
{
    vec2 texcoord;
} fs_in;

void main()
{
    // color = Sb + (sobj - snoobj)
    //vec4 c_sobj = texture(tex_sobj, fs_in.texcoord);
    //vec4 c_snoobj = texture(tex_snoobj, fs_in.texcoord);
    //vec4 c_sb = texture(tex_sb, fs_in.texcoord);
    vec4 c_single_texture = texture(tex, fs_in.texcoord);
    
    if (filter_index == 1){
        /*float pi = 3.1415926;
        float offset = time;
        float u = fs_in.texcoord.x + 0.1*sin(fs_in.texcoord.y*3.0*pi+offset);
        float v = fs_in.texcoord.y;*/
        //c_single_texture.r = c_single_texture.r * time;
        //c_single_texture = texture(tex, vec2(u, v));
        float offset = time;
        vec4 texture_color_Left = texture(tex, fs_in.texcoord - vec2(offset, 0.0));
        vec4 texture_color_Right = texture(tex, fs_in.texcoord + vec2(offset, 0.0));
        float Color_R = texture_color_Left.r*0.299+texture_color_Left.g*0.587+texture_color_Left.b*0.114;
        float Color_G = texture_color_Right.g;
        float Color_B = texture_color_Right.b;
        vec4 texture_color = vec4(Color_R, Color_G, Color_B, 1.0);
        color = texture_color;
    }else{
        color = c_single_texture;
    }
    
    
}
