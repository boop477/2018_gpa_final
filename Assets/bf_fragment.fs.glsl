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
uniform sampler2D tex; // Not being used
uniform sampler2D tex_normal_map; // Not being used

uniform sampler2D tex_ssao;
uniform samplerCube tex_cubemap;
uniform sampler2DShadow tex_shadow;

uniform int is_quad;
uniform int is_shadow;
uniform int is_ssao;
uniform int is_normal_map;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_height1;

layout(location = 0) out vec4 fragColor;


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



void main()
{
    // == Preprocessing == //
    vec3 texColor = texture(texture_diffuse1, fs_in.tex_cord).rgb; // lost_emrpir-RGBA.png
    float ambient = texelFetch(tex_ssao, ivec2(gl_FragCoord.xy), 0).r; // ssao map
    vec3 normal_map = texture(texture_normal1, fs_in.tex_cord).rgb; // Normal map
    normal_map = 2.0*normal_map - vec3(1.0); // normal vector from normal map
    normal_map = normalize(normal_map);
    
    vec4 final_color;
    
    vec3 shadow_color = vec3(0.41, 0.36, 0.37);
    vec3 quad_color = vec3(0.64, 0.57, 0.49);
    
    // Shadow factor
    float shadow_factor = textureProj(tex_shadow, fs_in.shadow_coord);
    
    // Blinn Phong
    vec3 Id = vec3(0.5, 0.5, 0.5);
    vec3 Is = vec3(1.0, 1.0, 1.0);
    int specular_power = 10;
    
    vec3 Kd = texColor;
    vec3 Ks = vec3(3.0);
    float nmap_coef = 0.2;
    
    // == BF Shading == //
    vec3 RdKd;
    vec3 RsKs;
    vec3 RaKa;
    if (is_normal_map == 1){
        // Use normal map
        RdKd = max(dot(normal_map*nmap_coef, normalize(fs_in.L)), 0.0)*Kd;
        RsKs = pow(max(dot(normal_map*nmap_coef, normalize(fs_in.H)), 0.0), specular_power) * Ks;
    }
    else{
        // Use normal
        RdKd = max(dot(fs_in.N, normalize(fs_in.L)), 0.0)*Kd;
        RsKs = pow(max(dot(fs_in.N, normalize(fs_in.H)), 0.0), specular_power) * Ks;
    }
    RaKa = vec3(texColor*1.0*ambient);
    final_color = vec4(RdKd+RsKs+RaKa, 1.0);
    
    // == Environment Mapping == //
    vec3 r = reflect(fs_in.view, normalize(fs_in.normal));
    vec4 env_color = texture(tex_cubemap, r);// * vec4(0.95, 0.80, 0.45, 1.0);
    
    // == Shadow == //
    if(is_shadow == 1){
        if (shadow_factor < 0.5){
            final_color *= vec4(0.5); // With shadow
        }
    }
    
    fragColor = final_color;
    //fragColor = vec4(1.0);
}
