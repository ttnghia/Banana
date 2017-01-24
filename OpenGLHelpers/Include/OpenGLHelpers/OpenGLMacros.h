//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/20/2017
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(QT_GUI_LIB) || defined(QT_CORE_LIB)
#ifdef __APPLE__
#   include <OpenGL.h>
#      include <QOpenGLFunctions_4_1_Core>
typedef  QOpenGLFunctions_4_1_Core OpenGLFunctions;
#   else
#      include <QOpenGLFunctions_4_5_Core>
typedef  QOpenGLFunctions_4_5_Core OpenGLFunctions;
#   endif
#   ifndef __Banana_Qt__
#      define __Banana_Qt__
#   endif
#else
#   define GLEW_STATIC
#   include <GL/glew.h>
#endif

#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline std::string GLErr2Str(GLenum err)
{
    switch(err)
    {
        case GL_NO_ERROR:
            return std::string("No error.");

        case GL_INVALID_ENUM:
            return std::string("Invalid enum.");

        case GL_INVALID_VALUE:
            return std::string("Invalid value.");

        case GL_INVALID_OPERATION:
            return std::string("Invalid operation.");

        case GL_STACK_OVERFLOW:
            return std::string("Stack overflow.");

        case GL_STACK_UNDERFLOW:
            return std::string("Stack underflow.");

        case GL_OUT_OF_MEMORY:
            return std::string("Out of memory.");

        default:
            return std::string("Unknown error.");
    }
}

#if _DEBUG
#   define glCall(a)\
    a; {\
        GLenum err = glGetError();\
        if (err!=GL_NO_ERROR) {\
            std::string str = "GL error when calling\n\""+std::string(#a)+\
            "\"\n\nOpenGL error: "+GLErr2Str(err)+ \
            ", in file: "+__FILE__+", line: "+std::to_string(__LINE__);\
        }\
    }
#else
#   define glCall(a) a;
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// glm setup
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
