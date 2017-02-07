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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <BananaAppCore/Macros.h>

#if defined(QT_GUI_LIB) || defined(QT_CORE_LIB)
#   include <QtAppHelpers/QtAppMacros.h>
#else // no Qt
#   define GLEW_STATIC
#   include <GL/glew.h>
#endif

#include <string>

        //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

#ifdef _MSC_VER
#   define DEBUG_BREAK __debugbreak();
#else
#   define DEBUG_BREAK ;
#endif

#ifdef _DEBUG
#   ifdef __Banana_Qt__ 
#   define glCall(a)\
    a; {\
        GLenum err = glGetError();\
        if (err!=GL_NO_ERROR) {\
            std::string str = "GL error when calling '"+std::string(#a)+"'"\
            ". OpenGL error: "+GLErr2Str(err)+ \
            ", in file: "+__FILE__+", line: "+std::to_string(__LINE__);\
            qDebug() << QString::fromStdString(str); \
            DEBUG_BREAK\
        }\
        }
#   else 
#   define glCall(a)\
    a; {\
        GLenum err = glGetError();\
        if (err!=GL_NO_ERROR) {\
            std::string str = "GL error when calling '"+std::string(#a)+"'"\
            ". OpenGL error: "+GLErr2Str(err)+ \
            ", in file: "+__FILE__+", line: "+std::to_string(__LINE__);\
            fprintf(stderr, "%s\n", str.c_str()); \
            DEBUG_BREAK\
        }\
        } 
#   endif
#else // NO _DEBUG
#   define glCall(a) a;
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// glm setup
#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Abstract base class for all classes that call opengl functions
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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
    OpenGLCallable()
    {}
};
#endif