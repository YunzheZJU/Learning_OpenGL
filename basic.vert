#version 430

layout(location = 0) in vec3 VertexPosition;

out vec4 Position;
out vec3 Normal;

uniform float Time;

uniform float Freq = 2.5;
uniform float Velocity = 2.5;
uniform float Amp = 0.6;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main() {
    vec4 pos = vec4(VertexPosition, 1.0);
    // Translate the y coordinate
    float u = Freq * pos.x - Velocity * Time;
    pos.y = Amp * sin(u);
    // Compute the normal vector;
    vec3 n = vec3(0.0);
    n.xy = normalize(vec2(cos(u), 1.0));
    // Send position and normal (in camera cords) to frag
    Position = ModelViewMatrix * pos;
    Normal = NormalMatrix * n;
    // The position in clip coordinates
    gl_Position = MVP * pos;
}