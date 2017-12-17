#version 410

in vec3 Position;
in vec3 Normal;

layout( location = 0 ) out vec4 FragColor;

uniform vec4 LightPosition;      // Light position in eye coords.
uniform vec3 LightIntensity;     // Light intensity

uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

vec3 ads() {
    vec3 s = normalize(vec3(LightPosition) - Position);
    vec3 v = normalize(vec3(-Position));
    vec3 h = normalize(v + s);
//    vec3 r = reflect(-s, n);
    return LightIntensity * (Ka + Kd * max(dot(s, Normal), 0.0) + Ks * pow(max(dot(h, Normal), 0.0), Shininess));
}

void main() {
    FragColor = vec4(ads(), 1.0);
}
