#include "all_headers.h"

int Mesh::unique_ID = 0;

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<GLuint> _indices, std::vector<Texture> _textures, Model* _parent, bool _centered)
{
	mID = unique_ID;
	++unique_ID;

	parent = _parent;
	centered = _centered;

	vertices = _vertices;
	indices = _indices;
	textures = _textures;

	ComputeCentroid();
	SetupMesh();

	ready = true;
}

Mesh::Mesh(Mesh * copy_this, Model* par) {
	mID = unique_ID;
	unique_ID++;

	parent = par;
	vertices = copy_this->vertices;
	textures = copy_this->textures;
	indices = copy_this->indices;
	centered = copy_this->centered;

	ComputeCentroid();
	//SetupMesh(); will be setup later on first draw attempt
}

void Mesh::AddVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv) {
	Vertex temp;

	temp.Position = pos;
	temp.Normal = norm;
	temp.TexCoords = uv;

	vertices.push_back(temp);
}

void Mesh::Draw(GLFWwindow * win) {

	if (ready) {

		for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// Draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else {
		SetupMesh();
		ready = true;
		parent->ready = true;
	}

}

void Mesh::ComputeCentroid() {

	// Find Center
	for (int k = 0; k < vertices.size(); k++) {
		if (vertices[k].Position.x > max.x) max.x = vertices[k].Position.x;
		if (vertices[k].Position.x < min.x) min.x = vertices[k].Position.x;

		if (vertices[k].Position.y > max.y) max.y = vertices[k].Position.y;
		if (vertices[k].Position.y < min.x) min.y = vertices[k].Position.y;

		if (vertices[k].Position.z > max.z) max.z = vertices[k].Position.z;
		if (vertices[k].Position.z < min.z) min.z = vertices[k].Position.z;
	}
	centroid = 0.5f * (max + min);

	radius = sqrt(pow(max.x, 2) + pow(max.y, 2) + pow(max.z, 2));

	centroid.w = 1.0f;
	max.w = 1.0f;
	min.w = 1.0f;
}


void Mesh::SetupMesh() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO[0]);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),&vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, Normal));

	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, TexCoords));
}