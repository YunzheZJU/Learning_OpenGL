#version 410

in vec3 Position;
in vec3 Normal;

layout( location = 0 ) out vec4 FragColor;

struct FogInfo {
    float maxDist;
    float minDist;
    vec3 color;
};
uniform FogInfo Fog;

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
    vec3 ambient = Ka * LightIntensity;
    vec3 diffuse = LightIntensity * Kd * max(0.0, dot(s, Normal));
    vec3 spec = LightIntensity * Ks * pow(max(0.0, dot(h, Normal)), Shininess);
    return ambient + diffuse + spec;
}

void main() {
//    float dist = abs(Position.z);
    float dist = length( Position.xyz );
    float fogFactor = (Fog.maxDist - dist) / (Fog.maxDist - Fog.minDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 shadeColor = ads();
    vec3 color = mix(Fog.color, shadeColor, fogFactor);
    FragColor = vec4(color, 1.0);
}
