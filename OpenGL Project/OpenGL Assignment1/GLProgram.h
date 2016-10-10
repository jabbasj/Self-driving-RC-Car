#include "all_headers.h"

class GLProgram
{
public:
	GLProgram(void);
	~GLProgram(void);
	void	AttachShader(GLShader* shader);
	void	DetachShader(GLShader* shader);
	bool	Link();

	void			Use() { glUseProgram(m_ID); }
	GLuint			GetID(){ return m_ID; }

	GLint	GetAttribLocation(char* name) { return glGetAttribLocation(m_ID, name); }
	void	SetAttrib(GLint loc, float val) { glVertexAttrib1f(loc, val); }
	void	SetAttrib(GLint loc, float* val, int varDim);

	void	BindAttribLocation(unsigned int index, char* name);

	GLint	EnableVertexAttribArray(char* name);
	void	DisableVertexAttribArray(GLint loc);

protected:
	GLuint m_ID;
};
