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

#ifdef __Banana_Qt__
#   include <QMessageBox>
#endif

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
void ShaderProgram::reloadShaders()
{
    for(auto it = m_ShaderSourceFiles.begin(); it != m_ShaderSourceFiles.end(); ++it)
    {
        std::string shaderSouce;
        loadFile(shaderSouce, it->second.c_str());
        addShader(it->first, shaderSouce.c_str());
    }

    if(link())
    {
#ifdef __Banana_Qt__
        QMessageBox::information(nullptr, QString("Info"),
                                 QString("Successfully reload shaders!"));
#else
        std::cout << "Successfully reload shaders!" << std::endl;
#endif
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLint ShaderProgram::getAtributeLocation(const char * atributeName)
{
    GLint location = glCall(glGetAttribLocation(programID, atributeName));

    if(location < 0)
    {
        fprintf(stderr, "Attribute %s not found!\n", atributeName);
        exit(EXIT_FAILURE);
    }

    return location;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLint ShaderProgram::getUniformLocation(const char * uniformName)
{
    GLint location = glCall(glGetUniformLocation(programID, uniformName));

    if(location < 0)
    {
        fprintf(stderr, "Uniform location %s not found!\n", uniformName);
        exit(EXIT_FAILURE);
    }

    return location;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, glm::mat4 mat)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniformMatrix4fv(location,
           1 /*only setting 1 matrix*/,
           false /*transpose?*/,
           glm::value_ptr(mat)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, glm::vec4 vec)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform4fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, glm::vec3 vec)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform3fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, glm::vec2 vec)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform2fv(location, 1, glm::value_ptr(vec)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, GLfloat value)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform1f(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, GLint value)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform1i(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, GLuint value)
{
    GLint location = getUniformLocation(uniformName);
    glCall(glUniform1ui(location, value));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ShaderProgram::setUniformValue(const char * uniformName, GLboolean value)
{
    GLint location = getUniformLocation(uniformName);
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
        QMessageBox::critical(nullptr, QString("Error"),
                              QString("Program failed to link!\n  => : ")
                              + QString(infoLog));
#else
        std::cout << "ERROR: Program failed to link!" << std::endl
            << "  => : " << infoLog << std::endl;
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
        QMessageBox::critical(nullptr, QString("Error"),
                              QString("Cannot open file %1 for reading!")
                              .arg(QString(fileName)));
#else
        printf("Error: Cannot open file %s for reading!\n", fileName);
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
