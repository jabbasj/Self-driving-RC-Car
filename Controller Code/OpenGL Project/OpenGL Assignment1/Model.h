#include "all_headers.h"

// TODO: Less obvious C&P

class Model
{
	friend class GLRenderer;
	friend class BulletParticle;
	friend class Mesh;

public:
	Model(GLchar* path, std::string _name = "unspecified", bool _centered = false);
	Model(Model* copy_this);
	Model();

protected:
	int mID = -1;
	static int unique_ID;
	std::string name;

	bool centered = false;
	glm::vec3 scale;

	void Draw(GLFWwindow* win);
	
	bool ready = false;

	// Functions for transformations of all meshes
	void RelativeTranslate(glm::vec3 change);
	void AbsoluteTranslate(glm::vec3 change);
	void Rotate(float angle, glm::vec3 vector);
	void RotateSelf(float angle);
	void Scale(glm::vec3 change);
	void Center();
	void Reset();
	void Transform(glm::mat4 tm);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::string path;

	void LoadModel(std::string path);	// Loads model, its textures and creates the meshes
	void SetupModel();

	//Below functions have been taken from source: http://www.learnopengl.com/#!Model-Loading/Model
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	GLint TextureFromFile(const char* path, std::string directory);
};