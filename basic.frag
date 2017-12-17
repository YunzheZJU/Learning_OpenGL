#version 410

in vec3 Position;
in vec3 Normal;

layout( location = 0 ) out vec4 FragColor;

uniform vec4 LightPosition;      // Light position in eye coords.
uniform vec3 LightIntensity;     // Light intensity

uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity

const int levels = 3;
const float scaleFactor = 1.0 / levels;

vec3 toonShade() {
    vec3 s = normalize(vec3(LightPosition) - Position);
    float cosine = max(0.0, dot(s, Normal));
    vec3 diffuse = Kd * floor(cosine * levels) * scaleFactor;
    return LightIntensity * (Ka + diffuse);
}

void main() {
    FragColor = vec4(toonShade(), 1.0);
}
