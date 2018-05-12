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

#include <Banana/Setup.h>
#include <Banana/Utils/AppConfigReader.h>
#include <QtWidgets>
#include <QString>
#include <QDir>
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::QtAppUtils
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QColor floatToQColor(float r, float g, float b) { return QColor(static_cast<int>(255 * r), static_cast<int>(255 * g), static_cast<int>(255 * b)); }
inline QColor floatToQColor(const Vec3f& color) { return floatToQColor(color.x, color.y, color.z); }
inline Vec3f  QColorToFloat(const QColor& color) { return Vec3f(color.redF(), color.greenF(), color.blueF()); }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QString getTexturePath()
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam("TexturePath")) {
        return QString::fromStdString(config.getStringValue("TexturePath"));
    } else {
        return QDir::currentPath() + "/Textures";
    }
}

inline QStringList getTextureFolders(const QString& texType)
{
    QDir dataDir(getTexturePath() + "/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    return dataDir.entryList();
}

inline QStringList getTextureFiles(const QString& texType)
{
    QDir dataDir(getTexturePath() + "/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QString getDefaultPath(const String& folderName)
{
    String          folderParams = folderName + "Path";
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam(folderParams)) {
        return QString::fromStdString(config.getStringValue(folderParams));
    } else {
        return QDir::currentPath() + "/" + QString::fromStdString(folderName);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QString getVariable(const String& variableName)
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam(variableName)) {
        return QString::fromStdString(config.getStringValue(variableName));
    } else {
        return QString("");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QStringList getFiles(const QString& folderName, const QStringList fileFilter = QStringList())
{
    QDir dataDir(folderName);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dataDir.setNameFilters(fileFilter);
    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QString getDefaultCapturePath()
{
    return getDefaultPath("Capture");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Int getDefaultSceneID()
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam("SceneID")) {
        return config.getIntValue("SceneID");
    } else {
        return 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QSize getDefaultWindowSize()
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() && config.hasParam("DefaultWindowSizeW") && config.hasParam("DefaultWindowSizeH")) {
        return QSize(config.getIntValue("DefaultWindowSizeW"), config.getIntValue("DefaultWindowSizeH"));
    } else {
        return QSize(1920, 1080);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline Vec4f getDefaultClearColor(const Vec4f& fallBackColor = Vec4f(0.38f, 0.52f, 0.10f, 1.0f))
{
    AppConfigReader config("config.ini");
    if(config.isFileLoaded() &&
       config.hasParam("DefaultClearColorR") &&
       config.hasParam("DefaultClearColorG") &&
       config.hasParam("DefaultClearColorB")) {
        return Vec4f(config.getFloatValue("DefaultClearColorR"),
                     config.getFloatValue("DefaultClearColorG"),
                     config.getFloatValue("DefaultClearColorB"),
                     1.0f);
    } else {
        return fallBackColor;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline auto getDefaultCamera(const Vec3f& fallBackCamPosition = Vec3f(-5, 1, 0), const Vec3f& fallBackCamFocus = Vec3f(0))
{
    Vec3f camPosition = fallBackCamPosition;
    Vec3f camFocus    = fallBackCamFocus;

    AppConfigReader config("config.ini");
    if(config.isFileLoaded()) {
        if(config.hasParam("DefaultCameraPositionX") &&
           config.hasParam("DefaultCameraPositionY") &&
           config.hasParam("DefaultCameraPositionZ")) {
            camPosition = Vec3f(config.getFloatValue("DefaultCameraPositionX"),
                                config.getFloatValue("DefaultCameraPositionY"),
                                config.getFloatValue("DefaultCameraPositionZ"));
        }
        if(config.hasParam("DefaultCameraFocusX") &&
           config.hasParam("DefaultCameraFocusY") &&
           config.hasParam("DefaultCameraFocusZ")) {
            camFocus = Vec3f(config.getFloatValue("DefaultCameraFocusX"),
                             config.getFloatValue("DefaultCameraFocusY"),
                             config.getFloatValue("DefaultCameraFocusZ"));
        }
    }
    return std::make_pair(camPosition, camFocus);;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana