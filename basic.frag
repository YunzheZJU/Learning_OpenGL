#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D RenderTex;
layout(binding = 1) uniform sampler2D BlurTex1;
layout(binding = 2) uniform sampler2D BlurTex2;

uniform float Garmma;
uniform int Width;
uniform int Height;
uniform float LumThresh;
uniform float PixOffset[10] = float[](0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
uniform float Weight[10];

subroutine vec4 RenderPassType();
subroutine uniform RenderPassType RenderPass;

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

layout(location = 0) out vec4 FragColor;

float luma(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

vec3 phongModel(vec3 pos, vec3 norm) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(vec3(-pos));
    vec3 h = normalize(v + s);
    return Light.Intensity * Material.Ka + Light.Intensity * Material.Kd * max(0.0, dot(s, norm)) +
        Light.Intensity * Material.Ks * pow(max(0.0, dot(h, norm)), Material.Shininess);
}

// Pass #1
subroutine (RenderPassType)
vec4 pass1() {
    return vec4(phongModel(Position, Normal), 1.0);
}

// Pass #2
subroutine (RenderPassType)
vec4 pass2() {
    vec4 val = texture(RenderTex, TexCoord);
//    return val * clamp(luma(val.rgb) - LumThresh, 0.0, 1.0) * (1.0 / (1.0 - LumThresh));
    if( luma(val.rgb) > LumThresh )
        return val;
    else
        return vec4(0.0);
}

// Pass #3
subroutine(RenderPassType)
vec4 pass3() {
    float dy = 1.0 / float(Height);
    vec4 sum = texture(BlurTex1, TexCoord) * Weight[0];
    for (int i = 1; i < 10; i++) {
        sum += texture(BlurTex1, TexCoord + vec2(0.0, PixOffset[i]) * dy) * Weight[i];
        sum += texture(BlurTex1, TexCoord - vec2(0.0, PixOffset[i]) * dy) * Weight[i];
    }
    return sum;
}

// Pass #4
subroutine(RenderPassType)
vec4 pass4() {
    float dx = 1.0 / float(Width);
    vec4 val = texture(RenderTex, TexCoord);
    vec4 sum = texture(BlurTex2, TexCoord) * Weight[0];
    for (int i = 1; i < 10; i++) {
        sum += texture(BlurTex2, TexCoord + vec2(PixOffset[i], 0.0) * dx) * Weight[i];
        sum += texture(BlurTex2, TexCoord - vec2(PixOffset[i], 0.0) * dx) * Weight[i];
    }
    return pow(val + sum, vec4(1 / Garmma));
}

void main() {
    FragColor = RenderPass();
}
