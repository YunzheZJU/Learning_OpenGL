#version 430

in vec3 ReflectDir;

layout (binding = 0) uniform samplerCube CubeMapTex;

uniform bool DrawSkyBox;
uniform float ReflectFactor;
uniform vec4 MaterialColor;

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
    vec4 cubeMapColor = texture(CubeMapTex, ReflectDir);
    if (DrawSkyBox) {
        FragColor = cubeMapColor;
    }
    else {
        FragColor = mix(MaterialColor, cubeMapColor, ReflectFactor);
    }
}
