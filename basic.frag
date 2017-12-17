#version 410

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D BaseTex;
uniform sampler2D AlphaTex;

struct LightInfo {
    vec4 Position;      // Light position in eye coords.
    vec3 Intensity;     // A, D, S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel(vec3 pos, vec3 norm) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(vec3(-pos));
    vec3 h = normalize(v + s);
    vec3 spec = Light.Intensity * Material.Ks * pow(max(0.0, dot(h, norm)), Material.Shininess);
    vec3 ambAndDiff = Light.Intensity * Material.Ka + Light.Intensity * Material.Kd * max(0.0, dot(s, norm));
    return ambAndDiff + spec;
}

void main() {
    vec3 ambAndDiff, spec;
    vec4 baseColor = texture(BaseTex, TexCoord);
    vec4 alphaMap = texture(AlphaTex, TexCoord);
    if (alphaMap.a < 0.15) {
        discard;
    }
    else {
        if (gl_FrontFacing) {
            FragColor = vec4(phongModel(Position, Normal), 1.0) * baseColor;
        }
        else {
            FragColor = vec4(phongModel(Position, -Normal), 1.0) * baseColor;
        }
    }
}
