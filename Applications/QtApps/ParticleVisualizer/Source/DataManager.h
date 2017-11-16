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
//    /                    Created: 2017 by Nghia Truong                     \
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

#include <Banana/Geometry/GeometryObjects.h>

#include <QtWidgets>
#include <QFileSystemWatcher>
#include <QList>
#include <QString>

#include <array>
#include <memory>

#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct LightData
{
    LightData(const Vec3f& position_ = DEFAULT_LIGHT_POSITION, const Vec3f& color_ = Vec3f(1.0)) : position(position_), color(color_) {}
    Vec3f position;
    Vec3f color;
};

struct MeshData
{
    Vec_Vec3f vertices;
    Vec_Vec3f normals;
};

struct VisualizationData
{
    Vec3f boxMin         = Vec3f(-1.0);
    Vec3f boxMax         = Vec3f(1.0);
    Vec3f cameraPosition = DEFAULT_CAMERA_POSITION;
    Vec3f cameraFocus    = DEFAULT_CAMERA_FOCUS;

    Vector<LightData> lights;
    Vector<MeshData>  boundaryObjs;

    ////////////////////////////////////////////////////////////////////////////////
    VisualizationData() { resetData(); }
    void resetData()
    {
        boxMin         = Vec3f(-1.0);
        boxMax         = Vec3f(1.0);
        cameraPosition = DEFAULT_CAMERA_POSITION;
        cameraFocus    = DEFAULT_CAMERA_FOCUS;

        lights.resize(0);
        lights.push_back(LightData());
        boundaryObjs.resize(0);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager() = default;
    bool                     setDataPath(const QString& dataPath);
    const VisualizationData& getVizData() const { return m_VizData; }

private slots:
    void countFrames();

signals:
    void numFramesChanged(int numFrames);

private:
    bool loadVizData(const QString& dataPath);

    int                           m_NumFrames      = 0;
    QString                       m_DataPath       = QString("");
    UniquePtr<QFileSystemWatcher> m_DataDirWatcher = nullptr;
    QStringList                   m_DataSubFolders;
    VisualizationData             m_VizData;
};