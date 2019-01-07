/*
 * Draw a mesh or a quad with environment mapping and blinn-phong reflection.
 *   tex_cubemap : samplerCube of skybox
 *   tex_shadow  : sampler2DShadow of depth map
 *   is_quad     : A flag showing that we are drawing quad or not. Since, unlike models,
 *                 quad use a single color, we should raise this flag when we want to draw a quad.
 *   is_shadow   : A flag showing that we should draw with shadow or not. This param should be used
 *                 when drawing Snoobj.
 */
#version 410 core
uniform sampler2D tex;
uniform samplerCube tex_cubemap;
uniform sampler2DShadow tex_shadow;
uniform int is_quad;
uniform int is_shadow;

in VS_OUT
{
    vec3 normal;
    vec3 view;
    vec2 tex_cord;
    
    vec3 N;         // normal in view space
    vec3 L;         // light vector in view space
    vec3 H;         // halfway vector in view space
    
    vec4 shadow_coord; // position in light coordinate
} fs_in;

out vec4 frag_color;

void main(void)
{
    vec3 texColor = texture(tex,fs_in.tex_cord).rgb;
    vec3 shadow_color = vec3(0.41, 0.36, 0.37);
    vec3 quad_color = vec3(0.64, 0.57, 0.49);
    // Shadow factor
    float shadow_factor = textureProj(tex_shadow, fs_in.shadow_coord);

    // Blinn Phong
    vec3 Id = vec3(0.5, 0.5, 0.5);
    vec3 Is = vec3(1.0, 1.0, 1.0);
    int specular_power = 200;
    
    vec3 Kd = vec3(0.35, 0.35, 0.35);
    vec3 Ks = vec3(0.75, 0.75, 0.75);
    
    vec3 RdKd = max(dot(fs_in.N, fs_in.L), 0.0)*Kd;
    vec3 RsKs = pow(max(dot(fs_in.N, fs_in.H), 0.0), specular_power) * Ks;
    vec4 bf_color = vec4(RdKd+RsKs, 1.0);

    // Environment mapping
    vec3 r = reflect(fs_in.view, normalize(fs_in.normal));
    vec4 env_color = texture(tex_cubemap, r);// * vec4(0.95, 0.80, 0.45, 1.0);
    
    // Drawing a model
    if(is_shadow == 1){
        frag_color = vec4(texColor, 1.0)*shadow_factor;
        if (shadow_factor >= 0.5){
            frag_color = vec4(texColor, 1.0); // No shadow
        }
        else{
            frag_color = vec4(texColor, 1.0)*vec4(0.5); // Add shadow
        }
    }
    else{
        frag_color = vec4(texColor, 1.0);
    }
    //frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    //frag_color = vec4(texColor, 1.0);
    
    // drawing a quad
    if (is_quad == 1){
        // Draw a quad
        frag_color = vec4(quad_color, 1.0);
        if(is_shadow==1 && shadow_factor<0.5){
            frag_color = vec4(shadow_color, 1.0);
        }
    }
}

