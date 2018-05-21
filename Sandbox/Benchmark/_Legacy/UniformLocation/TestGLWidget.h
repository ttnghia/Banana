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

#pragma once

#include <OpenGLHelpers/ShaderProgram.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TestGLWidget : public OpenGLWidget
{
    Q_OBJECT

public:
    TestGLWidget(QWidget* parent = nullptr);
    ~TestGLWidget();

    virtual void keyPressEvent(QKeyEvent* ev) override;

public slots:
    void updateAvgRenderTime(double avgRenderTime);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;

    void render4Location();
    void render8Location();
    void render16Location();
    void render32Location();

    bool           m_bUseCachedLocation;
    int            m_TestCase;

    struct
    {
        GLuint u_Uniform1;
        GLuint u_Uniform2;
        GLuint u_Uniform3;
        GLuint u_Uniform4;

        GLuint VAO;
    } m_RData4U;

    struct
    {
        GLuint u_Uniform1;
        GLuint u_Uniform2;
        GLuint u_Uniform3;
        GLuint u_Uniform4;
        GLuint u_Uniform5;
        GLuint u_Uniform6;
        GLuint u_Uniform7;
        GLuint u_Uniform8;

        GLuint VAO;
    } m_RData8U;

    struct
    {
        GLuint u_Uniform1;
        GLuint u_Uniform2;
        GLuint u_Uniform3;
        GLuint u_Uniform4;
        GLuint u_Uniform5;
        GLuint u_Uniform6;
        GLuint u_Uniform7;
        GLuint u_Uniform8;
        GLuint u_Uniform9;
        GLuint u_Uniform10;
        GLuint u_Uniform11;
        GLuint u_Uniform12;
        GLuint u_Uniform13;
        GLuint u_Uniform14;
        GLuint u_Uniform15;
        GLuint u_Uniform16;

        GLuint VAO;
    } m_RData16U;

    struct
    {
        GLuint u_Uniform1;
        GLuint u_Uniform2;
        GLuint u_Uniform3;
        GLuint u_Uniform4;
        GLuint u_Uniform5;
        GLuint u_Uniform6;
        GLuint u_Uniform7;
        GLuint u_Uniform8;
        GLuint u_Uniform9;
        GLuint u_Uniform10;
        GLuint u_Uniform11;
        GLuint u_Uniform12;
        GLuint u_Uniform13;
        GLuint u_Uniform14;
        GLuint u_Uniform15;
        GLuint u_Uniform16;
        GLuint u_Uniform17;
        GLuint u_Uniform18;
        GLuint u_Uniform19;
        GLuint u_Uniform20;
        GLuint u_Uniform21;
        GLuint u_Uniform22;
        GLuint u_Uniform23;
        GLuint u_Uniform24;
        GLuint u_Uniform25;
        GLuint u_Uniform26;
        GLuint u_Uniform27;
        GLuint u_Uniform28;
        GLuint u_Uniform29;
        GLuint u_Uniform30;
        GLuint u_Uniform31;
        GLuint u_Uniform32;

        GLuint VAO;
    } m_RData32U;

    ShaderProgram* m_Shader4Uniform;
    ShaderProgram* m_Shader8Uniform;
    ShaderProgram* m_Shader16Uniform;
    ShaderProgram* m_Shader32Uniform;
};