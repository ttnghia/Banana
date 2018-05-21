//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Setup.h>

#if defined(QT_GUI_LIB) || defined(QT_CORE_LIB)
#   include <QtAppHelpers/QtAppMacros.h>
#else // no Qt
#   define GLEW_STATIC
//#   include <GL/glew.h>
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline String GLErr2Str(GLenum err)
{
    switch(err) {
        case GL_NO_ERROR:
            return String("No error.");

        case GL_INVALID_ENUM:
            return String("Invalid enum.");

        case GL_INVALID_VALUE:
            return String("Invalid value.");

        case GL_INVALID_OPERATION:
            return String("Invalid operation.");

        case GL_STACK_OVERFLOW:
            return String("Stack overflow.");

        case GL_STACK_UNDERFLOW:
            return String("Stack underflow.");

        case GL_OUT_OF_MEMORY:
            return String("Out of memory.");

        default:
            return String("Unknown error.");
    }
}

#ifdef _MSC_VER
#   define DEBUG_BREAK __debugbreak();
#else
#   define DEBUG_BREAK ;
#endif

#ifdef _DEBUG
#   ifdef __Banana_Qt__
#   define glCall(a)                                                                                    \
    a; {                                                                                                \
        GLenum err = glGetError();                                                                      \
        if(err != GL_NO_ERROR) {                                                                        \
            String str = "GL error when calling '" + String(#a) + "'"                                   \
                                                                  ". OpenGL error: " + GLErr2Str(err) + \
                         ", in file: " + __FILE__ + ", line: " + std::to_string(__LINE__);              \
            qDebug() << QString::fromStdString(str);                                                    \
            DEBUG_BREAK                                                                                 \
        }                                                                                               \
    }
#   else
#   define glCall(a)                                                                                    \
    a; {                                                                                                \
        GLenum err = glGetError();                                                                      \
        if(err != GL_NO_ERROR) {                                                                        \
            String str = "GL error when calling '" + String(#a) + "'"                                   \
                                                                  ". OpenGL error: " + GLErr2Str(err) + \
                         ", in file: " + __FILE__ + ", line: " + std::to_string(__LINE__);              \
            fprintf(stderr, "%s\n", str.c_str());                                                       \
            DEBUG_BREAK                                                                                 \
        }                                                                                               \
    }
#   endif
#else // NO _DEBUG
#   define glCall(a) a;
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Abstract base class for all classes that call opengl functions
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
#ifdef __Banana_Qt__
class OpenGLCallable : public OpenGLFunctions
{
protected:
    OpenGLCallable()
    {
        initializeOpenGLFunctions();
    }
};
#else
class OpenGLCallable
{
protected:
    OpenGLCallable() {}
};
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana