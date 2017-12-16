//
// Created by Yunzhe on 2017/12/5.
//

#ifndef GPUBASEDRENDERING_A1_SHADER_H
#define GPUBASEDRENDERING_A1_SHADER_H

#include "global.h"
#include "textfile.h"

// Using namespace std for cout
using namespace std;

class Shader {
protected:
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLfloat angle;
public:
    void init();
    void set(const char *vertexShaderFile, const char *fragmentShaderFile);
    void enable();
    void disable();
    void logShader(GLuint shader);
    void logProgram();
    GLuint getShaderProgram();
};

#endif //GPUBASEDRENDERING_A1_SHADER_H
