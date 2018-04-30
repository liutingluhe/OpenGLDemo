//
//  main.cpp
//  03-Primitives
//
//  Created by luhe liu on 2018/4/10.
//  Copyright © 2018年 luhe liu. All rights reserved.
//

#include <GLTools.h>
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

//各种图元的批次
GLBatch pointBatch;
GLBatch lineBatch;
GLBatch lineStripBatch;
GLBatch lineLoopBatch;
GLBatch triangleBatch;
GLBatch triangleStripBatch;
GLBatch triangleFanBatch;

//变换管线
GLGeometryTransform transformPipeline;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat vCoast[24][3] = {
    {2.80, 1.20, 0.0 }, {2.0,  1.20, 0.0 },
    {2.0,  1.08, 0.0 },  {2.0,  1.08, 0.0 },
    {0.0,  0.80, 0.0 },  {-.32, 0.40, 0.0 },
    {-.48, 0.2, 0.0 },   {-.40, 0.0, 0.0 },
    {-.60, -.40, 0.0 },  {-.80, -.80, 0.0 },
    {-.80, -1.4, 0.0 },  {-.40, -1.60, 0.0 },
    {0.0, -1.20, 0.0 },  { .2, -.80, 0.0 },
    {.48, -.40, 0.0 },   {.52, -.20, 0.0 },
    {.48,  .20, 0.0 },   {.80,  .40, 0.0 },
    {1.20, .80, 0.0 },   {1.60, .60, 0.0 },
    {2.0, .60, 0.0 },    {2.2, .80, 0.0 },
    {2.40, 1.0, 0.0 },   {2.80, 1.0, 0.0 }
};

int nStep = 0;

//初始化点批次
void SetupPointBatch() {
    pointBatch.Begin(GL_POINTS, 24);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();
}
//初始化线批次
void SetupLineBatch() {
    lineBatch.Begin(GL_LINES, 24);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();
}
//初始化线条批次
void SetupLineStripBatch() {
    lineStripBatch.Begin(GL_LINE_STRIP, 24);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();
}
//初始化闭合线条批次
void SetupLineLoopBatch() {
    lineLoopBatch.Begin(GL_LINE_LOOP, 24);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();
}
//初始化三角形批次
void SetupTriangleBatch() {
    GLfloat vPyramid[12][3] = {
        -2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f
    };
    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();
}

//初始化三角形带批次
void SetupTriangleStripBatch() {
    // 用代码生成三角形带的顶点位置
    // x,y,z 坐标的点
    GLfloat vPoints[100][3];
    int iCounter = 0;
    GLfloat radius = 3.0f;
    GLfloat height = 1.0f;
    for(GLfloat angle = 0.0f; angle <= M3D_2PI; angle += 0.3f) {
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0;
        iCounter++;
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = height;
        iCounter++;
    }
    
    //使三角形带闭合
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = 0;
    iCounter++;
    
    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = height;
    iCounter++;
    
    //三角形带批次初始化
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vPoints);
    triangleStripBatch.End();
}

//初始化三角形扇批次
void SetupTriangleFanBatch() {
    // 用代码生成三角形扇的顶点位置
    // x,y,z 坐标的点
    GLfloat vPoints[100][3];
    int nVerts = 0;
    GLfloat r = 3.0f;
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    
    for (GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        nVerts++;
        vPoints[nVerts][0] = float(cos(angle)) * r;
        vPoints[nVerts][1] = float(sin(angle)) * r;
        vPoints[nVerts][2] = -r;
    }
    
    //使三角形扇闭合
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0;
    vPoints[nVerts][2] = 0.0f;
    
    //三角形扇批次初始化
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();
}

//为程序作一次性的设置
void SetupRC() {
    //设置窗口背景颜色
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f );
    
    //初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //设置变换管线以使用两个矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    //移动摄像机的位置
    cameraFrame.MoveForward(-15.0f);
    
    //准备画图要用的批次
    SetupPointBatch();
    SetupLineBatch();
    SetupLineStripBatch();
    SetupLineLoopBatch();
    SetupTriangleBatch();
    SetupTriangleFanBatch();
    SetupTriangleStripBatch();
}

