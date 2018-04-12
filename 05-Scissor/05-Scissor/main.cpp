//
//  main.cpp
//  05-Scissor
//
//  Created by luhe liu on 2018/4/12.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <GLTools.h>        // OpenGL toolkit
#include <glut/glut.h>

//渲染画面
void RenderScene(void) {
    
    //设置背景蓝色
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    //清空颜色缓存区
    glClear(GL_COLOR_BUFFER_BIT);
    
    //设置背景色为红色
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    //裁剪出(x: 100, y: 100, w: 600, h: 400)区域
    glScissor(100, 100, 600, 400);
    //开启裁剪
    glEnable(GL_SCISSOR_TEST);
    //清空颜色缓存区
    glClear(GL_COLOR_BUFFER_BIT);
    
    //设置背景色为绿色
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    //裁剪出(x: 200, y: 200, w: 400, h: 200)区域
    glScissor(200, 200, 400, 200);
    //清空颜色缓存区
    glClear(GL_COLOR_BUFFER_BIT);
    
    //关闭裁剪
    glDisable(GL_SCISSOR_TEST);
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
}

//为程序作一次性的设置
void SetupRC() {
}

//窗口大小改变时接受新的宽度和高度
void ChangeSize(int w, int h) {
    
    //设置视图窗口位置
    glViewport(0, 0, w, h);
}

//程序入口
int main(int argc, char* argv[]) {
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    /*初始化渲染模式，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    //初始化窗口大小
    glutInitWindowSize(800, 600);
    //创建窗口
    glutCreateWindow("OpenGL Scissor");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
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
