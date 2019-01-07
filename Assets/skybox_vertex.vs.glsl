#version 410 core

out VS_OUT
{
    vec3    tc;
} vs_out;

uniform mat4 inv_vp_matrix;
uniform mat4 view_matrix;
uniform vec3 eye=vec3(0.0, 0.0, 0.0);

void main(void)
{
    // 4 points on the desktop
    vec4[4] vertices = vec4[4](vec4(-1.0, -1.0, 1.0, 1.0),
                               vec4( 1.0, -1.0, 1.0, 1.0),
                               vec4(-1.0,  1.0, 1.0, 1.0),
                               vec4( 1.0,  1.0, 1.0, 1.0));
    // remap to world space
    vec4 p = inv_vp_matrix * vertices[gl_VertexID];
    p /= p.w;
    // a vector from eye to the point
    vs_out.tc = normalize(p.xyz - eye);
    
    gl_Position = vertices[gl_VertexID];
}
