//
//  main.cpp
//  02-MoveDemo
//
//  Created by luhe liu on 2018/4/9.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <iostream>
#include <GLShaderManager.h>
#include <GLTools.h>
#include <glut/glut.h>

GLBatch squareBatch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.2f;
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f, //左下角
    blockSize, -blockSize, 0.0f,  //右下角
    blockSize,  blockSize, 0.0f,  //右上角
    -blockSize,  blockSize, 0.0f  //左上角
};

int specialKey = GLUT_KEY_RIGHT;

void  BounceFunction(int key) {
    //每次按键的矩形移动距离
    GLfloat stepSize = 0.025f;
    //左下角坐标的 (x, y)
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[1];
    
    //判断键盘点击的是哪个按钮，计算下一个点的位置
    if(key == GLUT_KEY_UP) blockY += stepSize;
    if(key == GLUT_KEY_DOWN) blockY -= stepSize;
    if(key == GLUT_KEY_LEFT) blockX -= stepSize;
    if(key == GLUT_KEY_RIGHT) blockX += stepSize;
    
    //边界检测，让矩形没法超出边界
    if(blockX < -1.0f) blockX = -1.0f;
    if(blockX > (1.0f - blockSize * 2)) blockX = 1.0f - blockSize * 2;
    if(blockY < -1.0f) blockY = -1.0f;
    if(blockY > (1.0f - blockSize * 2)) blockY = 1.0f - blockSize * 2;
    
    //设置下个位置的 4 个顶点坐标
    //左下角
    vVerts[0] = blockX;
    vVerts[1] = blockY;
    //右下角
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY;
    //右上角
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY + blockSize * 2;
    //左上角
    vVerts[9] = blockX;
    vVerts[10] = blockY + blockSize * 2;
    
    //批次里面的数据更新
    squareBatch.CopyVertexData3f(vVerts);
}

// 监控特殊按键（功能键和方向键）点击
void SpecialKeys(int key, int x, int y) {
    
    specialKey = key;
    
//    BounceFunction(key);
    
    //触发重画事件 -> RenderScene
//    glutPostRedisplay();
}

//窗口渲染
void RenderScene(void) {
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //利用单位着色器设置矩形颜色为红色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    //画图
    squareBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
    //计算下一个矩形位置
    BounceFunction(specialKey);

    //触发重画事件 -> RenderScene
    glutPostRedisplay();
}

//为程序作一次性的设置
void SetupRC() {
    //设置背景颜色为蓝色
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    //着色器初始化
    shaderManager.InitializeStockShaders();
    
    //批次初始化
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
void ChangeSize(int width, int height) {
    glViewport(0, 0, width, height);
}

//程序入口
int main(int argc, char* argv[]) {
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    /*初始化渲染模式，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    //初始化窗口大小
    glutInitWindowSize(800, 600);
    //创建窗口
    glutCreateWindow("MoveDemo");
    
    //注册窗口大小变化回调函数
    glutReshapeFunc(ChangeSize);
    //注册窗口渲染回调函数
    glutDisplayFunc(RenderScene);
    //注册键盘特殊按键点击回调函数
    glutSpecialFunc(SpecialKeys);
    
    //确保驱动程序的初始化中没有出现任何问题。
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr, "glew error:%s\n", glewGetErrorString(err));
        return 1;
    }
    
    //初始化设置
    SetupRC();
    
    //进入调用循环
    glutMainLoop();
    return 0;
}
