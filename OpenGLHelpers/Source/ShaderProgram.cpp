//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/24/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <OpenGLHelpers/ShaderProgram.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::isValid()
{
    return m_isProgramLinked;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShaderFromSource(const GLchar * shaderSource)
{
    addShader(GL_VERTEX_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_VERTEX_SHADER] = std::string(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShaderFromSource(const GLchar * shaderSource)
{
    addShader(GL_GEOMETRY_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_GEOMETRY_SHADER] = std::string(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShaderFromSource(const GLchar * shaderSource)
{
    addShader(GL_FRAGMENT_SHADER, shaderSource);
    m_ShaderSourceCodes[GL_FRAGMENT_SHADER] = std::string(shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShaderFromFile(const char * fileName)
{
    std::string shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_VERTEX_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_VERTEX_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShaderFromFile(const char * fileName)
{
    std::string shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_GEOMETRY_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_GEOMETRY_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShaderFromFile(const char * fileName)
{
    std::string shaderSource;
    loadFile(shaderSource, fileName);

    addShader(GL_FRAGMENT_SHADER, shaderSource.c_str());
    m_ShaderSourceFiles[GL_FRAGMENT_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::link()
{
    GLuint newProgramID = glCall(glCreateProgram());

    for(GLuint shader : m_ShaderIDs)
    {
        glCall(glAttachShader(newProgramID, shader));
    }

    glCall(glLinkProgram(newProgramID));

    bool linkSuccess = glCall(checkLinkError(newProgramID));
    if(linkSuccess)
    {
        if(m_isProgramLinked)
        {
            glCall(glDeleteProgram(m_ProgramID));
        }

        m_ProgramID = newProgramID;
        m_isProgramLinked = true;
    }

    for(GLuint shader : m_ShaderIDs)
    {
        glCall(glDeleteShader(shader));
    }

    m_ShaderIDs.clear();
    return linkSuccess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::reloadShaders()
{
    bool result = true;
    for(auto it = m_ShaderSourceFiles.begin(); it != m_ShaderSourceFiles.end(); ++it)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, it->second.c_str());
        result = result && addShader(it->first, shaderSouce.c_str());
    }

    for(auto it = m_ShaderSourceCodes.begin(); it != m_ShaderSourceCodes.end(); ++it)
    {
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
GLuint ShaderProgram::getAtributeLocation(const char * atributeName,
                                          bool dieOnError/* = true*/)
{
    GLint location = glCall(glGetAttribLocation(m_ProgramID, atributeName));

    if(location < 0 && dieOnError)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("%1: Attribute %2 not found!")
                  .arg(QString::fromStdString(m_ProgramName))
                  .arg(atributeName));
#else
        __BNN_Die("%s: Attribute %s not found!\n", m_ProgramName.c_str(), atributeName);
#endif
    }

    return static_cast<GLuint>(location);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getUniformLocation(const char * uniformName,
                                         bool dieOnError/* = true*/)
{
    GLint location = glCall(glGetUniformLocation(m_ProgramID, uniformName));

    if(location < 0 && dieOnError)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("%1: Uniform location %2 not found!")
                  .arg(QString::fromStdString(m_ProgramName))
                  .arg(QString(uniformName)));
#else
        __BNN_Die("%s: Uniform location %s not found!\n", m_ProgramName.c_str(), uniformName);
#endif
    }

    return static_cast<GLuint>(location);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getUniformBlockIndex(const char* uniformBlockName, bool dieOnError /*= true*/)
{
    GLuint location = glCall(glGetUniformBlockIndex(m_ProgramID, uniformBlockName));

    if(location == GL_INVALID_INDEX && dieOnError)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("%1: Uniform block index %2 not found!")
                  .arg(QString::fromStdString(m_ProgramName))
                  .arg(QString(uniformBlockName)));
#else
        __BNN_Die("%s: Uniform location %s not found!\n", m_ProgramName.c_str(), uniformBlockName);
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
void ShaderProgram::setUniformValue(GLint location, const glm::mat4& mat)
{
    glCall(glUniformMatrix4fv(location,
           1 /*only setting 1 matrix*/,
           false /*transpose?*/,
           glm::value_ptr(mat)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec4& vec)
{
    glCall(glUniform4fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec3& vec)
{
    glCall(glUniform3fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec2& vec)
{
    glCall(glUniform2fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLfloat value)
{
    glCall(glUniform1f(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLint value)
{
    glCall(glUniform1i(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLuint value)
{
    glCall(glUniform1ui(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLboolean value)
{
    glCall(glUniform1i(location, value ? GL_TRUE : GL_FALSE));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::addShader(GLenum shaderType, const GLchar * shaderSource)
{
    GLuint shader = glCall(glCreateShader(shaderType));
    glCall(glShaderSource(shader, 1, &shaderSource, NULL));
    glCall(glCompileShader(shader));

    // => add shader into list
    if(checkCompileError(shader, shaderType))
    {
        m_ShaderIDs.push_back(shader);
        return true;
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::checkCompileError(GLuint shader, GLenum shaderType)
{
    GLint success;
    GLchar infoLog[1024];

    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

    if(!success)
    {
        glCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));

        std::string shaderName = "Unknown";

        if(shaderType == GL_VERTEX_SHADER)
        {
            shaderName = "VertexShader";
        }
        else if(shaderType == GL_GEOMETRY_SHADER)
        {
            shaderName = "GeometryShader";
        }
        else if(shaderType == GL_FRAGMENT_SHADER)
        {
            shaderName = "FragmentShader";
        }
        else
        {
            shaderName = "InvalidShader";
        }

#ifdef __Banana_Qt__
        __BNN_Info(QString("%1: %2 failed to compile!\n  => : %3")
                   .arg(QString::fromStdString(m_ProgramName))
                   .arg(QString::fromStdString(shaderName)).arg(infoLog));
#else
        std::cout << "ERROR: " << shaderName << " failed to compile!" << std::endl
            << "  => : " << infoLog << std::endl;
#endif
    }

    return (success != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::checkLinkError(GLuint program)
{
    GLint success;
    GLchar infoLog[1024];
    glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));

    if(!success)
    {
        glCall(glGetProgramInfoLog(program, 1024, NULL, infoLog));
#ifdef __Banana_Qt__
        __BNN_Die(QString("%1: Program failed to link!\n  => : ")
                  .arg(QString::fromStdString(m_ProgramName))
                  + QString(infoLog));
#else
        __BNN_Die("%s: Program failed to link!\n   => : ", m_ProgramName.c_str(),
                  infoLog);
#endif
    }

    return (success != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::loadFile(std::string & fileContent, const char * fileName)
{
    std::ifstream file(fileName);
    if(!file.is_open())
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("%1: Cannot open file %2 for reading!")
                  .arg(QString::fromStdString(m_ProgramName))
                  .arg(QString(fileName)));
#else
        __BNN_Die("%s: Cannot open file %s for reading!\n", m_ProgramName.c_str(), fileName);
#endif
    }

    // => allocate memory up front
    file.seekg(0, std::ios::end);
    fileContent.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    // => read file
    fileContent.assign((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::ShaderProgram::getSimpleVertexColorShader(std::string programName)
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

    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram * ShaderProgram::ShaderProgram::getSimpleUniformColorShader(std::string programName)
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

    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::ShaderProgram::getSimpleTextureShader(std::string programName)
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

    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::getObjUniformColorShader(std::string programName)
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
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
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

    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::getObjTextureShader(std::string programName)
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
        "    f_TexCoord = v_TexCoord;\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
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


    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram * ShaderProgram::ShaderProgram::getPhongShader(std::string programName)
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
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(std140) uniform PointLight\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    vec4 position;\n"
        "} light;\n"
        "\n"
        "layout(location = 0) in vec3 v_Position;\n"
        "layout(location = 1) in vec3 v_Normal;\n"
        "layout(location = 2) in vec2 v_TexCoord;\n"
        "out VS_OUT\n"
        "{\n"
        "    vec3 f_Normal;\n"
        "    vec3 f_LightDir;\n"
        "    vec3 f_ViewDir; \n"
        "    vec2 f_TexCoord;\n"
        "};\n"
        "\n"
        "void main()\n"
        "{\n"
        "    f_Normal = vec3(normalMatrix* vec4(v_Normal, 1.0));\n"
        "    vec4 worldCoord = modelMatrix * vec4(v_Position, 1.0);\n"
        "    f_LightDir = vec3(light.position) - vec3(worldCoord);\n"
        "    f_ViewDir = vec3(camPosition)- vec3(worldCoord);\n"
        "    f_TexCoord = v_TexCoord;\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform PointLight\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    vec4 position;\n"
        "} light;\n"
        "\n"
        "layout(std140) uniform Material\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    float shininess;\n"
        "} material;\n"
        "\n"
        "uniform int hasTexture;\n"
        "uniform sampler2D u_TexSampler;\n"
        "\n"
        "in VS_OUT\n"
        "{\n"
        "   vec3 f_Normal;\n"
        "   vec3 f_LightDir;\n"
        "   vec3 f_ViewDir; \n"
        "   vec2 f_TexCoord;\n"
        "};\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 normal = normalize(f_Normal);\n"
        "    vec3 lightDir = normalize(f_LightDir);\n"
        "    vec3 viewDir = normalize(f_ViewDir);\n"
        "    vec3 halfDir = normalize(lightDir + viewDir);\n"
        "\n"
        "    vec4 ambientColor = light.ambient * material.ambient;\n"
        "    vec4 diffuseColor = light.diffuse * vec4(max(dot(normal, lightDir), 0.0));\n"
        "    vec4 specularColor = light.specular * pow(max(dot(halfDir, normal), 0.0), material.shininess);\n"
        "    if(hasTexture == 1)\n"
        "    {\n"
        "        diffuseColor *= texture(u_TexSampler, f_TexCoord);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        diffuseColor *= material.diffuse;\n"
        "        specularColor *= material.specular;\n"
        "    }\n"
        "\n"
        "    outColor = ambientColor + diffuseColor + specularColor;\n"
        "}\n";

    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::ShaderProgram::getSkyBoxShader(std::string programName)
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


    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShader);
    shader->link();

    return shader;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::getScreenQuadShader(const char* fragmentShaderSource,
                                                  std::string programName)
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


    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromSource(fragmentShaderSource);
    shader->link();

    return shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ShaderProgram* ShaderProgram::getScreenQuadShaderFromFile(const char* fragmentShaderFile, std::string programName /*=std::string("ScreenQuadShader")*/)
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
        "    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0); \n"
        "    f_TexCoord = pos[gl_VertexID] * vec2(-0.5, 0.5) + vec2(0.5, 0.5); \n"
        "}\n";


    ShaderProgram* shader = new ShaderProgram(programName);
    shader->addVertexShaderFromSource(vertexShader);
    shader->addFragmentShaderFromFile(fragmentShaderFile);
    shader->link();

    return shader;
}
