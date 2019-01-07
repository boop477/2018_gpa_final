#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

out VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;

void main()
{
    mat4 um4p = proj_matrix;
    mat4 um4mv = view_matrix * model_matrix;
    gl_Position = um4p * um4mv * vec4(iv3vertex, 1.0);
    vertexData.texcoord = iv2tex_coord;
    vertexData.N = vec3((iv3normal.x+1.0)/2.0, (iv3normal.y+1.0)/2.0, (iv3normal.z+1.0)/2.0);
}
