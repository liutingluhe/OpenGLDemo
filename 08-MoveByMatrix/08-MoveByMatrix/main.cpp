//
//  main.cpp
//  08-MoveByMatrix
//
//  Created by luhe liu on 2018/4/18.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <GLTools.h>    // OpenGL toolkit
#include <GLShaderManager.h>
#include <math3d.h>
#include <glut/glut.h>

GLBatch squareBatch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;
GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;

// 矩形4个顶点位置坐标
GLfloat vVerts[] = {
   -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize,  blockSize, 0.0f,
   -blockSize,  blockSize, 0.0f
};


// 初始化环境
void SetupRC() {
    // 设置窗口背景为蓝色
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
    
    // 着色器初始化
    shaderManager.InitializeStockShaders();
    
    // 移动矩形批次初始化顶点数据
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

// 特殊按键点击响应回调
void SpecialKeys(int key, int x, int y) {
    // 步进值，每次按键移动的距离
    GLfloat stepSize = 0.025f;

    // 4个方向位置偏移
    if(key == GLUT_KEY_UP) yPos += stepSize;
    if(key == GLUT_KEY_DOWN) yPos -= stepSize;
    if(key == GLUT_KEY_LEFT) xPos -= stepSize;
    if(key == GLUT_KEY_RIGHT) xPos += stepSize;
    
    // 边界判断和限制
    if(xPos < (-1.0f + blockSize)) xPos = -1.0f + blockSize;
    if(xPos > (1.0f - blockSize)) xPos = 1.0f - blockSize;
    if(yPos < (-1.0f + blockSize))  yPos = -1.0f + blockSize;
    if(yPos > (1.0f - blockSize)) yPos = 1.0f - blockSize;
    
    // 触发渲染
    glutPostRedisplay();
}

// 窗口渲染回调
void RenderScene(void) {
    // 清除缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 创建3个4x4矩阵，分别是合成矩阵、平移矩阵、旋转矩阵
    M3DMatrix44f mFinalTransform, mTranslationMatrix, mRotationMatrix;
    
    // 平移(xPos, yPos, 0)的矩阵表示
    m3dTranslationMatrix44(mTranslationMatrix, xPos, yPos, 0.0f);
    
    // 绕z轴旋转的矩阵，每次旋转角度加5度，m3dDegToRad = 角度 -> 弧度
    static float zRot = 0.0f;
    zRot += 5.0f;
    m3dRotationMatrix44(mRotationMatrix, m3dDegToRad(zRot), 0.0f, 0.0f, 1.0f);
    
    // 矩阵相乘，参数顺序很重要，先平移，后旋转
    m3dMatrixMultiply44(mFinalTransform, mTranslationMatrix, mRotationMatrix);
    
    // 着色器上色，移动矩形为红色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mFinalTransform, vRed);
    squareBatch.Draw();
    
    // 因为是双缓冲区模式，后台缓冲区替换到前台缓存区进行显示
    glutSwapBuffers();
}

// 窗口变化回调
void ChangeSize(int width, int height) {
    // 设置视口
    glViewport(0, 0, width, height);
}

// 程序入口
int main(int argc, char* argv[]) {
    // 设置 Mac OS 工作目录路径
    gltSetWorkingDirectory(argv[0]);
    
    // GLUT初始化
    glutInit(&argc, argv);
    
    // 设置渲染模式
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    
    // 初始化窗口大小
    glutInitWindowSize(600, 600);
    
    // 创建窗口并命名
    glutCreateWindow("Move Block with Arrow Keys");
    
    // 判断驱动程序是否初始化完毕
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    // 窗口变化回调函数设置
    glutReshapeFunc(ChangeSize);
    
    // 窗口渲染回调函数设置
    glutDisplayFunc(RenderScene);
    
    // 特殊按钮响应回调函数设置
    glutSpecialFunc(SpecialKeys);
    
    // 初始化环境
    SetupRC();
    
    // 主消息循环
    glutMainLoop();
    
    return 0;
}
