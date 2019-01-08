#version 410 core

out VS_OUT
{
    vec2 texcoord;
} vs_out;

void main()
{
    // Screen coordinate
    vec4 positions[4] = vec4[](
         vec4(-1.0, -1.0, 0.0, 1.0),
         vec4( 1.0, -1.0, 0.0, 1.0),
         vec4(-1.0,  1.0, 0.0, 1.0),
         vec4( 1.0,  1.0, 0.0, 1.0));
    
    // Map from screen coordinate[-1, 1] to texture coordinate[0, 1]
    vs_out.texcoord = positions[gl_VertexID].xy * 0.5 + 0.5;
    
    gl_Position = positions[gl_VertexID];
}
