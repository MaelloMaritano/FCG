#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertex_color;

uniform float theta_deg;
uniform float phi_deg;

out vec3 interpolated_color;

void main()
{
    float rho=-2.0;

    float fd=2.0;
    float ncp=1.0;
    float fcp=3.0;

    float theta=radians(theta_deg);
    float phi=radians(phi_deg);

    mat4 Rx=mat4 (
        vec4(1.0,   0.0,   0.0,   0.0),
        vec4(0.0, cos(phi), sin(phi),   0.0),
        vec4(0.0,-sin(phi), cos(phi),   0.0),
        vec4(0.0,   0.0,   0.0,   1.0) );

    mat4 Ry=mat4 (
        vec4(cos(theta),  0.0, -sin(theta),  0.0),
        vec4(0.0,    1.0,   0.0,   0.0),
        vec4(sin(theta),  0.0,  cos(theta),  0.0),
        vec4(0.0,    0.0,   0.0,   1.0) );

    mat4 T=mat4 (
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, rho, 1.0) );

    mat4 P=mat4 (
        vec4(fd,  0.0, 0.0, 0.0),
        vec4(0.0, fd,  0.0, 0.0),
        vec4(0.0, 0.0, -((fcp+ncp)/(fcp-ncp)), -1.0),
        vec4(0.0, 0.0, -((2.0*fcp*ncp)/(fcp-ncp)), 0.0)
    );

    

    mat4 ViewMatrix=T*Rx*Ry;
    mat4 MVP=P*ViewMatrix;

    gl_Position=MVP*vec4(position, 1.0);
    interpolated_color=vertex_color;
}
