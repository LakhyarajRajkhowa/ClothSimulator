#version 330 core

uniform vec3 uColor; // color for both particles and springs

out vec4 FragColor;

void main()
{
    // Simple point rendering (square points)
    FragColor = vec4(uColor, 1.0);
}
