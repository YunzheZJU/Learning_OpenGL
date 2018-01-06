#version 430

in float Transp;

layout(binding = 0) uniform sampler2D ParticleTex;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = texture(ParticleTex, gl_PointCoord);    // Enable GL_POINT_SPRITE first
    FragColor.a *= Transp;
}