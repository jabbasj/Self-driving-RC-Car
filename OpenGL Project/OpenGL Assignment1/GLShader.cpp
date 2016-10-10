#include "all_headers.h"

GLShader::GLShader(GLenum shaderType)
{
	m_Type = shaderType;
	m_ID = glCreateShader(shaderType);
	m_code = NULL;
}

GLShader::~GLShader(void)
{
	if (m_code) delete[] m_code;
	glDeleteShader(m_ID);
}

bool GLShader::Load(char * fileName)
{
	ShaderName = fileName;
	int err = FileRead(fileName);
	if (m_code) return true;
	else return false;
}

bool GLShader::Compile(void)
{
	if (m_code == NULL) return false;
	const GLchar * code = reinterpret_cast<const char*>(m_code);

	glShaderSource(m_ID, 1, &code, NULL);
	glCompileShader(m_ID);

	int param;
	GLchar infoLog[512];
	glGetShaderiv(m_ID, GL_COMPILE_STATUS, &param);
	if (param == GL_TRUE) return true;
	else
	{
		glGetShaderInfoLog(m_ID, 512, NULL, infoLog);
		std::cout << "Shader (" << ShaderName << ") Compilation Failed:\n" << infoLog << std::endl;
		return false;
	}
}


int GLShader::FileRead(char* fileName)
{
	std::ifstream file;
	file.open(fileName, std::ios::in); // opens as ASCII!
	if (!file) return -1;

	len = getFileLength(file);

	if (len == 0) return -2;   // Error: Empty File 

	m_code = (GLubyte*) new char[len + 1];
	if (*m_code == 0) return -3;   // can't reserve memory

	// len isn't always strlen cause some characters are stripped in ascii read...
	// 0-terminate at max pos
	m_code[len] = 0;

	unsigned int i = 0;
	while (file.good())
	{
		m_code[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	m_code[i] = 0;  // 0-terminate it at the correct position

	file.close();

	return 0; // No Error
}


unsigned long GLShader::getFileLength(std::ifstream& file)
{
	if (!file.good()) return 0;

	unsigned long pos = file.tellg();
	file.seekg(0, std::ios::end);
	unsigned long len = file.tellg();
	file.seekg(std::ios::beg);

	return len;
}