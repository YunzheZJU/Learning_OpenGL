//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_A1_SYSTEM_H
#define GPUBASEDRENDERING_A1_SYSTEM_H

// Include related head files
#include "global.h"
#include "utility.h"
#include "draw.h"
#include "light.h"
#include "textfile.h"
#include "Shader.h"

// Using namespace std for cout
using namespace std;

//extern GLuint vaoHandle;
extern Shader shader;

void Idle();
void Redraw();
void Reshape(int width, int height);
void ProcessMouseClick(int button, int state, int x, int y);
void ProcessMouseMove(int x, int y);
void ProcessFocus(int state);
void ProcessNormalKey(unsigned char k, int x, int y);
void ProcessSpecialKey(int k, int x, int y);
void PrintStatus();
void SetBufferedObjects();
void setShaders();

#endif //GPUBASEDRENDERING_A1_SYSTEM_H
