#version 410 core

//uniform vec3 foreground;
in vec3 color;  //diverso da colore uscito da vertex, colore interpolato dei vertici
out vec4 frag_color;

void main()
{
    frag_color = vec4 (color, 1.0);
}
