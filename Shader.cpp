//
// Created by Yunzhe on 2017/12/5.
//

#include "Shader.h"

void Shader::init() {
    angle = 0.0f;
    cout << "Creating..." << endl;
    shaderProgram = glCreateProgram();
    if (shaderProgram == 0) {
        cerr << "Error occurred when creating shader program." << endl;
        exit(1);
    }
}

void Shader::set(const char *vertexShaderFile, const char *fragmentShaderFile) {
    cout << "Setting..." << endl;
    char* vertexShaderString = nullptr;
    char* fragmentShaderString = nullptr;

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, vertexShader);

    cout << "Creating shader..." << endl;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragmentShader == 0) {
        cerr << "Error occurred when creating vertex shader." << endl;
        exit(1);
    }

    cout << "Reading..." << endl;
    vertexShaderString = textFileRead(vertexShaderFile);
    fragmentShaderString = textFileRead(fragmentShaderFile);

    const char *vv = vertexShaderString;
    const char *ff = fragmentShaderString;

    cout << "Sourcing..." << endl;
    glShaderSource(vertexShader, 1, &vv, nullptr);
    glShaderSource(fragmentShader, 1, &ff, nullptr);

    free(vertexShaderString);
    free(fragmentShaderString);

    cout << "Compiling..." << endl;
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    GLint vertexShaderCompileResult;
    GLint fragmentShaderCompileResult;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileResult);
    if (vertexShaderCompileResult == GL_FALSE) {
        cerr << "Fail to compile shader." << endl;
        logShader(vertexShader);
        exit(1);
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileResult);
    if (fragmentShaderCompileResult == GL_FALSE) {
        cerr << "Fail to compile shader." << endl;
        logShader(fragmentShader);
        exit(1);
    }

    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);

    glLinkProgram(shaderProgram);
    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        cerr << "Fail to link shader program." << endl;
        logProgram();
        exit(1);
    }
};

void Shader::enable() {

    glUseProgram(shaderProgram);
    mat4 rotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f));
    angle += 0.01f;
    GLint location = glGetUniformLocation(shaderProgram, "RotationMatrix");

    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
    }
}

void Shader::disable() {
    glUseProgram(0);
}

void Shader::logShader(GLuint shader) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        auto * logBuffer = (char*) malloc(static_cast<size_t>(logLength));
        GLsizei written;
        glGetShaderInfoLog(shader, logLength, &written, logBuffer);
        cerr << "Shader log: \n"<< logBuffer << endl;
        free(logBuffer);
    }
};

void Shader::logProgram() {
    GLint logLength;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        auto * logBuffer = (char*) malloc(static_cast<size_t>(logLength));
        GLsizei written;
        glGetProgramInfoLog(shaderProgram, logLength, &written, logBuffer);
        cerr << "Program log: \n"<< logBuffer << endl;
        free(logBuffer);
    }
}

GLuint Shader::getShaderProgram() {
    return shaderProgram;
}