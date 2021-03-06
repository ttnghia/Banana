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

#include <OpenGLHelpers/ShaderProgram.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShaderFromSource(const GLchar* shaderSource)
{
    addShader(GL_VERTEX_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_VERTEX_SHADER] = String(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShaderFromSource(const GLchar* shaderSource)
{
    addShader(GL_GEOMETRY_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_GEOMETRY_SHADER] = String(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShaderFromSource(const GLchar* shaderSource)
{
    addShader(GL_FRAGMENT_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_FRAGMENT_SHADER] = String(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShaderFromFile(const char* fileName)
{
    String shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_VERTEX_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_VERTEX_SHADER] = String(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShaderFromFile(const char* fileName)
{
    String shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_GEOMETRY_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_GEOMETRY_SHADER] = String(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShaderFromFile(const char* fileName)
{
    String shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_FRAGMENT_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_FRAGMENT_SHADER] = String(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::link()
{
    GLuint newProgramID = glCall(glCreateProgram());

    for(GLuint shader : m_ShaderIDs) {
        glCall(glAttachShader(newProgramID, shader));
    }

    glCall(glLinkProgram(newProgramID));

    bool linkSuccess = glCall(checkLinkError(newProgramID));
    if(linkSuccess) {
        if(m_isProgramLinked) {
            glCall(glDeleteProgram(m_ProgramID));
        }

        m_ProgramID       = newProgramID;
        m_isProgramLinked = true;
    }

    for(GLuint shader : m_ShaderIDs) {
        glCall(glDeleteShader(shader));
    }

    m_ShaderIDs.clear();
    return linkSuccess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::reloadShaders()
{
    bool result = true;
    for(auto it = m_ShaderSourceFiles.begin(); it != m_ShaderSourceFiles.end(); ++it) {
        String shaderSouce;
        loadFile(shaderSouce, it->second.c_str());
        result = result && addShader(it->first, shaderSouce.c_str());
    }

    for(auto it = m_ShaderSourceCodes.begin(); it != m_ShaderSourceCodes.end(); ++it) {
        result = result && addShader(it->first, it->second.c_str());
    }

    return (result && link());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::clearCachedSource()
{
    m_ShaderSourceCodes.clear();
    m_ShaderSourceFiles.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getAtributeLocation(const char* atributeName,
                                          bool        dieOnError /* = true*/)
{
    GLint location = glCall(glGetAttribLocation(m_ProgramID, atributeName));

    if(location < 0 && dieOnError) {
#ifdef __Banana_Qt__
        __BNN_DIE(QString("%1: Attribute %2 not found!").arg(QString::fromStdString(m_ProgramName)).arg(atributeName));
#else
        __BNN_DIE(("%s: Attribute %s not found!\n", m_ProgramName.c_str(), atributeName));
#endif
    }

    return static_cast<GLuint>(location);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getUniformLocation(const char* uniformName,
                                         bool        dieOnError /* = true*/)
{
    GLint location = glCall(glGetUniformLocation(m_ProgramID, uniformName));

    if(location < 0 && dieOnError) {
#ifdef __Banana_Qt__
        __BNN_DIE(QString("%1: Uniform location %2 not found!").arg(QString::fromStdString(m_ProgramName)).arg(QString(uniformName)));
#else
        __BNN_DIE(("%s: Uniform location %s not found!\n", m_ProgramName.c_str(), uniformName));
#endif
    }

    return static_cast<GLuint>(location);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getUniformBlockIndex(const char* uniformBlockName, bool dieOnError /*= true*/)
{
    GLuint location = glCall(glGetUniformBlockIndex(m_ProgramID, uniformBlockName));

    if(location == GL_INVALID_INDEX && dieOnError) {
#ifdef __Banana_Qt__
        __BNN_DIE(QString("%1: Uniform block index %2 not found!").arg(QString::fromStdString(m_ProgramName)).arg(QString(uniformBlockName)));
#else
        __BNN_DIE(("%s: Uniform location %s not found!\n", m_ProgramName.c_str(), uniformBlockName));
#endif
    }

    return location;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::ShaderProgram::bindUniformBlock(GLuint blockIndex, GLuint bindingPoint)
{
    glCall(glUniformBlockBinding(m_ProgramID, blockIndex, bindingPoint));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const Mat4x4f& mat)
{
    glCall(glUniformMatrix4fv(location,
                              1 /*only setting 1 matrix*/,
                              false /*transpose?*/,
                              glm::value_ptr(mat)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const GLfloat* data, GLuint count)
{
    switch(count) {
        case 1u:
            glCall(glUniform1f(location, data[0]));
            break;
        case 2u:
            glCall(glUniform2fv(location, 1, data));
            break;
        case 3u:
            glCall(glUniform3fv(location, 1, data));
            break;
        case 4u:
            glCall(glUniform4fv(location, 1, data));
            break;
        default:
            glCall(glUniform1fv(location, count, data));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const GLint* data, GLuint count)
{
    switch(count) {
        case 1u:
            glCall(glUniform1i(location, data[0]));
            break;
        case 2u:
            glCall(glUniform2iv(location, 1, data));
            break;
        case 3u:
            glCall(glUniform3iv(location, 1, data));
            break;
        case 4u:
            glCall(glUniform4iv(location, 1, data));
            break;
        default:
            glCall(glUniform1iv(location, count, data));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::addShader(GLenum shaderType, const GLchar* shaderSource)
{
    GLuint shader = glCall(glCreateShader(shaderType));
    glCall(glShaderSource(shader, 1, &shaderSource, NULL));
    glCall(glCompileShader(shader));

    // => add shader into list
    if(checkCompileError(shader, shaderType)) {
        m_ShaderIDs.push_back(shader);
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::checkCompileError(GLuint shader, GLenum shaderType)
{
    GLint  success;
    GLchar infoLog[1024];

    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

    if(!success) {
        glCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));

        String shaderName = "Unknown";

        if(shaderType == GL_VERTEX_SHADER) {
            shaderName = "VertexShader";
        } else if(shaderType == GL_GEOMETRY_SHADER) {
            shaderName = "GeometryShader";
        } else if(shaderType == GL_FRAGMENT_SHADER) {
            shaderName = "FragmentShader";
        } else {
            shaderName = "InvalidShader";
        }

#ifdef __Banana_Qt__
        __BNN_INFO(QString("%1: %2 failed to compile!\n  => : %3").arg(QString::fromStdString(m_ProgramName)).arg(QString::fromStdString(shaderName)).arg(infoLog));
#else
        std::cout << "ERROR: " << shaderName << " failed to compile!" << std::endl << "  => : " << infoLog << std::endl;
#endif
    }

    return (success != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::checkLinkError(GLuint program)
{
    GLint  success;
    GLchar infoLog[1024];
    glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));

    if(!success) {
        glCall(glGetProgramInfoLog(program, 1024, NULL, infoLog));
#ifdef __Banana_Qt__
        __BNN_DIE(QString("%1: Program failed to link!\n  => : ").arg(QString::fromStdString(m_ProgramName)) + QString(infoLog));
#else
        __BNN_DIE(("%s: Program failed to link!\n   => : ", m_ProgramName.c_str(), infoLog));
#endif
    }

    return (success != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::loadFile(String& fileContent, const char* fileName)
{
    std::ifstream file(fileName);
    if(!file.is_open()) {
#ifdef __Banana_Qt__
        __BNN_DIE(QString("%1: Cannot open file %2 for reading!").arg(QString::fromStdString(m_ProgramName)).arg(QString(fileName)));
#else
        __BNN_DIE(("%s: Cannot open file %s for reading!\n", m_ProgramName.c_str(), fileName));
#endif
    }

    // => allocate memory up front
    file.seekg(0, std::ios::end);
    fileContent.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    // => read file
    fileContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::ShaderProgram::getSimpleVertexColorShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "layout(location = 1) in vec3 v_Color;\n"
        "out vec3 f_Color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(v_Position, 1.0);\n"
        "    f_Color = v_Color;\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "in vec3 f_Color;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{ outColor = vec4(f_Color, 1.0f); }\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::ShaderProgram::getSimpleUniformColorShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(v_Position, 1.0);\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform vec3 f_Color;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{ outColor = vec4(f_Color, 1.0f); }\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::ShaderProgram::getSimpleTextureShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "layout(location = 1) in vec2 v_TexCoord;\n"
        "out vec2 f_TexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(v_Position, 1.0);\n"
        "    f_TexCoord = v_TexCoord;\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform sampler2D u_TexSampler;\n"
        "in vec2 f_TexCoord;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(u_TexSampler, f_TexCoord);\n"
        "}\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getSimpleLightSpaceDepthShader(String programName /*= String("SimpleDepthShader")*/)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "#define NUM_TOTAL_LIGHTS 8\n"
        "struct LightMatrix\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 prjMatrix;\n"
        "};\n"

        "layout(std140) uniform LightMatrices\n"
        "{\n"
        "    LightMatrix lightMatrices[NUM_TOTAL_LIGHTS]; \n"
        "}; \n"
        "\n"
        "uniform int u_LightID;\n"
        "in vec3 v_Position;\n"
        "out float f_LightDistance;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 lightCoord = lightMatrices[u_LightID].viewMatrix * modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_LightDistance = lightCoord.z;\n"
        "    gl_Position     = lightMatrices[u_LightID].prjMatrix * lightCoord;\n"
        "}";


    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "in float f_LightDistance;\n"
        "out float outDepth;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outDepth = f_LightDistance;\n"
        "}\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getSimpleCameraSpaceDepthShader(String programName /*= String("SimpleCameraSpaceDepthShader")*/)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "in vec3 v_Position;\n"
        "out float f_CameraDistance;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 eyeCoord    = viewMatrix * modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_CameraDistance = eyeCoord.z;\n"
        "    gl_Position      = projectionMatrix * eyeCoord;\n"
        "}";


    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "in float f_CameraDistance;\n"
        "out float outDepth;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outDepth = f_CameraDistance;\n"
        "}\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getObjUniformColorShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    gl_Position     = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";


    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform vec3 f_Color;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{ outColor = vec4(f_Color, 1.0f); }\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getObjTextureShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "layout(location = 1) in vec2 v_TexCoord;\n"
        "out vec2 f_TexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_TexCoord      = v_TexCoord;\n"
        "    gl_Position     = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";


    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform sampler2D u_TexSampler;\n"
        "in vec2 f_TexCoord;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(u_TexSampler, f_TexCoord);\n"
        "}\n";


    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::ShaderProgram::getPhongShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "#define NUM_TOTAL_LIGHTS 8\n"
        "struct LightMatrix\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 prjMatrix;\n"
        "};\n"

        "layout(std140) uniform LightMatrices\n"
        "{\n"
        "    LightMatrix lightMatrices[NUM_TOTAL_LIGHTS]; \n"
        "}; \n"
        "\n"
        "struct PointLight\n"
        "{\n"
        "    vec4 ambient; \n"
        "    vec4 diffuse; \n"
        "    vec4 specular; \n"
        "    vec4 position; \n"
        "};\n"

        "layout(std140) uniform Lights\n"
        "{\n"
        "    PointLight lights[NUM_TOTAL_LIGHTS]; \n"
        "    int u_NumLights; \n"
        "}; \n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "uniform int u_HasShadow;\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "layout(location = 1) in vec3 v_Normal;\n"
        "layout(location = 2) in vec2 v_TexCoord;\n"
        "out VS_OUT\n"
        "{\n"
        "    float f_LightDistance[NUM_TOTAL_LIGHTS];\n"
        "    vec4  f_FragPosLightSpace[NUM_TOTAL_LIGHTS];\n"
        "    vec3 f_Normal;\n"
        "    vec3 f_FragPos;\n"
        "    vec3 f_ViewDir; \n"
        "    vec2 f_TexCoord;\n"
        "};\n"
        "\n"
        "void main()\n"
        "{\n"
        "    f_Normal = vec3(normalMatrix* vec4(v_Normal, 1.0));\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_FragPos = vec3(worldCoord);\n"
        "    f_ViewDir = vec3(camPosition)- vec3(worldCoord);\n"
        "    f_TexCoord = v_TexCoord;\n"
        "    if(u_HasShadow == 1)\n"
        "    {\n"
        "        for(int i = 0; i < u_NumLights; ++i)\n"
        "        {\n"
        "            vec4 lightCoord = lightMatrices[i].viewMatrix * worldCoord;\n"
        "            f_LightDistance[i] = lightCoord.z;\n"
        "            f_FragPosLightSpace[i] = lightMatrices[i].prjMatrix * lightCoord;\n"
        "        }\n"
        "    }\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "#define NUM_TOTAL_LIGHTS 8\n"
        "struct PointLight\n"
        "{\n"
        "    vec4 ambient; \n"
        "    vec4 diffuse; \n"
        "    vec4 specular; \n"
        "    vec4 position; \n"
        "};\n"

        "layout(std140) uniform Lights\n"
        "{\n"
        "    PointLight lights[NUM_TOTAL_LIGHTS]; \n"
        "    int u_NumLights; \n"
        "}; \n"
        "\n"
        "layout(std140) uniform Material\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    float shininess;\n"
        "} material;\n"
        "\n"
        "uniform int u_HasTexture;\n"
        "uniform int u_HasShadow;\n"
        "uniform sampler2D u_TexSampler;\n"
        "uniform sampler2D u_ShadowMap[NUM_TOTAL_LIGHTS];\n"
        "#define SHADOW_BIAS -0.05\n"
        "\n"
        "uniform float u_Exposure = 1.0f;\n"
        "\n"
        "in VS_OUT\n"
        "{\n"
        "    float f_LightDistance[NUM_TOTAL_LIGHTS];\n"
        "    vec4  f_FragPosLightSpace[NUM_TOTAL_LIGHTS];\n"
        "    vec3 f_Normal;\n"
        "    vec3 f_FragPos;\n"
        "    vec3 f_ViewDir; \n"
        "    vec2 f_TexCoord;\n"
        "};\n"
        "out vec4 outColor;\n"
        "\n"
        "vec3 shadeLight(int lightID, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 f_TexCoord)\n"
        "{\n"
        "    float inShadow = 0.0;\n"
        "    if(u_HasShadow == 1)\n"
        "    {\n"
        "        vec3 projCoords = f_FragPosLightSpace[lightID].xyz / f_FragPosLightSpace[lightID].w;\n"
        "        projCoords = clamp(projCoords * 0.5 + 0.5, 0, 1); // Transform to [0,1] range\n"
        "        vec2 texelSize = 1.0 / textureSize(u_ShadowMap[lightID], 0);\n"
        "        for(int x = -1; x <= 1; ++x)\n"
        "        {\n"
        "            for(int y = -1; y <= 1; ++y)\n"
        "            {\n"
        "                float pcfDepth = texture(u_ShadowMap[lightID], projCoords.xy + vec2(x, y) * texelSize).r;\n"
        "                inShadow += (f_LightDistance[lightID] < pcfDepth + SHADOW_BIAS) ? 1.0 : 0.0;\n"
        "            }\n"
        "        }\n"
        "        inShadow /= 9.0; \n"
        "    }\n"
        "    vec3 lightDir = normalize(vec3(lights[lightID].position) - fragPos);\n"
        "    vec3 halfDir  = normalize(lightDir + viewDir);\n"
        "\n"
        "    vec4 ambientColor  = lights[lightID].ambient;\n"
        "    vec4 diffuseColor  = lights[lightID].diffuse * vec4(max(dot(normal, lightDir), 0.0));;\n"
        "    vec4 specularColor = lights[lightID].specular * pow(max(dot(halfDir, normal), 0.0), material.shininess);\n"
        "    if(u_HasTexture == 1)\n"
        "    {\n"
        "        vec4 objColor  = texture(u_TexSampler, f_TexCoord);\n"
        "        ambientColor  *= (objColor * 0.2);\n"
        "        diffuseColor  *= objColor;\n"
        "        specularColor *= objColor;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        ambientColor  *= material.ambient;\n"
        "        diffuseColor  *= material.diffuse;\n"
        "        specularColor *= material.specular;\n"
        "    }\n"
        "\n"
        "    return vec3(ambientColor + (1.0 - inShadow)*(diffuseColor + specularColor));\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 normal = normalize(f_Normal);\n"
        "    vec3 viewDir = normalize(f_ViewDir);\n"
        "    vec3 shadeColor = vec3(0, 0, 0);\n"
        "\n"
        "    for(int i = 0; i < u_NumLights; ++i)\n"
        "        shadeColor += shadeLight(i, normal, f_FragPos, viewDir, f_TexCoord);\n"
        "\n"
        "    outColor = vec4(u_Exposure * shadeColor, 1.0);\n"
        "}\n";

    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::ShaderProgram::getSkyBoxShader(String programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform ModelMatrix\n"
        "{\n"
        "    mat4 modelMatrix;\n"
        "    mat4 normalMatrix;\n"
        "};\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "out vec3 f_ViewDir;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_ViewDir = vec3(worldCoord) - vec3(camPosition);\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";


    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform samplerCube u_TexSampler;\n"
        "in vec3 f_ViewDir;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec3 viewDir = normalize(f_ViewDir);\n"
        "   outColor = texture(u_TexSampler, viewDir);\n"

        "}";


    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getScreenQuadShader(const char* fragmentShaderSource,
                                                            String      programName)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "// vertex shader, screen quad\n"
        "#version 410 core\n"
        "\n"
        "out vec2 f_TexCoord; \n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec2 pos[] = vec2[](vec2(-1.0, -1.0),\n"
        "                        vec2(1.0, -1.0),\n"
        "                        vec2(-1.0, 1.0),\n"
        "                        vec2(1.0, 1.0));\n"
        "\n"
        "    vec2 tex[] = vec2[](vec2(0.0, 0.0),\n"
        "                        vec2(1.0, 0.0),\n"
        "                        vec2(0.0, 1.0),\n"
        "                        vec2(1.0, 1.0));\n"
        "\n"
        "    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);\n"
        "    f_TexCoord = tex[gl_VertexID];\n"
        "}\n";


    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShaderSource);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getScreenQuadShaderFromFile(const char* fragmentShaderFile, String programName /*=String("ScreenQuadShader")*/)
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "// vertex shader, screen quad\n"
        "#version 410 core\n"
        "\n"
        "out vec2 f_TexCoord; \n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec2 pos[] = vec2[](vec2(-1.0, -1.0),\n"
        "                        vec2(1.0, -1.0),\n"
        "                        vec2(-1.0, 1.0),\n"
        "                        vec2(1.0, 1.0));\n"
        "\n"
        "    vec2 tex[] = vec2[](vec2(0.0, 0.0),\n"
        "                        vec2(1.0, 0.0),\n"
        "                        vec2(0.0, 1.0),\n"
        "                        vec2(1.0, 1.0));\n"
        "\n"
        "    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);\n"
        "    f_TexCoord = tex[gl_VertexID];\n"
        "}\n";


    SharedPtr<ShaderProgram> shader = std::make_shared<ShaderProgram>(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromFile(fragmentShaderFile);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getCheckerboardBackgroundShader(String programName)
{
    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "// fragment shader, CheckerboardBackground\n"
        "#version 410 core\n"
        "\n"
        "uniform vec4 u_Color1 = vec4(1, 1, 1, 1);\n"
        "uniform vec4 u_Color2 = vec4(0, 0, 0, 1);\n"
        "uniform ivec2 u_Scales = ivec2(20, 20);\n"
        "uniform int u_ScreenWidth = 2;\n"
        "uniform int u_ScreenHeight= 2;\n"
        "in vec2 f_TexCoord; \n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   int xPixel = int(floor(f_TexCoord.x * float(u_ScreenWidth) / float(u_Scales.x)));\n"
        "   int yPixel = int(floor(f_TexCoord.y * float(u_ScreenHeight) / float(u_Scales.y)));\n"
        "   bool isEven = mod(xPixel + yPixel, 2) == 0;\n"
        "   outColor = isEven ? u_Color1 : u_Color2;\n"

        "}";


    return getScreenQuadShader(fragmentShader, programName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<ShaderProgram> ShaderProgram::getGridBackgroundShader(String programName)
{
    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "// fragment shader, CheckerboardBackground\n"
        "#version 410 core\n"
        "\n"
        "uniform vec4 m_UBackgroundColor = vec4(1, 1, 1, 1);\n"
        "uniform vec4 m_ULineColor = vec4(0, 0, 0, 1);\n"
        "uniform ivec2 u_Scales = ivec2(20, 20);\n"
        "uniform int u_ScreenWidth = 1;\n"
        "uniform int u_ScreenHeight= 1;\n"
        "in vec2 f_TexCoord; \n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   int xPixel = int(floor(f_TexCoord.x * float(u_ScreenWidth)));\n"
        "   int yPixel = int(floor(f_TexCoord.y * float(u_ScreenHeight)));\n"
        "   bool inLine_x = mod(xPixel, u_Scales.x) == 0;\n"
        "   bool inLine_y = mod(yPixel, u_Scales.y) == 0;\n"
        "   outColor = (inLine_x || inLine_y) ? m_ULineColor : m_UBackgroundColor;\n"

        "}";


    return getScreenQuadShader(fragmentShader, programName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana