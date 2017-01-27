//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <OpenGLHelpers/ShaderProgram.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::isValid()
{
    return m_isProgramLinked;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShader(const GLchar * shaderSource)
{
    addShader(GL_VERTEX_SHADER, shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShader(const GLchar * shaderSource)
{
    addShader(GL_GEOMETRY_SHADER, shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShader(const GLchar * shaderSource)
{
    addShader(GL_FRAGMENT_SHADER, shaderSource);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addVertexShaderFromFile(const char * fileName)
{
    std::string shaderSouce;
    loadFile(shaderSouce, fileName);

    addVertexShader(shaderSouce.c_str());
    m_ShaderSourceFiles[GL_VERTEX_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addGeometryShaderFromFile(const char * fileName)
{
    std::string shaderSouce;
    loadFile(shaderSouce, fileName);

    addGeometryShader(shaderSouce.c_str());
    m_ShaderSourceFiles[GL_GEOMETRY_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addFragmentShaderFromFile(const char * fileName)
{
    std::string shaderSouce;
    loadFile(shaderSouce, fileName);

    addFragmentShader(shaderSouce.c_str());
    m_ShaderSourceFiles[GL_FRAGMENT_SHADER] = std::string(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
            glCall(glDeleteShader(programID));
        }

        programID = newProgramID;
        m_isProgramLinked = true;
    }

    for(GLuint shader : m_ShaderIDs)
    {
        glCall(glDeleteShader(shader));
    }

    m_ShaderIDs.clear();
    return linkSuccess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ShaderProgram::reloadShaders()
{
    for(auto it = m_ShaderSourceFiles.begin(); it != m_ShaderSourceFiles.end(); ++it)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, it->second.c_str());
        addShader(it->first, shaderSouce.c_str());
    }

    return link();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLint ShaderProgram::getAtributeLocation(const char * atributeName,
                                         bool dieOnError/* = true*/)
{
    GLint location = glCall(glGetAttribLocation(programID, atributeName));

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

    return location;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLint ShaderProgram::getUniformLocation(const char * uniformName,
                                        bool dieOnError/* = true*/)
{
    GLint location = glCall(glGetUniformLocation(programID, uniformName));

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

    return location;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint ShaderProgram::getUniformBlockIndex(const char* uniformBlockName, bool dieOnError /*= true*/)
{
    GLuint location = glCall(glGetUniformBlockIndex(programID, uniformBlockName));

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::mat4& mat)
{
    glCall(glUniformMatrix4fv(location,
           1 /*only setting 1 matrix*/,
           false /*transpose?*/,
           glm::value_ptr(mat)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec4& vec)
{
    glCall(glUniform4fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec3& vec)
{
    glCall(glUniform3fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, const glm::vec2& vec)
{
    glCall(glUniform2fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLfloat value)
{
    glCall(glUniform1f(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLint value)
{
    glCall(glUniform1i(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLuint value)
{
    glCall(glUniform1ui(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(GLint location, GLboolean value)
{
    glCall(glUniform1i(location, value ? GL_TRUE : GL_FALSE));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::addShader(GLenum shaderType, const GLchar * shaderSource)
{
    GLuint shader = glCall(glCreateShader(shaderType));
    glCall(glShaderSource(shader, 1, &shaderSource, NULL));
    glCall(glCompileShader(shader));

    // => add shader into list
    if(checkCompileError(shader, shaderType))
    {
        m_ShaderIDs.push_back(shader);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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


        std::cout << "ERROR: " << shaderName << " failed to compile!" << std::endl
            << "  => : " << infoLog << std::endl;
    }

    return (success != 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
        exit(EXIT_FAILURE);
    }

    // => allocate memory up front
    file.seekg(0, std::ios::end);
    fileContent.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    // => read file
    fileContent.assign((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

