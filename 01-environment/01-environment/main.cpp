//
//  main.cpp
//  01-environment
//
//  Created by luhe liu on 2018/3/24.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <iostream>
#include <GLShaderManager.h>
#include <GLTools.h>
#include <glut/glut.h>

GLBatch triangleBatch;
GLShaderManager shaderManager;

//窗口大小改变时接受新的宽度和高度
void ChangeSize(int width, int height) {
    // 设置视口，(x, y, width, height)，其中 (x, y) 代表窗口中视口的左下角坐标
    glViewport(0, 0, width, height);
}

//为程序作一次性的设置
void SetupRC() {
    //设置背影颜色
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    //构建三角形顶点数组，vVert包含3个顶点的(x, y, z)笛卡尔坐标
    GLfloat vVerts[] = {
        -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };
    //开始构建批次，GL_TRIANGLES 表示三角形，后面参数是顶点数
    triangleBatch.Begin(GL_TRIANGLES, 3);
    /*
     * 批次增加存储属性
     * CopyVertexData3f(XYZ顶点数据)
     * CopyNormalDataf(表面法线)
     * CopyColorData4f(RGBA颜色)
     * CopyTexCoordData2f(纹理坐标)
     */
    triangleBatch.CopyVertexData3f(vVerts);
    //结束批次属性设置，表明完成数据复制操作，不能再添加新属性
    triangleBatch.End();
}

//开始渲染
void RenderScene(void) {
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    //根据批次里的顶点数据利用当前设置的着色器进行画图
    triangleBatch.Draw();
    /*
     * 当 glutInitDisplayMode 有传入双缓冲模式时，
     * 将在后台缓冲区进行渲染，然后在结束时交换到前台
     * 为了防止观察者看到可能随着动画帧和动画帧之间闪烁的渲染过程
     */
    glutSwapBuffers();
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
    glutCreateWindow("Triangle");
    //注册回调函数，拦截窗口大小变化消息，ChangeSize 是自定义方法名
    glutReshapeFunc(ChangeSize);
    //注册回调函数，拦截窗口渲染消息，RenderScene 是自定义方法名
    glutDisplayFunc(RenderScene);
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
