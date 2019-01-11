/*
 * Draw a mesh or a quad with environment mapping and blinn-phong reflection
 *   model_matrix: model coord -> world coord
 *   view_matrix: world coord -> camera coord
 *   proj matrix: camera coord -> homogeneous coord
 *   light_mvp_matrix: Umm, a mvp matrix of a imaginary camera located in the light position. This mvp matrix is used get the shadow coordinate in homogeneous coord.
 *   light_pos: The position of the light in WORLD SPACE.
 */

#version 410 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform mat4 light_mvp_matrix;
//uniform vec3 light_pos = vec3(5.00, 5.00, 5.00); // light position in WORLD SPACE!
uniform vec3 light_pos = vec3(2.50, 3.00, 5.00);

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_cord;
layout(location = 3) in vec3 iv3tangent;
layout(location = 4) in vec3 iv2bitangent;

out VS_OUT
{
    vec3 normal;
    vec3 view;
    vec2 tex_cord;
    
    vec3 N;         // normal in camera space
    vec3 L;         // light vector in camera space
    vec3 H;         // halfway vector in camera space
    
    vec4 shadow_coord; // position in light coordinate
} vs_out;

void main(void)
{
    mat4 _mv_matrix = view_matrix * model_matrix;
    
    // Texture(0, 1)
    vs_out.tex_cord = tex_cord;
    
    // Shadow
    vs_out.shadow_coord = light_mvp_matrix * vec4(position,1);
    
    // BP
    vec4 P = _mv_matrix * vec4(position,1);      // Position in camera space
    vec4 light_view_space = view_matrix * vec4(light_pos, 1.0);  // we need to convert the light position from world space to camera space by multiplying the view matrix.
    
    vs_out.N = mat3(_mv_matrix) * normal;           // Normal in view space
    vs_out.L = light_view_space.xyz - P.xyz;
    vec3 v = -P.xyz;
    vs_out.H = normalize((light_view_space.xyz - P.xyz) + v);
    
    // Environment mapping
    vec4 pos_vs = _mv_matrix * vec4(position,1);
    vs_out.normal = mat3(_mv_matrix) * normal;
    vs_out.view = pos_vs.xyz;
    
    gl_Position = proj_matrix * pos_vs;
}
