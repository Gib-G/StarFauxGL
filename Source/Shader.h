#pragma once
#include "Types.h"

class CShader
{
	private:
		GLuint m_programID;

	public:

		CShader();
		~CShader();

		// use the program
		bool	Load(const string& vertexPath, const string& fragmentPath);
		GLint	GetUniformLocation(const string& name) const;
		void	Use() const;

		void SetUniform(const string& name, const glm::mat4& mat) const;
		void SetUniform(const string& name, const glm::mat3& mat) const;
		void SetUniform(const string& name, const glm::vec4& vec) const;
		void SetUniform(const string& name, const glm::vec3& vec) const;
		void SetUniform(const string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
		void SetUniform(const string& name, GLfloat x, GLfloat y, GLfloat z) const;
		void SetUniform(const string& name, GLfloat x) const;
		void SetUniform(const string& name, int x) const;
};