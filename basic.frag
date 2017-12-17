#version 430

in vec3 ReflectDir;
in vec3 RefractDir;

layout (binding = 0) uniform samplerCube CubeMapTex;

uniform bool DrawSkyBox;

struct MaterialInfo {
    float Eta;
    float ReflectionFactor;
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

//vec3 phongModel(vec3 norm, vec3 diffR) {
//    vec3 r = reflect(-LightDir, norm);
//    vec3 ambient = Light.Intensity * Material.Ka;
//    float sDotN = max(dot(LightDir, norm), 0.0);
//    vec3 diffuse = Light.Intensity * diffR * sDotN;
//    vec3 spec = vec3(0.0);
//    if (sDotN > 0.0) {
//        spec = Light.Intensity * Material.Ks * pow(max(dot(r, ViewDir), 0.0), Material.Shininess);
//    }
//    return ambient + diffuse + spec;
//}

void main() {
    vec4 reflectColor = texture(CubeMapTex, ReflectDir);
    vec4 refractColor = texture(CubeMapTex, RefractDir);
    if (DrawSkyBox) {
        FragColor = reflectColor;
    }
    else {
        FragColor = mix(refractColor, reflectColor, Material.ReflectionFactor);
    }
}