//画点
void DrawPointBatch(GLBatch* pBatch) {
    //使用着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    //设置点大小
    glPointSize(4.0f);
    //画点
    pBatch->Draw();
    
    //还原绘画环境
    glPointSize(1.0f);
}

//画线
void DrawLineBatch(GLBatch* pBatch) {
    //使用着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    //设置线粗
    glLineWidth(2.0f);
    //画线
    pBatch->Draw();
    
    //还原绘画环境
    glLineWidth(1.0f);
}

//画三角形
void DrawTriangleBatch(GLBatch* pBatch) {
    //画绿色面
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    //开启调节片段的深度值，使深度值产生偏移而不实际改变 3D 空间的物理位置
    glPolygonOffset(-1.0f, -1.0f);
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    //开启线条的抗锯齿
    glEnable(GL_LINE_SMOOTH);
    
    //开启颜色混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //多边形模式切换为前后面的线模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //画边界黑线
    glLineWidth(2.5f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();
    
    //还原绘画环境
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

//渲染画面
void RenderScene(void) {
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //保存当前的模型视图矩阵 (单位矩阵)
    modelViewMatrix.PushMatrix();
    
    //MultMatrix: 用一个矩阵乘以矩阵堆栈的顶部矩阵，相乘得到的结果随后将存储在堆栈的顶部
    //处理画笔位置，即模型相对于摄像机的位置
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);

    //处理模型自身的旋转
    M3DMatrix44f mObjectFrame;
    objectFrame.GetCameraMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);

    //画图
    switch(nStep) {
        case 0: DrawPointBatch(&pointBatch); break;
        case 1: DrawLineBatch(&lineBatch); break;
        case 2: DrawLineBatch(&lineStripBatch); break;
        case 3: DrawLineBatch(&lineLoopBatch); break;
        case 4: DrawTriangleBatch(&triangleBatch); break;
        case 5: DrawTriangleBatch(&triangleStripBatch); break;
        case 6: DrawTriangleBatch(&triangleFanBatch); break;
    }
    
    // 还原以前的模型视图矩阵 (单位矩阵)
    modelViewMatrix.PopMatrix();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
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
        case GLUT_KEY_UP: objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f); break;
        case GLUT_KEY_DOWN: objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f); break;
        case GLUT_KEY_LEFT: objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f); break;
        case GLUT_KEY_RIGHT: objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f); break;
        default:
            break;
    }
    
    //触发渲染
    glutPostRedisplay();
}

//普通按键监听
void KeyPressFunc(unsigned char key, int x, int y) {
    //空格键的 key = 32，计算空格键的按下次数，6次一循环
    if (key == 32) {
        nStep++;
        if (nStep > 6) {
            nStep = 0;
        }
    }
    
    //切换窗口标题
    switch(nStep) {
        case 0: glutSetWindowTitle("GL_POINTS"); break;
        case 1: glutSetWindowTitle("GL_LINES"); break;
        case 2: glutSetWindowTitle("GL_LINE_STRIP"); break;
        case 3: glutSetWindowTitle("GL_LINE_LOOP"); break;
        case 4: glutSetWindowTitle("GL_TRIANGLES"); break;
        case 5: glutSetWindowTitle("GL_TRIANGLE_STRIP"); break;
        case 6: glutSetWindowTitle("GL_TRIANGLE_FAN"); break;
    }
    
    //触发渲染
    glutPostRedisplay();
}

//窗口大小改变时接受新的宽度和高度
void ChangeSize(int width, int height) {
    
    // 防止下面除法的除数为0导致的闪退
    if(height == 0) height = 1;
    
    //设置视图窗口位置
    glViewport(0, 0, width, height);
    
    // 创建投影矩阵，并将它载入到投影矩阵堆栈中
    viewFrustum.SetPerspective(35.0f, float(width) / float(height), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 在模型视图矩阵顶部载入单位矩阵
    modelViewMatrix.LoadIdentity();
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
    glutCreateWindow("GL_POINTS");
    
    //注册回调函数
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(KeyPressFunc);
    
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
