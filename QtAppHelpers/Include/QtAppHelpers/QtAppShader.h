//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
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

#include <OpenGLHelpers/Shader.h>
#include <QFile>
#include <QTextStream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class QtAppShader : public Shader
{
public:

    QtAppShader()
    {}

    ////////////////////////////////////////////////////////////////////////////////
    void addVertexShaderFromResource(const char* fileName)
    {
        std::string shaderSouce;
        loadResourceFile(shaderSouce, fileName);

        addVertexShader(shaderSouce.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addGeometryShaderFromResource(const char* fileName)
    {
        std::string shaderSouce;
        loadResourceFile(shaderSouce, fileName);

        addGeometryShader(shaderSouce.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////
    void addFragmentShaderFromResource(const char* fileName)
    {
        std::string shaderSouce;
        loadResourceFile(shaderSouce, fileName);

        addFragmentShader(shaderSouce.c_str());
    }

protected:
    void loadResourceFile(std::string& fileContent, const char* fileName)
    {
        QFile file(fileName);

        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            qDebug() << "Could not open file for read";
            exit(EXIT_FAILURE);
        }

        QTextStream in(&file);
        fileContent = in.readAll().toStdString();
        file.close();
    }
};