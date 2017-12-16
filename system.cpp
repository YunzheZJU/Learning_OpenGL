//
// System.cpp
// Processing system display and control
// Created by Yunzhe on 2017/12/4.
//

#include "system.h"

Shader shader = Shader();
GLfloat camera[3] = {0, 0, 10};                    // Position of camera
GLfloat target[3] = {0, 0, 0};                    // Position of target of camera
GLfloat camera_polar[3] = {10, -1.57f, 0};            // Polar coordinates of camera
GLboolean bMsaa = GL_FALSE;                            // Switch of Multisampling anti-alias
GLboolean bShader = GL_TRUE;                       // Switch of shader
GLfloat camera_locator[3] = {320, 0, 0};            // Position of shadow of camera
GLboolean bcamera = GL_TRUE;                        // Switch of camera/target control
GLboolean bfocus = GL_TRUE;                            // Status of window focus
GLboolean bmouse = GL_FALSE;                        // Whether mouse postion should be moved
GLboolean bmsaa = GL_FALSE;                            // Switch of Multisampling anti-alias
int fpsmode = 2;                                    // 0:off, 1:on, 2:waiting
int window[2] = {1280, 720};                        // Window size
int windowcenter[2];                                // Center of this window, to be updated
char message[70] = "Welcome!";                        // Message string to be shown
//int focus = NONE;									// Focus object by clicking RMB

void Idle() {
    glutPostRedisplay();
}

void Reshape(int width, int height) {
    if (height == 0) {                        // Prevent A Divide By Zero By
        height = 1;                            // Making Height Equal One
    }
    glViewport(width / 2.0 - 640, height / 2.0 - 360, 1280, 720);
    window[W] = width;
    window[H] = height;
    updateWindowcenter(window, windowcenter);

    glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
    glLoadIdentity();                        // Reset The Projection Matrix
    gluPerspective(45.0f, 1.7778f, 0.1f, 30000.0f);    // 1.7778 = 1280 / 720
    glMatrixMode(GL_MODELVIEW);                // Select The Modelview Matrix
}

void Redraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();                        // Reset The Current Modelview Matrix

    gluLookAt(camera[X], camera[Y], camera[Z],
              target[X], target[Y], target[Z],
              0, 1, 0);                            // Define the view model

    if (bMsaa) {
        glEnable(GL_MULTISAMPLE_ARB);
    } else {
        glDisable(GL_MULTISAMPLE_ARB);
    }

    // Set up the lights and enable them
    SetUpLights();

    if (bShader) {
        shader.enable();
    }
    // Draw something here
    glDisable(GL_LIGHTING);
    DrawScene();
    shader.disable();
    glEnable(GL_LIGHTING);

    // Draw crosshair and locator in fps mode, or target when not in fps mode(fpsmode == 0).
    if (fpsmode == 0) {
        glDisable(GL_DEPTH_TEST);
        drawLocator(target, 2);
        glEnable(GL_DEPTH_TEST);
    } else {
        drawCrosshair();
        camera_locator[X] = camera[X];
        camera_locator[Z] = camera[Z];
        glDisable(GL_DEPTH_TEST);
//        drawLocator(camera_locator, 2);
        glEnable(GL_DEPTH_TEST);
    }

    // Draw lights
    drawLocator(lightPosition0, 1);

    // Show fps, message and other information
    PrintStatus();

    glutSwapBuffers();
}

void ProcessMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        bMsaa = !bMsaa;
        cout << "LMB pressed. Switch on/off multisampling anti-alias.\n" << endl;
        strcpy(message, "LMB pressed. Switch on/off multisampling anti-alias.");
        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && fpsmode) {
//        processPick(window);
        bShader = !bShader;
    }
}

void ProcessMouseMove(int x, int y) {
    cout << "Mouse moves to (" << x << ", " << y << ")" << endl;
    if (fpsmode) {
        // Track target and reverse mouse moving to center point.
        if (fpsmode == 2) {
            // 鼠标位置居中，为确保在glutPositionWindow()之后执行
            updateWindowcenter(window, windowcenter);
            SetCursorPos(windowcenter[X], windowcenter[Y]);
            glutSetCursor(GLUT_CURSOR_NONE);
            fpsmode = 1;
            return;
        }
        if (x < window[W] * 0.25) {
            x += window[W] * 0.5;
            bmouse = !bmouse;
        } else if (x > window[W] * 0.75) {
            x -= window[W] * 0.5;
            bmouse = !bmouse;
        }
        if (y < window[H] * 0.25) {
            y = window[H] * 0.25;
            bmouse = !bmouse;
        } else if (y > window[H] * 0.75) {
            y = window[H] * 0.75;
            bmouse = !bmouse;
        }
        // 将新坐标与屏幕中心的差值换算为polar的变化
        camera_polar[A] = (window[W] / 2 - x) * (180 / 180.0 * PI) / (window[W] / 4.0) *
                          PANNING_PACE;            // Delta pixels * 180 degrees / (1/4 width) * PANNING_PACE
        camera_polar[T] = (window[H] / 2 - y) * (90 / 180.0 * PI) / (window[H] / 4.0) *
                          PANNING_PACE;            // Delta pixels * 90 degrees / (1/4 height) * PANNING_PACE
        // 移动光标
        if (bmouse) {
            SetCursorPos(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y);
            bmouse = !bmouse;
        }
        // 更新摄像机目标
        updateTarget(camera, target, camera_polar);
    }
}

void ProcessFocus(int state) {
    if (state == GLUT_LEFT) {
        bfocus = GL_FALSE;
        cout << "Focus is on other window." << endl;
    } else if (state == GLUT_ENTERED) {
        bfocus = GL_TRUE;
        cout << "Focus is on this window." << endl;
    }
}

void ProcessNormalKey(unsigned char k, int x, int y) {
    switch (k) {
        // 退出程序
        case 27: {
            cout << "Bye." << endl;
            exit(0);
            break;
        }
            // 切换摄像机本体/焦点控制
        case 'Z':
        case 'z': {
            strcpy(message, "Z pressed. Switch camera control!");
            bcamera = !bcamera;
            break;
        }
            // 切换第一人称控制
        case 'C':
        case 'c': {
            strcpy(message, "C pressed. Switch fps control!");
            // 摄像机归零
            cameraMakeZero(camera, target, camera_polar);
            if (!fpsmode) {
                // 调整窗口位置
                int windowmaxx = glutGet(GLUT_WINDOW_X) + window[W];
                int windowmaxy = glutGet(GLUT_WINDOW_Y) + window[H];
                if (windowmaxx >= glutGet(GLUT_SCREEN_WIDTH) || windowmaxy >= glutGet(GLUT_SCREEN_HEIGHT)) {
                    // glutPositionWindow()并不会立即执行！
                    glutPositionWindow(glutGet(GLUT_SCREEN_WIDTH) - window[W], glutGet(GLUT_SCREEN_HEIGHT) - window[H]);
                    fpsmode = 2;
                    break;
                }
                // 鼠标位置居中
                updateWindowcenter(window, windowcenter);
                SetCursorPos(windowcenter[X] - window[W] * 0.125, windowcenter[Y]);
                glutSetCursor(GLUT_CURSOR_NONE);
                fpsmode = 1;
            } else {
                glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
                fpsmode = 0;
            }
            break;
        }
            // 第一人称移动/摄像机本体移动/焦点移动
        case 'A':
        case 'a': {
            strcpy(message, "A pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, camera_polar);
                camera[X] -= cos(camera_polar[A]) * MOVING_PACE;
                camera[Z] += sin(camera_polar[A]) * MOVING_PACE;
                target[X] -= cos(camera_polar[A]) * MOVING_PACE;
                target[Z] += sin(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera_polar[A] -= 0.1;
                    updateCamera(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "A pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] -= 10;
                    updatePolar(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "A pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'D':
        case 'd': {
            strcpy(message, "D pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, camera_polar);
                camera[X] += cos(camera_polar[A]) * MOVING_PACE;
                camera[Z] -= sin(camera_polar[A]) * MOVING_PACE;
                target[X] += cos(camera_polar[A]) * MOVING_PACE;
                target[Z] -= sin(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera_polar[A] += 0.1;
                    updateCamera(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "D pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] += 10;
                    updatePolar(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "D pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'W':
        case 'w': {
            strcpy(message, "W pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, camera_polar);
                camera[X] -= sin(camera_polar[A]) * MOVING_PACE;
                camera[Z] -= cos(camera_polar[A]) * MOVING_PACE;
                target[X] -= sin(camera_polar[A]) * MOVING_PACE;
                target[Z] -= cos(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera[Y] += 10;
                    cout << fixed << setprecision(1) << "W pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[Y] += 10;
                    updatePolar(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "W pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'S':
        case 's': {
            strcpy(message, "S pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, camera_polar);
                camera[X] += sin(camera_polar[A]) * MOVING_PACE;
                camera[Z] += cos(camera_polar[A]) * MOVING_PACE;
                target[X] += sin(camera_polar[A]) * MOVING_PACE;
                target[Z] += cos(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    if (camera[Y] >= 10) {
                        camera[Y] -= 10;
                    }
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                    strcpy(message, "S pressed. Watch carefully!");
                } else {
                    target[Y] -= 10;
                    updatePolar(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'Q':
        case 'q': {
            if (bcamera) {
                strcpy(message, "Q pressed. Camera is moved...nearer!");
                camera_polar[R] *= 0.95;
                updateCamera(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera is set to (" <<
                     camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
            } else {
                strcpy(message, "Q pressed. Camera target is moving towards +Z!");
                target[Z] += 5;
                updatePolar(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera target is set to (" <<
                     target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
            }
            break;
        }
        case 'E':
        case 'e': {
            if (bcamera) {
                strcpy(message, "E pressed. Camera is moved...farther!");
                camera_polar[R] *= 1.05;
                updateCamera(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera is set to (" <<
                     camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
            } else {
                strcpy(message, "E pressed. Camera target is moving towards -Z!");
                target[Z] -= 5;
                updatePolar(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera target is set to (" <<
                     target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
            }
            break;
        }
            // 光源颜色分量
        case 'R':
        case 'r': {
            cout << "R pressed." << endl;
            currentColor[0] -= 0.1f;
            if (currentColor[0] < 0) {
                currentColor[0] = 1.0f;
            }
            break;
        }
        case 'G':
        case 'g': {
            cout << "G pressed." << endl;
            currentColor[1] -= 0.1f;
            if (currentColor[1] < 0) {
                currentColor[1] = 1.0f;
            }
            break;
        }
        case 'B':
        case 'b': {
            cout << "B pressed." << endl;
            currentColor[2] -= 0.1f;
            if (currentColor[2] < 0) {
                currentColor[2] = 1.0f;
            }
            break;
        }
            // 光源强度（衰减）
        case '+': {
            cout << "+ pressed." << endl;
            constantattenuation -= 0.1f;
            cout << constantattenuation << endl;
            break;
        }
        case '-': {
            cout << "- pressed." << endl;
            constantattenuation += 0.1f;
            cout << constantattenuation << endl;
            break;
        }
            // 屏幕截图
        case 'X':
        case 'x': {
            cout << "X pressed." << endl;
            if (screenshot(window[W], window[H])) {
                cout << "Screenshot is saved." << endl;
                strcpy(message, "X pressed. Screenshot is Saved.");
            } else {
                cout << "Screenshot failed." << endl;
                strcpy(message, "X pressed. Screenshot failed.");
            }
            break;
        }
        default:break;
    }
}

void ProcessSpecialKey(int k, int x, int y) {
    switch (k) {
        // Up arrow
        case 101: {
            lightPosition0[Y] += 5;
            break;
        }
        // Down arrow
        case 103: {
            if (lightPosition0[Y] >= 10) {
                lightPosition0[Y] -= 5;
            }
            break;
        }
        default:
            break;
    }
}

void PrintStatus() {
    static int frame = 0;
    static int currenttime;
    static int timebase = 0;
    static char fpstext[50];
    char *c;
    char cameraPositionMessage[50];
    char targetPositionMessage[50];
    char cameraPolarPositonMessage[50];

    frame++;
    currenttime = glutGet(GLUT_ELAPSED_TIME);
    if (currenttime - timebase > 1000) {
        sprintf(fpstext, "FPS:%4.2f",
                frame * 1000.0 / (currenttime - timebase));
        timebase = currenttime;
        frame = 0;
    }

    sprintf(cameraPositionMessage, "Camera Position  %2.1f   %2.1f   %2.1f",
            camera[X], camera[Y], camera[Z]);
    sprintf(targetPositionMessage, "Target Position     %2.1f   %2.1f   %2.1f",
            target[X], target[Y], target[Z]);
    sprintf(cameraPolarPositonMessage, "Camera Polar      %2.1f   %2.3f   %2.3f",
            camera_polar[R], camera_polar[A], camera_polar[T]);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);                    // 不受灯光影响
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPushMatrix();                            // 保存原矩阵
    glLoadIdentity();                        // 装入单位矩阵
    glOrtho(-640, 640, -360, 360, -1, 1);    // 设置裁减区域
    glMatrixMode(GL_MODELVIEW);                // 选择Modelview矩阵
    glPushMatrix();                            // 保存原矩阵
    glLoadIdentity();                        // 装入单位矩阵
    glPushAttrib(GL_LIGHTING_BIT);
    glRasterPos2f(-620, 340);
    for (c = fpstext; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glRasterPos2f(400, 340);
    for (c = cameraPositionMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(400, 305);
    for (c = targetPositionMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(400, 270);
    for (c = cameraPolarPositonMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(-620, -340);
    for (c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glMatrixMode(GL_MODELVIEW);                // 选择Modelview矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void SetBufferedObjects() {
    float positionData[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };
    float texCoordData[] = {
            0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f
    };
    GLuint vboHandles[2];
    // 申请两个缓存标识符
    glGenBuffers(2, vboHandles);
    // 两个标识符的值存到变量
    GLuint positionBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];
    // 为两个缓存分别指定缓存类型（GL_ARRAY_BUFFER, GL_UNIFORM_BUFFER or GL_ELEMENT_ARRAY_BUFFER），申请空间并塞入数据
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), positionData, GL_STATIC_DRAW); // GL_STATIC_DRAW告诉OpenGL如何优化
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), texCoordData, GL_STATIC_DRAW);

    // 申请一个顶点数组标识符
    glGenVertexArrays(1, &vaoHandle);
    // 用标识符初始化顶点数组对象
    glBindVertexArray(vaoHandle);
    // 启用编号为0和1的顶点数组
    glEnableVertexAttribArray(0);   // 给vertexPosition
    glEnableVertexAttribArray(1);   // 给vertexColor
    // 将0号顶点数组与第一个缓存（类型是数组缓存）绑定
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // 将1号顶点数组与第二个缓存（类型是数组缓存）绑定
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}