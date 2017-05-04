#include "all_headers.h"

// Child of Model, holds the vertices, textures, buffers, etc...
// Sets itself up at construction
// Draws itself
class Mesh {
	friend class GLRenderer;
	friend class Model;

public:
	Mesh(std::vector<Vertex> _vertices, std::vector<GLuint> _indices, std::vector<Texture> _textures, Model* _parent, bool _centered = false);
	Mesh(Mesh * copy_this, Model* par);

protected:

	int mID;
	static int unique_ID;
	Model* parent;
	bool ready = false;

	std::vector<Vertex> vertices;	
	std::vector<Texture> textures;
	std::vector<GLuint> indices;

	void Draw(GLFWwindow * win);


	glm::mat4 ModelMat;					// Model matrix

	bool centered = false;				// Whether mesh's orgin is at its centeroid or not
	glm::vec4 centroid;					// Average center of Mesh
	glm::vec4 max;
	glm::vec4 min;
	GLfloat radius;
	void ComputeCentroid();

	GLfloat vertical_angle = 0.0f;
	GLfloat horizontal_angle = 0.0f;
	GLfloat change_value = 0.001f;
	
private:
	// Buffers
	GLuint VAO, VBO[2], EBO;

	void AddVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv);
	void SetupMesh();
};