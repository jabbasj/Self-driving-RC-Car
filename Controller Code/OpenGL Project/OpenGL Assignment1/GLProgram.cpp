#include "all_headers.h"


GLProgram::GLProgram(void)
{
	m_ID = glCreateProgram();
}

GLProgram::~GLProgram(void)
{
	glDeleteProgram(m_ID);
}

void GLProgram::AttachShader(GLShader* shader)
{
	glAttachShader(m_ID, shader->GetID());
}

void GLProgram::DetachShader(GLShader* shader)
{
	glDetachShader(m_ID, shader->GetID());
}

bool GLProgram::Link()
{
	glLinkProgram(m_ID);

	int param;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &param);
	if (param == GL_TRUE) return true;
	else return false;
}


void GLProgram::SetAttrib(GLint loc, float* val, int varDim)
{
	if (varDim == 4)
		glVertexAttrib4fv(loc, val);
	else if (varDim == 3)
		glVertexAttrib3fv(loc, val);
	else if (varDim == 2)
		glVertexAttrib2fv(loc, val);
	else if (varDim == 1)
		glVertexAttrib1fv(loc, val);
}

GLint GLProgram::EnableVertexAttribArray(char* name)
{
	GLint loc = glGetAttribLocation(m_ID, name);
	if (loc != -1)
		glEnableVertexAttribArray(loc);

	return loc;
}

void GLProgram::DisableVertexAttribArray(GLint loc)
{
	glDisableVertexAttribArray(loc);
}

void GLProgram::BindAttribLocation(unsigned int index, char* name)
{
	glBindAttribLocation(m_ID, index, name);
	//error handling?
	unsigned int err = glGetError();
	DWORD werr = GetLastError();
}