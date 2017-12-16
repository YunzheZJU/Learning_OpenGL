//
// Created by Yunzhe on 2017/12/5.
//

#include "Shader.h"

void Shader::init() {
    cout << "Creating program..." << endl;
    shaderProgram = glCreateProgram();
    if (shaderProgram == 0) {
        cerr << "Error occurred when creating shader program." << endl;
        exit(1);
    } else {
        cout << "Creating program...OK" << endl;
    }
}

void Shader::set(const char *vertexShaderFile, const char *fragmentShaderFile) {
    char* vertexShaderString = nullptr;
    char* fragmentShaderString = nullptr;

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, vertexShader);

    cout << "Creating shaders..." << endl;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragmentShader == 0) {
        cerr << "Error occurred when creating shaders." << endl;
        exit(1);
    } else {
        cout << "Creating shaders...OK" << endl;
    }

    cout << "Reading shader source files..." << endl;
    vertexShaderString = textFileRead(vertexShaderFile);
    fragmentShaderString = textFileRead(fragmentShaderFile);
    if (vertexShaderString == nullptr || fragmentShaderString == nullptr) {
        cerr << "Error occurred when reading shader source files." << endl;
        exit(1);
    } else {
        cout << "Reading shader source files...OK" << endl;
    }

    const char *vv = vertexShaderString;
    const char *ff = fragmentShaderString;

    cout << "Sourcing..." << endl;
    glShaderSource(vertexShader, 1, &vv, nullptr);
    glShaderSource(fragmentShader, 1, &ff, nullptr);
    cout << "Sourcing...OK" << endl;

    free(vertexShaderString);
    free(fragmentShaderString);

    cout << "Compiling shaders..." << endl;
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    GLint vertexShaderCompileResult;
    GLint fragmentShaderCompileResult;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileResult);
    if (vertexShaderCompileResult == GL_FALSE) {
        cerr << "Fail to compile vertex shader." << endl;
        logShader(vertexShader);
        exit(1);
    } else {
        cout << "Compiling vertex shader...OK" << endl;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileResult);
    if (fragmentShaderCompileResult == GL_FALSE) {
        cerr << "Fail to compile fragment shader." << endl;
        logShader(fragmentShader);
        exit(1);
    } else {
        cout << "Compiling fragment shader...OK" << endl;
    }

    cout << "Attaching..." << endl;
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    cout << "Attaching...OK" << endl;

    cout << "Linking..." << endl;
    glLinkProgram(shaderProgram);
    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        cerr << "Fail to link shader program." << endl;
        logProgram();
        exit(1);
    } else {
        cout << "Linking...OK" << endl;
    }
};

void Shader::enable() {
    glUseProgram(shaderProgram);
    doAfterLinking();
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

void Shader::doAfterLinking() {
    // 获得着色程序内的uniform block位置
    GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "BlobSettings");
    // 按照uniform block的尺寸分配物理内存空间
    GLint blockSize;
    glGetActiveUniformBlockiv(shaderProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    GLubyte *blockBuffer = (GLubyte *) malloc(static_cast<size_t>(blockSize));
    // 得到uniform block内变量相对于起点的偏移量
    const GLchar *names[] = {"InnerColor", "OuterColor", "RadiusInner", "RadiusOuter"};
    GLuint indices[4];
    glGetUniformIndices(shaderProgram, 4, names, indices);
    GLint offset[4];
    glGetActiveUniformsiv(shaderProgram, 4, indices, GL_UNIFORM_OFFSET, offset);
    // 把数据按合适的偏移塞到物理内存里
    GLfloat outerColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    GLfloat innerColor[] = {1.0f, 1.0f, 0.75f, 1.0f};
    GLfloat innerRadius = 0.25f;
    GLfloat outerRadius = 0.45f;
    memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &innerRadius, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[3], &outerRadius, 4 * sizeof(GLfloat));
    // 在显存里创建缓存对象并把物理内存上的数据塞进去
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);   // GL_DYNAMIC_DRAW指定优化方式
    // 把着色程序内的uniform block和显存上的缓存对象绑定
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);
}