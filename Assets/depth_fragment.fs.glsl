/*
 * Fragment shader to draw a depth map
 */
#version 410 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z, 1.0);
}
