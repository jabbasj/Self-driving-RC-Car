#include "all_headers.h"

using namespace glm;

int Terrain::MAX_X_POS = 0;
int Terrain::MAX_Z_POS = 0;

int Terrain::HEIGHT_SCALAR = 5;
int Terrain::X_SCALAR = 10;
int Terrain::Z_SCALAR = 10;


void Terrain::InsertPoint(Vertex v) {
	_vec2 temp;
	temp.x = v.Position.x;
	temp.z = v.Position.z;

	DepthMap.insert(std::pair<_vec2, Vertex>(temp, v));
}

void Terrain::Draw(GLFWwindow * win) {

	glDisable(GL_CULL_FACE);
	
	glBindTexture(GL_TEXTURE_2D, terrain_texture);
	glActiveTexture(GL_TEXTURE0);

	// Draw the triangles
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glEnable(GL_CULL_FACE);
}
 
//Source: https://www.opengl.org/discussion_boards/showthread.php/185140-Loading-bitmap-with-SOIL-then-convert-to-int
//Assumes NxN picture
void Terrain::GenerateDepthMap() {

	int width, height;

	unsigned char* pixels_ = SOIL_load_image("./models/city.bmp", &width, &height, 0, SOIL_LOAD_RGB);

	MAX_X_POS = width;
	MAX_Z_POS = height;

	for (int i = 0; i <= width; i++)
	{
		for (int j = 0; j <= height; j++)
		{
			
			unsigned char r = pixels_[(i + j * width) * 3 + 0];
			unsigned char g = pixels_[(i + j * width) * 3 + 1];
			unsigned char b = pixels_[(i + j * width) * 3 + 2];

			float _r = (float)r / 255;
			float _g = (float)g / 255;
			float _b = (float)b / 255;

			Vertex v;
			v.Position.x = float(i * X_SCALAR + X_TRANSLATE);
			v.Position.y = float(HEIGHT_SCALAR*(_r + _g + _b));
			v.Position.z = float(j * Z_SCALAR + Z_TRANSLATE);
			InsertPoint(v);
		}
	}

	SOIL_free_image_data(pixels_);

	GenerateStreetMap();
}


void Terrain::GenerateStreetMap() {

	int width, height;

	unsigned char* pixels_ = SOIL_load_image("./models/city_streets.bmp", &width, &height, 0, SOIL_LOAD_RGB);

	for (int i = 0; i <= width; i++)
	{
		for (int j = 0; j <= height; j++)
		{

			unsigned char r = pixels_[(i + j * width) * 3 + 0];
			unsigned char g = pixels_[(i + j * width) * 3 + 1];
			unsigned char b = pixels_[(i + j * width) * 3 + 2];

			float _r = (float)r / 255;
			float _g = (float)g / 255;
			float _b = (float)b / 255;


			//STREET MAP TEST
			_vec2 temp;
			temp.x = float(i * X_SCALAR + X_TRANSLATE);
			temp.z = float(j * Z_SCALAR + Z_TRANSLATE);

			int street_id = int((_r + _g + _b) * 10 / 3);
			/*black (ideal): 0, .... ,white (edge, do not drive): 10*/
			StreetMap.insert(std::pair<_vec2, int>(temp, street_id));
		}
	}

	SOIL_free_image_data(pixels_);

}


void Terrain::LoadVertices() {
	int x = 0;
	int z = 0;

	float u = 0.0f;
	float v = 0.0f;

	for (int j = 0; j < MAX_Z_POS; j++) {

		x = 0;
		for (int i = 0; i < MAX_X_POS; i++) {

			_vec2 pos1;
			Vertex * v1;
			pos1.x = float(x * X_SCALAR + X_TRANSLATE);
			pos1.z = float(z * Z_SCALAR + Z_TRANSLATE);

			v1 = &(DepthMap.find(pos1)->second);
			glm::vec3 p1 = v1->Position;

			_vec2 pos2;
			Vertex * v2;
			pos2.x = float(x * X_SCALAR + X_TRANSLATE);
			pos2.z = float((z + 1) * Z_SCALAR + Z_TRANSLATE);

			v2 = &(DepthMap.find(pos2)->second);
			glm::vec3 p2 = v2->Position;


			_vec2 pos3;
			Vertex * v3;
			pos3.x = float((x + 1) * X_SCALAR + X_TRANSLATE);
			pos3.z = float(z * Z_SCALAR + Z_TRANSLATE);

			v3 = &(DepthMap.find(pos3)->second);
			glm::vec3 p3 = v3->Position;

			// Normals
			glm::vec3 u = p1 - p2;
			glm::vec3 v = p1 - p3;
			glm::vec3 normal = glm::cross(u, v);
			normal = glm::normalize(normal);

			v1->Normal = normal;
			v1->TexCoords = glm::vec2(p1.x / MAX_X_POS, p1.z / MAX_Z_POS);
			vertices.push_back(*v1);

			v2->Normal = normal;
			v2->TexCoords = glm::vec2(p2.x / MAX_X_POS, p2.z / MAX_Z_POS);
			vertices.push_back(*v2);

			v3->Normal = normal;
			v3->TexCoords = glm::vec2(p3.x / MAX_X_POS, p3.z / MAX_Z_POS);
			vertices.push_back(*v3);


			_vec2 pos4;
			Vertex * v4;
			pos4.x = float((x + 1) * X_SCALAR + X_TRANSLATE);
			pos4.z = float((z + 1) * Z_SCALAR + Z_TRANSLATE);

			v4 = &(DepthMap.find(pos4)->second);
			glm::vec3 p4 = v4->Position;

			// Normals
			u = p2 - p4;
			v = p2 - p3;
			normal = glm::cross(u, v);
			normal = glm::normalize(normal);

			vertices.push_back(*v3);
			vertices.push_back(*v2);

			v4->Normal = normal;
			v4->TexCoords = glm::vec2(p4.x / MAX_X_POS, p4.z / MAX_Z_POS);
			vertices.push_back(*v4);

			/*
			float angle = glm::dot(normal, glm::vec3(1, 0, 0));
			if (angle < 0.10) {
				SpawnMap.insert(std::pair<_vec2, bool>(pos4, false));
			}*/

			x++;
		}

		z++;
	}
}




void Terrain::SetupTerrain() {

	srand(size_t(time(NULL)));

	//RequestUserInput();
	GenerateDepthMap();
	LoadVertices();

	_vec2 newTile;
	newTile.x = 0.0f;
	newTile.z = 0.0f;
	m_Tiles.push_back(newTile);

	m_TextureNum = rand() % NUM_OF_TEXTURES + 1;

	glGenTextures(1, &terrain_texture);

	std::string tex_path = "./models/terrain" + std::to_string(m_TextureNum) + ".jpg";
	int width, height;

	unsigned char* image = SOIL_load_image(tex_path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, terrain_texture);
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


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
}