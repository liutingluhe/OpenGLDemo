//
//  main.cpp
//  04-GeoTest
//
//  Created by luhe liu on 2018/4/12.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <GLTools.h>    // OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <math.h>
#include <glut/glut.h>

GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//是否开启正反面剔除
int iCull = 0;
//是否开始深度测试
int iDepth = 0;

//点击菜单选项
void ProcessMenu(int value) {
    switch(value) {
        case 1:
            //开关深度测试
            iDepth = !iDepth;
            break;
        case 2:
            //开关正反面剔除
            iCull = !iCull;
            break;
        case 3:
            //开关多边形面模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            //开关多边形线模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 5:
            //开关多边形点模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }
    
    //触发渲染
    glutPostRedisplay();
}

//渲染画面
void RenderScene(void) {
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //判断是否开启正反面剔除
    if(iCull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    //判断是否开启深度测试
    if(iDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    //保存当前的模型视图矩阵
    modelViewMatrix.PushMatrix(viewFrame);
    
    //这里使用默认光源进行着色，可以看到阴影
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
    
    //画花托
    torusBatch.Draw();
    
    // 还原以前的模型视图矩阵
    modelViewMatrix.PopMatrix();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
}

//为程序作一次性的设置
void SetupRC() {
    //设置窗口背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );
    
    //初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //设置变换管线以使用两个矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    //移动物体的位置，值越大物体越远，值越小物体越近
    viewFrame.MoveForward(6.0f);
    
    //创建一个花托批次，参数依次是：批次，外半径，内半径，片段数，堆叠数（在计算机中圆是正多边形，顶点数越多越像真正的圆）
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    //设置默认点大小
    glPointSize(4.0f);
}

//特殊按键（功能键或者方向键）监听
void SpecialKeys(int key, int x, int y) {
    //上、下、左、右按键，3D 旋转
    /*
     * RotateWorld(float fAngle, float x, float y, float z)
     * fAngle: 旋转弧度, x/y/z：以哪个坐标轴旋转
     * m3dDegToRad：角度 -> 弧度
     */
    switch (key) {
        case GLUT_KEY_UP:
            viewFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_DOWN:
            viewFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
            break;
        case GLUT_KEY_LEFT:
            viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
            break;
        default:
            break;
    }
    
    //触发渲染
    glutPostRedisplay();
}

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
void ChangeSize(int width, int height) {
    // 防止下面除法的除数为0导致的闪退
    if(height == 0) height = 1;
    
    //设置视图窗口位置
    glViewport(0, 0, width, height);
    
    // 创建投影矩阵，并将它载入到投影矩阵堆栈中
    viewFrustum.SetPerspective(35.0f, float(width) / float(height), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
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
    glutCreateWindow("Geometry Test Program");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    
    //创建菜单，绑定一个响应方法
    glutCreateMenu(ProcessMenu);
    
    //为菜单添加选项
    glutAddMenuEntry("Toggle depth test",1);
    glutAddMenuEntry("Toggle cull backface",2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    
    //右键弹出菜单
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
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
