//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include "Common.h"
#include "HairModel.h"

#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>
#include <OpenGLHelpers/Material.h>
#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/RenderObjects.h>

#include <QtAppHelpers/QtAppShaderProgram.h>
#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    RenderWidget(QWidget* parent, const SharedPtr<HairModel>& hairModel);
private:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();

    SharedPtr<HairModel> m_HairModel;
public slots:
    void updateModelDimension();
    void updateVizData();

    ////////////////////////////////////////////////////////////////////////////////
    // hair
public slots:
    void loadHairModel(const String& hairFile);
    void setColorMode(int colorMode);
    void setColorDataMin(const Vec3f& colorMin) { m_RDataHair.colorMinVal = colorMin; }
    void setColorDataMax(const Vec3f& colorMax) { m_RDataHair.colorMaxVal = colorMax; }
    void setHairMaterial(const Material::MaterialData& material);
private:
    struct RDataHair
    {
        SharedPtr<QtAppShaderProgram> shader        = nullptr;
        UniquePtr<OpenGLBuffer>       buffPosition  = nullptr;
        UniquePtr<OpenGLBuffer>       buffColorData = nullptr;
        UniquePtr<Material>           material      = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_iColor;
        GLint  v_fColor;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_nVertices;
        GLuint u_PointRadius;
        GLuint u_PointScale;
        GLuint u_Dimension;
        GLuint u_ScreenHeight;
        GLuint u_DomainHeight;
        GLuint u_ClipPlane;
        GLuint u_ColorMode;
        GLuint u_vColorMin;
        GLuint u_vColorMax;
        GLuint u_ColorMinVal;
        GLuint u_ColorMaxVal;

        GLuint  nVertices = 0;
        GLuint  nStrands  = 0;
        GLfloat pointRadius;
        GLfloat pointScale;

        GLint pColorMode  = HairColorMode::UniformMaterial;
        float vColorMin   = 0;
        float vColorMax   = 1.0f;
        Vec3f colorMinVal = DEFAULT_COLOR_DATA_MIN;
        Vec3f colorMaxVal = DEFAULT_COLOR_DATA_MAX;

        bool initialized = false;
    } m_RDataHair;

    void initRDataHair();
    void initHairVAO();
    void renderHair();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // mesh
public slots:
    void loadMesh(const QString& meshFile);
    void setMeshMaterial(const Material::MaterialData& material);
    void transformMeshWithHair();
private:
    void initRDataMesh();
    void renderMesh();

    SharedPtr<MeshObject> m_MeshObject;
    UniquePtr<MeshRender> m_MeshRender;
    ////////////////////////////////////////////////////////////////////////////////
};