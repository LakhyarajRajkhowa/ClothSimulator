#version 330 core

layout(location = 0) in vec2 aPos; // x, y position
layout(location = 1) in vec3 aColor; // optional if needed for particles, can ignore

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    gl_PointSize = 5.0; // default point size; can be overridden in code
}
