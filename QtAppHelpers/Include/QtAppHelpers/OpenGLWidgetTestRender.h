//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/26/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include  <OpenGLHelpers/OpenGLBuffer.h>
#include  <OpenGLHelpers/OpenGLTexture.h>
#include  <OpenGLHelpers/ShaderProgram.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidgetTestRender : public OpenGLWidget
{
    Q_OBJECT

public:
    enum class RenderType
    {
        NotSetup,
        Triangle,
        Texture
    };

    OpenGLWidgetTestRender(QWidget *parent);
    ~OpenGLWidgetTestRender();

    void setRenderTriangle();
    void setRenderTexture(QString texFile);

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    void renderTriangle();
    void renderTexture();

    RenderType     m_RenderType;
    OpenGLBuffer*  m_UniformBuffer;
    OpenGLBuffer*  m_VertexBuffer;
    OpenGLBuffer*  m_IndexBuffer;
    OpenGLTexture* m_Texture;
    ShaderProgram* m_Shader;
    GLuint         m_VAO;
};
