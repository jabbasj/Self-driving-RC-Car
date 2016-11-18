#include "all_headers.h"

int Model::unique_ID = 0;

Model::Model() {
	name = "null";
}

Model::Model(GLchar* _path, std::string _name, bool _centered)	{
	name = _name;
	centered = _centered;
	path = _path;
	scale = glm::vec3(1.0f);
	Mesh::unique_ID = 0;
	mID = unique_ID;
	++unique_ID;

	LoadModel(_path);

	ready = true;
}

Model::Model(Model * copy_this) {
	mID = unique_ID;
	unique_ID++;

	name = copy_this->name;
	centered = copy_this->centered;
	directory = copy_this->directory;
	scale = copy_this->scale;
	path = copy_this->path;

	for (size_t i = 0; i < copy_this->meshes.size(); i++) {
		meshes.push_back(Mesh(&copy_this->meshes[i], this));
	}

	ready = true;
}

void Model::Draw(GLFWwindow * win)
{
	for (GLuint i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(win);
	}
}

/*
void Model::Update(GLFWwindow * win)
{
	if (name == "spinner") {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i].Spin(win);
		}
	}

	if (name == "revolver") {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i].Revolve(win);
		}
	}
}*/

void Model::AbsoluteTranslate(glm::vec3 change) {

	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].ModelMat[3][0] += change.x;
		meshes[i].ModelMat[3][1] += change.y;
		meshes[i].ModelMat[3][2] += change.z;
	}
}

void Model::RelativeTranslate(glm::vec3 change) {

	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].ModelMat = glm::translate(meshes[i].ModelMat, change);
	}
}

void Model::Scale(glm::vec3 change) {

	glm::mat4 _scale = glm::scale(glm::mat4(), change);

	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].ModelMat = meshes[i].ModelMat * _scale;
	}
}


void Model::Rotate(float angle, glm::vec3 vector) {

	glm::mat4 rot = glm::rotate(glm::mat4(), angle, vector);

	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].ModelMat = meshes[i].ModelMat * rot;
	}
}

void Model::RotateSelf(float angle) {	

	for (size_t i = 0; i < meshes.size(); i++) {

		glm::vec3 mesh_pos(meshes[i].ModelMat * meshes[i].centroid);
		mesh_pos.x = 0; mesh_pos.z = 0;

		glm::mat4 rot = glm::rotate(glm::mat4(), angle, mesh_pos);

		meshes[i].ModelMat = meshes[i].ModelMat * rot;
	}
}


void Model::Center() {

	for (size_t i = 0; i < meshes.size(); i++) {

		meshes[i].ModelMat[3].x = 0;
		meshes[i].ModelMat[3].y = 0;
		meshes[i].ModelMat[3].z = 0;

		if (centered) {
			meshes[i].ModelMat = glm::translate(meshes[i].ModelMat, glm::vec3(0, -meshes[i].min.y * scale.y, 0));
		}
	}
}

void Model::Reset() {

	glm::mat4 identity;

	for (size_t i = 0; i < meshes.size(); i++) {

		meshes[i].ModelMat = identity;

	}
}

void Model::Transform(glm::mat4 tm) {
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].ModelMat = meshes[i].ModelMat * tm;
	}
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error loading " << path << ":" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}


// Source: http://www.learnopengl.com/#!Model-Loading/Model
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


// Source: http://www.learnopengl.com/#!Model-Loading/Model
// Materials part not used, just left in here for future
Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		if (&mesh->mNormals[0]) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, this, centered);
}

// Source: http://www.learnopengl.com/#!Model-Loading/Model
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.id = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = str;
		textures.push_back(texture);
	}

	return textures;
}

// Source: http://www.learnopengl.com/#!Model-Loading/Model
GLint Model::TextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}