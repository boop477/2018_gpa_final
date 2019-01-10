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
    
    //if (is_using_df == 1)
    //    color = c_sb + (c_sobj-c_snoobj);
    //else
    //    color = c_single_texture;
    color = c_single_texture;
}
