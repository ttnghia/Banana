//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "TestGLWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TestGLWidget::TestGLWidget(QWidget* parent) : OpenGLWidget(parent),
    m_bUseCachedLocation(true),
    m_TestCase(1),
    m_Shader4Uniform(nullptr),
    m_Shader8Uniform(nullptr),
    m_Shader16Uniform(nullptr),
    m_Shader32Uniform(nullptr)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TestGLWidget::~TestGLWidget()
{
    delete m_Shader4Uniform;
    delete m_Shader8Uniform;
    delete m_Shader16Uniform;
    delete m_Shader32Uniform;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::initializeGL()
{
    OpenGLWidget::initializeGL();

    ////////////////////////////////////////////////////////////////////////////////
    const char* fragmentShader4Uniform =
        "#version 410 core\n"
        "\n"
        "uniform float u_Uniform1;\n"
        "uniform float u_Uniform2;\n"
        "uniform float u_Uniform3;\n"
        "uniform float u_Uniform4;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(u_Uniform1 + u_Uniform2 + u_Uniform3 + u_Uniform4, 0, 0, 1.0f); \n"
        "}\n";

    const char* fragmentShader8Uniform = "#version 410 core\n"
                                         "\n"
                                         "uniform float u_Uniform1;\n"
                                         "uniform float u_Uniform2;\n"
                                         "uniform float u_Uniform3;\n"
                                         "uniform float u_Uniform4;\n"
                                         "uniform float u_Uniform5;\n"
                                         "uniform float u_Uniform6;\n"
                                         "uniform float u_Uniform7;\n"
                                         "uniform float u_Uniform8;\n"
                                         "out vec4 outColor;\n"
                                         "\n"
                                         "void main()\n"
                                         "{\n"
                                         "    vec2 color1 = vec2(u_Uniform1 + u_Uniform2 + u_Uniform3 + u_Uniform4);\n"
                                         "    vec2 color2 = vec2(u_Uniform5 + u_Uniform6 + u_Uniform7 + u_Uniform8);\n"
                                         "    outColor = vec4(0.5*(color1 + color2), 0, 1.0f); \n"
                                         "}\n";

    const char* fragmentShader16Uniform = "#version 410 core\n"
                                          "\n"
                                          "uniform float u_Uniform1;\n"
                                          "uniform float u_Uniform2;\n"
                                          "uniform float u_Uniform3;\n"
                                          "uniform float u_Uniform4;\n"
                                          "uniform float u_Uniform5;\n"
                                          "uniform float u_Uniform6;\n"
                                          "uniform float u_Uniform7;\n"
                                          "uniform float u_Uniform8;\n"
                                          "uniform float u_Uniform9;\n"
                                          "uniform float u_Uniform10;\n"
                                          "uniform float u_Uniform11;\n"
                                          "uniform float u_Uniform12;\n"
                                          "uniform float u_Uniform13;\n"
                                          "uniform float u_Uniform14;\n"
                                          "uniform float u_Uniform15;\n"
                                          "uniform float u_Uniform16;\n"
                                          "out vec4 outColor;\n"
                                          "\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec3 color1 = vec3(u_Uniform1 + u_Uniform2 + u_Uniform3 + u_Uniform4);\n"
                                          "    vec3 color2 = vec3(u_Uniform5 + u_Uniform6 + u_Uniform7 + u_Uniform8);\n"
                                          "    vec3 color3 = vec3(u_Uniform9 + u_Uniform10 + u_Uniform11 + u_Uniform12);\n"
                                          "    vec3 color4 = vec3(u_Uniform13 + u_Uniform14 + u_Uniform15 + u_Uniform16);\n"
                                          "    outColor = vec4(0.25*(color1 + color2 + color3 + color4), 1.0f); \n"
                                          "}\n";

    const char* fragmentShader32Uniform = "#version 410 core\n"
                                          "\n"
                                          "uniform float u_Uniform1;\n"
                                          "uniform float u_Uniform2;\n"
                                          "uniform float u_Uniform3;\n"
                                          "uniform float u_Uniform4;\n"
                                          "uniform float u_Uniform5;\n"
                                          "uniform float u_Uniform6;\n"
                                          "uniform float u_Uniform7;\n"
                                          "uniform float u_Uniform8;\n"
                                          "uniform float u_Uniform9;\n"
                                          "uniform float u_Uniform10;\n"
                                          "uniform float u_Uniform11;\n"
                                          "uniform float u_Uniform12;\n"
                                          "uniform float u_Uniform13;\n"
                                          "uniform float u_Uniform14;\n"
                                          "uniform float u_Uniform15;\n"
                                          "uniform float u_Uniform16;\n"
                                          "uniform float u_Uniform17;\n"
                                          "uniform float u_Uniform18;\n"
                                          "uniform float u_Uniform19;\n"
                                          "uniform float u_Uniform20;\n"
                                          "uniform float u_Uniform21;\n"
                                          "uniform float u_Uniform22;\n"
                                          "uniform float u_Uniform23;\n"
                                          "uniform float u_Uniform24;\n"
                                          "uniform float u_Uniform25;\n"
                                          "uniform float u_Uniform26;\n"
                                          "uniform float u_Uniform27;\n"
                                          "uniform float u_Uniform28;\n"
                                          "uniform float u_Uniform29;\n"
                                          "uniform float u_Uniform30;\n"
                                          "uniform float u_Uniform31;\n"
                                          "uniform float u_Uniform32;\n"
                                          "out vec4 outColor;\n"
                                          "\n"
                                          "void main()\n"
                                          "{\n"
                                          "    float color1 = (u_Uniform1 + u_Uniform2 + u_Uniform3 + u_Uniform4);\n"
                                          "    float color2 = (u_Uniform5 + u_Uniform6 + u_Uniform7 + u_Uniform8);\n"
                                          "    float color3 = (u_Uniform9 + u_Uniform10 + u_Uniform11 + u_Uniform12);\n"
                                          "    float color4 = (u_Uniform13 + u_Uniform14 + u_Uniform15 + u_Uniform16);\n"
                                          "    float color5 = (u_Uniform17 + u_Uniform18 + u_Uniform19 + u_Uniform20);\n"
                                          "    float color6 = (u_Uniform21 + u_Uniform22 + u_Uniform23 + u_Uniform24);\n"
                                          "    float color7 = (u_Uniform25 + u_Uniform26 + u_Uniform27 + u_Uniform28);\n"
                                          "    float color8 = (u_Uniform29 + u_Uniform30 + u_Uniform31 + u_Uniform32);\n"
                                          "    outColor = vec4(0, 0, 0.125*(color1 + color2 + color3 + color4 + color5 + color6 + color7 + color8), 1.0f); \n"
                                          "}\n";

    m_Shader4Uniform = ShaderProgram::getScreenQuadShader(fragmentShader4Uniform, "Shader4Uniform");
    m_Shader8Uniform = ShaderProgram::getScreenQuadShader(fragmentShader8Uniform, "Shader8Uniform");
    m_Shader16Uniform = ShaderProgram::getScreenQuadShader(fragmentShader16Uniform, "Shader16Uniform");
    m_Shader32Uniform = ShaderProgram::getScreenQuadShader(fragmentShader32Uniform, "Shader32Uniform");

    glCall(glGenVertexArrays(1, &m_RData4U.VAO));
    glCall(glGenVertexArrays(1, &m_RData8U.VAO));
    glCall(glGenVertexArrays(1, &m_RData16U.VAO));
    glCall(glGenVertexArrays(1, &m_RData32U.VAO));

    ////////////////////////////////////////////////////////////////////////////////
    // 4 uniform
    m_RData4U.u_Uniform1 = m_Shader4Uniform->getUniformLocation("u_Uniform1");
    m_RData4U.u_Uniform2 = m_Shader4Uniform->getUniformLocation("u_Uniform2");
    m_RData4U.u_Uniform3 = m_Shader4Uniform->getUniformLocation("u_Uniform3");
    m_RData4U.u_Uniform4 = m_Shader4Uniform->getUniformLocation("u_Uniform4");

    ////////////////////////////////////////////////////////////////////////////////
    // 8 uniform
    m_RData8U.u_Uniform1 = m_Shader8Uniform->getUniformLocation("u_Uniform1");
    m_RData8U.u_Uniform2 = m_Shader8Uniform->getUniformLocation("u_Uniform2");
    m_RData8U.u_Uniform3 = m_Shader8Uniform->getUniformLocation("u_Uniform3");
    m_RData8U.u_Uniform4 = m_Shader8Uniform->getUniformLocation("u_Uniform4");
    m_RData8U.u_Uniform5 = m_Shader8Uniform->getUniformLocation("u_Uniform5");
    m_RData8U.u_Uniform6 = m_Shader8Uniform->getUniformLocation("u_Uniform6");
    m_RData8U.u_Uniform7 = m_Shader8Uniform->getUniformLocation("u_Uniform7");
    m_RData8U.u_Uniform8 = m_Shader8Uniform->getUniformLocation("u_Uniform8");

    ////////////////////////////////////////////////////////////////////////////////
    // 16 uniform
    m_RData16U.u_Uniform1 = m_Shader16Uniform->getUniformLocation("u_Uniform1");
    m_RData16U.u_Uniform2 = m_Shader16Uniform->getUniformLocation("u_Uniform2");
    m_RData16U.u_Uniform3 = m_Shader16Uniform->getUniformLocation("u_Uniform3");
    m_RData16U.u_Uniform4 = m_Shader16Uniform->getUniformLocation("u_Uniform4");
    m_RData16U.u_Uniform5 = m_Shader16Uniform->getUniformLocation("u_Uniform5");
    m_RData16U.u_Uniform6 = m_Shader16Uniform->getUniformLocation("u_Uniform6");
    m_RData16U.u_Uniform7 = m_Shader16Uniform->getUniformLocation("u_Uniform7");
    m_RData16U.u_Uniform8 = m_Shader16Uniform->getUniformLocation("u_Uniform8");
    m_RData16U.u_Uniform9 = m_Shader16Uniform->getUniformLocation("u_Uniform9");
    m_RData16U.u_Uniform10 = m_Shader16Uniform->getUniformLocation("u_Uniform10");
    m_RData16U.u_Uniform11 = m_Shader16Uniform->getUniformLocation("u_Uniform11");
    m_RData16U.u_Uniform12 = m_Shader16Uniform->getUniformLocation("u_Uniform12");
    m_RData16U.u_Uniform13 = m_Shader16Uniform->getUniformLocation("u_Uniform13");
    m_RData16U.u_Uniform14 = m_Shader16Uniform->getUniformLocation("u_Uniform14");
    m_RData16U.u_Uniform15 = m_Shader16Uniform->getUniformLocation("u_Uniform15");
    m_RData16U.u_Uniform16 = m_Shader16Uniform->getUniformLocation("u_Uniform16");

    ////////////////////////////////////////////////////////////////////////////////
    // 32 uniform
    m_RData32U.u_Uniform1 = m_Shader32Uniform->getUniformLocation("u_Uniform1");
    m_RData32U.u_Uniform2 = m_Shader32Uniform->getUniformLocation("u_Uniform2");
    m_RData32U.u_Uniform3 = m_Shader32Uniform->getUniformLocation("u_Uniform3");
    m_RData32U.u_Uniform4 = m_Shader32Uniform->getUniformLocation("u_Uniform4");
    m_RData32U.u_Uniform5 = m_Shader32Uniform->getUniformLocation("u_Uniform5");
    m_RData32U.u_Uniform6 = m_Shader32Uniform->getUniformLocation("u_Uniform6");
    m_RData32U.u_Uniform7 = m_Shader32Uniform->getUniformLocation("u_Uniform7");
    m_RData32U.u_Uniform8 = m_Shader32Uniform->getUniformLocation("u_Uniform8");
    m_RData32U.u_Uniform9 = m_Shader32Uniform->getUniformLocation("u_Uniform9");
    m_RData32U.u_Uniform10 = m_Shader32Uniform->getUniformLocation("u_Uniform10");
    m_RData32U.u_Uniform11 = m_Shader32Uniform->getUniformLocation("u_Uniform11");
    m_RData32U.u_Uniform12 = m_Shader32Uniform->getUniformLocation("u_Uniform12");
    m_RData32U.u_Uniform13 = m_Shader32Uniform->getUniformLocation("u_Uniform13");
    m_RData32U.u_Uniform14 = m_Shader32Uniform->getUniformLocation("u_Uniform14");
    m_RData32U.u_Uniform15 = m_Shader32Uniform->getUniformLocation("u_Uniform15");
    m_RData32U.u_Uniform16 = m_Shader32Uniform->getUniformLocation("u_Uniform16");
    m_RData32U.u_Uniform17 = m_Shader32Uniform->getUniformLocation("u_Uniform17");
    m_RData32U.u_Uniform18 = m_Shader32Uniform->getUniformLocation("u_Uniform18");
    m_RData32U.u_Uniform19 = m_Shader32Uniform->getUniformLocation("u_Uniform19");
    m_RData32U.u_Uniform20 = m_Shader32Uniform->getUniformLocation("u_Uniform20");
    m_RData32U.u_Uniform21 = m_Shader32Uniform->getUniformLocation("u_Uniform21");
    m_RData32U.u_Uniform22 = m_Shader32Uniform->getUniformLocation("u_Uniform22");
    m_RData32U.u_Uniform23 = m_Shader32Uniform->getUniformLocation("u_Uniform23");
    m_RData32U.u_Uniform24 = m_Shader32Uniform->getUniformLocation("u_Uniform24");
    m_RData32U.u_Uniform25 = m_Shader32Uniform->getUniformLocation("u_Uniform25");
    m_RData32U.u_Uniform26 = m_Shader32Uniform->getUniformLocation("u_Uniform26");
    m_RData32U.u_Uniform27 = m_Shader32Uniform->getUniformLocation("u_Uniform27");
    m_RData32U.u_Uniform28 = m_Shader32Uniform->getUniformLocation("u_Uniform28");
    m_RData32U.u_Uniform29 = m_Shader32Uniform->getUniformLocation("u_Uniform29");
    m_RData32U.u_Uniform30 = m_Shader32Uniform->getUniformLocation("u_Uniform30");
    m_RData32U.u_Uniform31 = m_Shader32Uniform->getUniformLocation("u_Uniform31");
    m_RData32U.u_Uniform32 = m_Shader32Uniform->getUniformLocation("u_Uniform32");


}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::paintGL()
{
    startFrameTimer();
    OpenGLWidget::paintGL();

    ////////////////////////////////////////////////////////////////////////////////
    switch(m_TestCase)
    {
    case 1:
        render4Location();
        break;

    case 2:
        render8Location();
        break;

    case 3:
        render16Location();
        break;

    case 4:
        render32Location();
        break;

    default:
        __BNN_CalledToWrongPlace
    }

    ////////////////////////////////////////////////////////////////////////////////

    endFrameTimer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::render4Location()
{
    if(m_bUseCachedLocation)
    {
        m_Shader4Uniform->bind();
        m_Shader4Uniform->setUniformValue(m_RData4U.u_Uniform1, 0.2f);
        m_Shader4Uniform->setUniformValue(m_RData4U.u_Uniform2, 0.2f);
        m_Shader4Uniform->setUniformValue(m_RData4U.u_Uniform3, 0.2f);
        m_Shader4Uniform->setUniformValue(m_RData4U.u_Uniform4, 0.2f);
        glCall(glBindVertexArray(m_RData4U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader4Uniform->release();
    }
    else
    {
        m_Shader4Uniform->bind();
        m_Shader4Uniform->setUniformValue("u_Uniform1", 0.1f);
        m_Shader4Uniform->setUniformValue("u_Uniform2", 0.1f);
        m_Shader4Uniform->setUniformValue("u_Uniform3", 0.1f);
        m_Shader4Uniform->setUniformValue("u_Uniform4", 0.1f);
        glCall(glBindVertexArray(m_RData4U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader4Uniform->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::render8Location()
{
    if(m_bUseCachedLocation)
    {
        m_Shader8Uniform->bind();
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform1, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform2, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform3, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform4, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform5, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform6, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform7, 0.2f);
        m_Shader8Uniform->setUniformValue(m_RData8U.u_Uniform8, 0.2f);
        glCall(glBindVertexArray(m_RData8U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader8Uniform->release();
    }
    else
    {
        m_Shader8Uniform->bind();
        m_Shader8Uniform->setUniformValue("u_Uniform1", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform2", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform3", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform4", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform5", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform6", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform7", 0.1f);
        m_Shader8Uniform->setUniformValue("u_Uniform8", 0.1f);
        glCall(glBindVertexArray(m_RData8U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader8Uniform->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::render16Location()
{
    if(m_bUseCachedLocation)
    {
        m_Shader16Uniform->bind();
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform1, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform2, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform3, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform4, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform5, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform6, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform7, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform8, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform9, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform10, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform11, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform12, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform13, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform14, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform15, 0.2f);
        m_Shader16Uniform->setUniformValue(m_RData16U.u_Uniform16, 0.2f);
        glCall(glBindVertexArray(m_RData16U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader16Uniform->release();
    }
    else
    {
        m_Shader16Uniform->bind();
        m_Shader16Uniform->setUniformValue("u_Uniform1", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform2", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform3", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform4", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform5", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform6", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform7", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform8", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform9", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform10", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform11", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform12", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform13", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform14", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform15", 0.1f);
        m_Shader16Uniform->setUniformValue("u_Uniform16", 0.1f);
        glCall(glBindVertexArray(m_RData16U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader16Uniform->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::render32Location()
{
    if(m_bUseCachedLocation)
    {
        m_Shader32Uniform->bind();
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform1, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform2, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform3, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform4, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform5, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform6, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform7, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform8, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform9, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform10, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform11, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform12, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform13, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform14, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform15, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform16, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform17, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform18, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform19, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform20, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform21, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform22, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform23, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform24, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform25, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform26, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform27, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform28, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform29, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform30, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform31, 0.2f);
        m_Shader32Uniform->setUniformValue(m_RData32U.u_Uniform32, 0.2f);
        glCall(glBindVertexArray(m_RData32U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader32Uniform->release();
    }
    else
    {
        m_Shader32Uniform->bind();
        m_Shader32Uniform->setUniformValue("u_Uniform1", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform2", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform3", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform4", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform5", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform6", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform7", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform8", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform9", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform10", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform11", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform12", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform13", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform14", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform15", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform16", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform17", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform18", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform19", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform20", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform21", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform22", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform23", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform24", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform25", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform26", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform27", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform28", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform29", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform30", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform31", 0.1f);
        m_Shader32Uniform->setUniformValue("u_Uniform32", 0.1f);
        glCall(glBindVertexArray(m_RData32U.VAO));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
        glCall(glBindVertexArray(0));
        m_Shader32Uniform->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::keyPressEvent(QKeyEvent* ev)
{
    switch(ev->key())
    {
    case Qt::Key_0:
        m_bUseCachedLocation = !m_bUseCachedLocation;
        break;

    case Qt::Key_1:
        m_TestCase = 1;
        break;

    case Qt::Key_2:
        m_TestCase = 2;
        break;

    case Qt::Key_3:
        m_TestCase = 3;
        break;

    case Qt::Key_4:
        m_TestCase = 4;
        break;

    default:
        ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidget::updateAvgRenderTime(double avgRenderTime)
{
    static float testResults[20][8];

    makeCurrent();
    static int count = 0;
    ++count;

    if(count < 4)
    {
        return;
    }
    else if(count == 4)
    {
        printf("Render testcase: %d, use cached location = %d\n", m_TestCase, m_bUseCachedLocation ? 1 : 0);
        fflush(stdout);
    }
    else if(count == 25)
    {
        count = 0;

        if(m_bUseCachedLocation)
        {
            m_bUseCachedLocation = false;
        }
        else
        {
            m_bUseCachedLocation = true;
            m_TestCase += 1;

            if(m_TestCase > 4)
            {

                for(int i = 1; i <= 4; ++i)
                {
                    QDebug dbg = QDebug(QtDebugMsg).nospace();
                    dbg << "Test result for test case " << i << ", use cached location: \n";

                    for(int j = 0; j < 20; ++j)
                    {
                        dbg << testResults[j][(i - 1) * 2] << ", ";
                    }

                    dbg << "\n\n";
                    dbg << "Test result for test case " << i << ", use direct queried location: \n";

                    for(int j = 0; j < 20; ++j)
                    {
                        dbg << testResults[j][(i - 1) * 2 + 1] << ", ";
                    }

                    dbg << "\n\n";
                    dbg << "Difference in render time: \n";

                    for(int j = 0; j < 20; ++j)
                    {
                        dbg << (testResults[j][(i - 1) * 2 + 1] - testResults[j][(i - 1) * 2]) << ", ";
                    }

                    dbg << "\n\n";
                }

                ////////////////////////////////////////////////////////////////////////////////
                qDebug() << "Finished!";
                exit(0);
            }
        }
    }
    else
    {
        testResults[count - 5][(m_TestCase - 1) * 2 + (m_bUseCachedLocation ? 0 : 1)] = avgRenderTime;
    }


    doneCurrent();
}