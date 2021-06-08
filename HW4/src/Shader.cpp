
#include <Shader.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // read vertex and fragment files
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try 
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        cout << "SUCCESS::SHADER::FILE_IS_SUCCESFULLY_READED" << endl;
    }
    catch(ifstream::failure const& e)
    {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // compile shaders and create shader program
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    else
    {
        cout << "SUCCESS::SHADER::VERTEX::COMPILATION_SUCCESSED" << endl;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }
    else
    {
        cout << "SUCCESS::SHADER::FRAGMENT::COMPILATION_SUCCESSED" << endl;
    }

    // shader Program
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    else
    {
        cout << "SUCCESS::SHADER::PROGRAM::LINKING_SUCCESSED" << endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() 
{
    glUseProgram(programID);
}

void Shader::setBool(const string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}
void Shader::setInt(const string &name, int value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::setUInt(const string &name, unsigned int value) const
{
    glUniform1ui(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::setFloat(const string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setBoolVec(const string &name, vector<bool> value, int nElement) const
{
    vector<int> intValue(value.begin(), value.end());
    switch(nElement)
    {
        case 1:
            glUniform1iv(glGetUniformLocation(programID, name.c_str()), 1, &intValue[0]);
            break;
        case 2:
            glUniform2iv(glGetUniformLocation(programID, name.c_str()), 1, &intValue[0]);
            break;
        case 3:
            glUniform3iv(glGetUniformLocation(programID, name.c_str()), 1, &intValue[0]);
            break;
        case 4:
            glUniform4iv(glGetUniformLocation(programID, name.c_str()), 1, &intValue[0]);
            break;
    }
}
void Shader::setIntVec(const string &name, vector<int> value, int nElement) const
{
    switch(nElement)
    {
        case 1:
            glUniform1iv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 2:
            glUniform2iv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 3:
            glUniform3iv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 4:
            glUniform4iv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
    }
}
void Shader::setUIntVec(const string &name, vector<unsigned int> value, int nElement) const
{
    switch(nElement)
    {
        case 1:
            glUniform1uiv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 2:
            glUniform2uiv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 3:
            glUniform3uiv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 4:
            glUniform4uiv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
    }
}
void Shader::setFloatVec(const string &name, vector<float> value, int nElement) const
{
    switch(nElement)
    {
        case 1:
            glUniform1fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 2:
            glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 3:
            glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
        case 4:
            glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
            break;
    }
}
void Shader::setMatrix4(const string &name, vector<vector<float>> value) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMatrix4(const string &name, float* value) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value);
}