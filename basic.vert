#version 430

layout(location = 0) in vec3 VertexInitVel;
layout(location = 1) in float StartTime;

out float Transp;

uniform float Time;
uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);
uniform float ParticleLifetime;

uniform mat4 MVP;

void main() {
    vec3 pos = vec3(0.0);
    Transp = 0.0;
    if (Time > StartTime) {
        float t = Time - StartTime;
        if (t < ParticleLifetime) {
            pos = VertexInitVel * t + Gravity * t * t;
            Transp = 1.0 - t / ParticleLifetime;
        }
    }
    gl_Position = MVP * vec4(pos, 1.0);
}