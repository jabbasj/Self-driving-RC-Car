#include "all_headers.h"

class GLShader
{
public:
	GLShader(GLenum shaderType);
	~GLShader(void);

	bool	Load(char* fileName);
	bool	Compile(void);
	GLuint	GetID(){ return m_ID; }

protected:
	int				FileRead(char* fileName);
	std::string		ShaderName;
	unsigned long	getFileLength(std::ifstream& file);
	unsigned long	len;

protected:
	GLenum	m_Type; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_ID;
	GLubyte*  m_code;
};
