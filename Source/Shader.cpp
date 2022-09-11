#include "Shader.h"
#include "FileUtil.h"

Shader::Shader()
{
	m_programID = (GLuint)-1;
}

bool Shader::Load(const string& vertexPath, const string& fragmentPath)
{
	string vertexCode;
	string fragmentCode;

	if (loadFile(vertexPath,vertexCode) == false)
	{
		ConsoleWriteErr("Failed to load %s",vertexPath.c_str());
		return false;
	}
	if (loadFile(fragmentPath,fragmentCode) == false)
	{
		ConsoleWriteErr("Failed to load %s",fragmentPath.c_str());
		return false;
	}

	const char*	vertexShaderSource   = vertexCode  .c_str();
	const char*	fragmentShaderSource = fragmentCode.c_str();
	GLint		success;
	GLchar		infoLog[512];
	GLuint		vertexShaderID;
	GLuint		fragmentShaderID;

	// Compile vertex shader
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		ConsoleWriteErr("Shader::Shader() : compilation failed for %s\n%s", vertexPath.c_str(), infoLog);
		return false;
	}

	// Compile fragment shader
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		ConsoleWriteErr("Shader::Shader() : compilation failed for %s\n%s", fragmentPath.c_str(), infoLog);
		return false;
	}

	// Link shader program
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShaderID);
	glAttachShader(m_programID, fragmentShaderID);
	glLinkProgram(m_programID);

	// Delete shaders
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return true;
}

Shader::~Shader()
{
	if (m_programID != (GLuint)-1)
	{
		glDeleteProgram(m_programID);
	}
}

void Shader::Use() const
{
	if (m_programID != (GLuint)-1)
	{
		glUseProgram(m_programID);
	}
}

GLint Shader::GetUniformLocation(const string& name) const
{
	GLint res = -1;
	if (m_programID != (GLuint)-1)
	{
		res = glGetUniformLocation(m_programID, name.c_str());
		if (res == -1)
		{
			ConsoleWriteErr("Shader::GetUniformLocation(%s) : No such uniform", name.c_str());
			return -1;
		}
	}
	return res;
}

void Shader::SetUniform(const string& name, const glm::mat4& mat) const
{
	GLint loc = GetUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform(const string& name, const glm::mat3& mat) const
{
	GLint loc = GetUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform(const string& name, const glm::vec3& vec) const
{
	GLint loc = GetUniformLocation(name);
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const string& name, const glm::vec4& vec) const
{
	GLint loc = GetUniformLocation(name);
	glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform(const string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
{
	GLint loc = GetUniformLocation(name);
	glUniform4f(loc, x, y, z, w);
}

void Shader::SetUniform(const string& name, GLfloat x, GLfloat y, GLfloat z) const
{
	GLint loc = GetUniformLocation(name);
	glUniform3f(loc, x, y, z);
}

void Shader::SetUniform(const string& name, GLfloat x) const
{
	GLint loc = GetUniformLocation(name);
	glUniform1f(loc, x);
}

void Shader::SetUniform(const string& name, int x) const
{
	GLint loc = GetUniformLocation(name);
	glUniform1i(loc, x);
}
