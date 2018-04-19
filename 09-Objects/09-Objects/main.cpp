//
//  main.cpp
//  09-Objects
//
//  Created by luhe liu on 2018/4/18.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <GLTools.h>    // OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLGeometryTransform.h>
#include <math.h>
#include <glut/glut.h>

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrame cameraFrame;
GLFrame objectFrame;
GLFrustum viewFrustum;

GLTriangleBatch sphereBatch;
GLTriangleBatch torusBatch;
GLTriangleBatch cylinderBatch;
GLTriangleBatch coneBatch;
GLTriangleBatch diskBatch;

GLGeometryTransform transformPipeline;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

int nStep = 0;

// 程序初始化环境
void SetupRC() {
    // 设置背景为灰色背景
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f );
    
    // 初始化着色器
    shaderManager.InitializeStockShaders();
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 设置变换管线，管理模型视图堆栈和投影堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    // 照相机位置往后移动 15
    cameraFrame.MoveForward(-15.0f);
    
    // 初始化球体的三角形批次，后面参数依次是，球半径，片段数，堆叠数
    gltMakeSphere(sphereBatch, 3.0, 10, 20);
    
    // 初始化花托的三角形批次，后面参数依次是，外半径，内半径，片段数，堆叠数
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 150, 15);
    
    // 初始化圆柱的三角形批次，后面参数依次是，底部半径，顶部半径，高度，片段数，堆叠数
    gltMakeCylinder(cylinderBatch, 2.0f, 2.0f, 3.0f, 13, 2);
    
    // 初始化圆锥的三角形批次，后面参数依次是，底部半径，顶部半径，高度，片段数，堆叠数
    gltMakeCylinder(coneBatch, 2.0f, 0.0f, 3.0f, 13, 2);
    
    // 初始化圆盘的三角形批次，后面参数依次是，内半径，外半径，片段数，堆叠数
    gltMakeDisk(diskBatch, 1.5f, 3.0f, 13, 3);
}

// 画三角形批次共用方法
void DrawWireFramedBatch(GLTriangleBatch* pBatch) {
    
    // 画物体表面画绿色
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    // 设置多边形偏移
    glPolygonOffset(-1.0f, -1.0f);
    
    // 开启线段的抗锯齿
    glEnable(GL_LINE_SMOOTH);
    
    // 开启混合颜色模式
    glEnable(GL_BLEND);
    
    // 设置混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 设置多边型模式为线段
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 设置多边形模式为前后面的线段
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // 设置线宽 2.5
    glLineWidth(2.5f);
    
    // 画物体线条黑色
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();
    
    // 以下为还原渲染环境，养成还原环境的好习惯
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

// 窗口渲染调用
void RenderScene(void) {
    // 清除缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 模型堆栈压栈，顶部是单位矩阵
    modelViewMatrix.PushMatrix();
    
    // 获取照相机位置矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    
    // 单位矩阵 x 照相机位置矩阵 = 照相机位置矩阵
    modelViewMatrix.MultMatrix(mCamera);
    
    // 获取物体位置矩阵
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    
    // 照相机位置矩阵 x 物体位置矩阵 = 物体在照相机下的位置矩阵
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    // 设置着色器为黑色
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    // 根据不同状态，画不同的对象
    switch(nStep) {
        case 0: DrawWireFramedBatch(&sphereBatch); break;
        case 1: DrawWireFramedBatch(&torusBatch); break;
        case 2: DrawWireFramedBatch(&cylinderBatch); break;
        case 3: DrawWireFramedBatch(&coneBatch); break;
        case 4: DrawWireFramedBatch(&diskBatch); break;
    }
    
    // 出栈，还原栈顶为单位矩阵
    modelViewMatrix.PopMatrix();
    
    // 双缓冲区模式，后台缓冲区切换到前台进行显示
    glutSwapBuffers();
}


// 特殊按键点击响应
void SpecialKeys(int key, int x, int y) {
    
    // 按下上、下、左、右方向键，对物体进行旋转，m3dDegToRad = 角度 -> 弧度
    switch(key) {
        case GLUT_KEY_UP:
            // angle=-5, x=1, y=0, z=0 表示绕x轴正方向顺时针旋转(从x轴正方向看去)
            objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_DOWN:
            // angle=5, x=1, y=0, z=0 表示绕x轴正方向逆时针旋转(从x轴正方向看去)
            objectFrame.RotateWorld(m3dDegToRad(-5.0f), -1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_LEFT:
            // angle=-5, x=0, y=1, z=0 表示绕y轴正方向顺时针旋转(从y轴正方向看去)
            objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            // angle=5, x=0, y=1, z=0 表示绕y轴正方向逆时针旋转(从y轴正方向看去)
            objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
            break;
    }
    
    // 触发渲染
    glutPostRedisplay();
}

// 普通按键点击响应
void KeyPressFunc(unsigned char key, int x, int y) {
    
    // key = 32 是空格键，5次空格键一次循环
    if(key == 32) {
        nStep++;
        if(nStep > 4) nStep = 0;
    }
    
    // 切换窗口标题
    switch(nStep) {
        case 0: glutSetWindowTitle("Sphere"); break;
        case 1: glutSetWindowTitle("Torus"); break;
        case 2: glutSetWindowTitle("Cylinder"); break;
        case 3: glutSetWindowTitle("Cone"); break;
        case 4: glutSetWindowTitle("Disk"); break;
    }
    
    // 触发渲染
    glutPostRedisplay();
}

// 窗口变化回调
void ChangeSize(int width, int height) {
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 防止除数为0导致错误
    if(height <= 0) height = 1;
    
    // 设置投影矩阵，参数分别为：投影视角，宽高比，近点距离，远点距离
    viewFrustum.SetPerspective(35.0f, float(width) / float(height), 1.0f, 500.0f);
    
    // 投影矩阵堆栈顶部载入当前设置的投影矩阵
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 模型矩阵堆栈载入单位矩阵
    modelViewMatrix.LoadIdentity();
}

// 程序入口
int main(int argc, char* argv[]) {
    // 针对 Mac OS 设置当前工作目录路径
    gltSetWorkingDirectory(argv[0]);
    
    // 初始化 GLUT
    glutInit(&argc, argv);
    
    // 设置渲染模式，双缓冲、颜色、深度测试、模式
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    // 初始化窗口大小
    glutInitWindowSize(800, 600);
    
    // 创建窗口并命名
    glutCreateWindow("Sphere");
    
    // 驱动程序初始化是否成功
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    // 窗口大小变化回调函数设置
    glutReshapeFunc(ChangeSize);
    
    // 普通键盘按钮点击监听回调函数设置
    glutKeyboardFunc(KeyPressFunc);
    
    // 特殊键盘按钮点击监听回调函数设置
    glutSpecialFunc(SpecialKeys);
    
    // 窗口渲染回调函数设置
    glutDisplayFunc(RenderScene);
    
    // 程序初始化环境
    SetupRC();
    
    // 主消息循环
    glutMainLoop();
    
    return 0;
}
