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
			v.Position.x = i * X_SCALAR + X_TRANSLATE;
			v.Position.y = HEIGHT_SCALAR*(_r + _g + _b);
			v.Position.z = j * Z_SCALAR + Z_TRANSLATE;
			InsertPoint(v);

			//STREET MAP TEST
			_vec2 temp;
			temp.x = v.Position.x;
			temp.z = v.Position.z;
			int identifier = 0; /*street*/

			if (v.Position.y > 0.0f) {
				identifier = 1; /*not street*/
			}

			StreetMap.insert(std::pair<_vec2, int>(temp, identifier));
		}
	}

	SOIL_free_image_data(pixels_);

	//ModifyTerrain();
}


void Terrain::AdjustDepthMapEdge() {

	// adjust bottom edge and top edge
	for (int i = 0; i <= MAX_Z_POS; ++i) {
		_vec2 bottomPoint;
		bottomPoint.x = 0 * X_SCALAR + X_TRANSLATE;
		bottomPoint.z = i * Z_SCALAR + Z_TRANSLATE;

		_vec2 topPoint;
		topPoint.x = MAX_X_POS * X_SCALAR + X_TRANSLATE;
		topPoint.z = i * Z_SCALAR + Z_TRANSLATE;

		if (DepthMap.count(bottomPoint) > 0){
			if (DepthMap.count(topPoint) > 0) {
				DepthMap.find(topPoint)->second.Position.y = DepthMap.find(bottomPoint)->second.Position.y;
			}
		}
	}

	// left and right edge
	for (int i = 1; i < MAX_X_POS; ++i) {
		_vec2 leftPoint;
		leftPoint.x = i * X_SCALAR + X_TRANSLATE;
		leftPoint.z = 0 * Z_SCALAR + Z_TRANSLATE;

		_vec2 rightPoint;
		rightPoint.x = i * X_SCALAR + X_TRANSLATE;
		rightPoint.z = MAX_Z_POS * Z_SCALAR + Z_TRANSLATE;

		if (DepthMap.count(leftPoint) > 0){
			if (DepthMap.count(rightPoint) > 0) {
				DepthMap.find(rightPoint)->second.Position.y = DepthMap.find(leftPoint)->second.Position.y;
			}
		}
	}

	// adjust 4 corners of the map
	_vec2 bottomLeftCorner;
	bottomLeftCorner.x = 0 * X_SCALAR + X_TRANSLATE;
	bottomLeftCorner.z = 0 * Z_SCALAR + Z_TRANSLATE;

	_vec2 bottomRightCorner;
	bottomRightCorner.x = 0 * X_SCALAR + X_TRANSLATE;
	bottomRightCorner.z = MAX_Z_POS * Z_SCALAR + Z_TRANSLATE;

	_vec2 topLeftCorner;
	topLeftCorner.x = MAX_X_POS  * X_SCALAR + X_TRANSLATE;
	topLeftCorner.z = 0 * Z_SCALAR + Z_TRANSLATE;


	_vec2 topRightCorner;
	topRightCorner.x = MAX_X_POS * X_SCALAR + X_TRANSLATE;
	topRightCorner.z = MAX_Z_POS * Z_SCALAR + Z_TRANSLATE;

	float newHeight = 0.0f;

	if (DepthMap.count(bottomLeftCorner) > 0) {
		newHeight = DepthMap.find(bottomLeftCorner)->second.Position.y;
	}

	if (DepthMap.count(bottomRightCorner) > 0) {
		DepthMap.find(bottomRightCorner)->second.Position.y = newHeight;
	}

	if (DepthMap.count(topLeftCorner) > 0) {
		DepthMap.find(topLeftCorner)->second.Position.y = newHeight;
	}

	if (DepthMap.count(topRightCorner) > 0) {
		DepthMap.find(topRightCorner)->second.Position.y = newHeight;
	}
}

void Terrain::ModifyTerrain() {

	//TODO: User input for number of "disturbance" lines used
	for (int i = 0; i < NUM_OF_LINES; i++) {
		_vec2 start;
		
		bool spawnSpikes = (rand() % 2) == 1 ? true : false;
		
		float lineAngle = (rand() % 3) * 90.0f;
		if (spawnSpikes) {
			lineAngle = (rand() % 9) * 40.0f;
		}

		float height = rand() % 50 + 1;
		int width = rand() % 100 + 1;
		float depth = rand() % 500 + 100;

		if (spawnSpikes) {
			depth = rand() % 200 + 100;
		}

		auto pt = DepthMap.begin();
		std::advance(pt, rand() % DepthMap.size());
		start = pt->first;

		float slopeAngle = (float)(rand() % 60 + 30);
		float tangent = glm::tan(glm::radians(slopeAngle));
		int offSet = (int)(depth / X_SCALAR / tangent);

		int newHeight = height + 2 * offSet;
		int newWidth = width + 2 * offSet;

		vec2 rectangleCenter = vec2(newWidth * 0.5f, newHeight * 0.5f);

		float diagonalAngle = glm::atan((float)newHeight / newWidth);
		diagonalAngle = glm::degrees(diagonalAngle);

		for (int i = 0; i < newHeight; ++i) {
			for (int j = 0; j < newWidth; ++j) {

				float vertexDepth = depth;
				if (j < offSet || j >(offSet + width) || i < offSet || i > (offSet + height)) {
					// smoothing
					float xDisToFlatEdge = 0.0f;
					float zDisToFlatEdge = 0.0f;

					if (i < offSet) {
						xDisToFlatEdge = i;
					}
					else if (i > (offSet + height)) {
						xDisToFlatEdge = newHeight - i;
					}
					if (j < offSet) {
						zDisToFlatEdge = j;
					}
					else if (j > (offSet + width)) {
						zDisToFlatEdge = newWidth - j;
					}

					float averageDisToFlatEdge = 0.5 * (xDisToFlatEdge + zDisToFlatEdge);
					float vertexDepthInX = depth * xDisToFlatEdge / offSet;
					float vertexDepthInZ = depth * zDisToFlatEdge / offSet;

					vec2 centerToPoint = vec2((float)j, (float)i) - rectangleCenter;
					float angleOfCenterToPoint = glm::angle<float>(vec2(1.0f, 0.0f), normalize(centerToPoint));
					angleOfCenterToPoint = glm::degrees(angleOfCenterToPoint);
					
					if (angleOfCenterToPoint <= diagonalAngle || angleOfCenterToPoint >= (180 - diagonalAngle)) {
						vertexDepth = vertexDepthInZ;
					}
					else {
						vertexDepth = vertexDepthInX;
					}
				}

				glm::vec2 originToPoint = glm::vec2(j, i);
				vec2 newOriginToPoint = glm::rotate<float>(originToPoint, glm::radians(lineAngle));

				_vec2 newPointOffSet;
				newPointOffSet.x =  (int)glm::ceil(newOriginToPoint.x) * X_SCALAR;
				newPointOffSet.z = (int)glm::ceil(newOriginToPoint.y) * Z_SCALAR;

				_vec2 newPoint;
				newPoint.x = start.x + newPointOffSet.x;
				newPoint.z = start.z + newPointOffSet.z;

				if (DepthMap.count(newPoint) > 0){
					DepthMap.find(newPoint)->second.Position.y += vertexDepth;
				}

				// Don't spawn near spikes
				if (SpawnMap.count(newPoint) > 0){
					SpawnMap.find(newPoint)->second = true;
				}
			}
		}
	}

	AdjustDepthMapEdge();
}

void Terrain::LoadVertices() {
	int x = 0;
	int z = 0;

	float u = 0.0f;
	float v = 0.0f;

	for (int j = 0; j < MAX_Z_POS; j++) {

		x = 0;
		for (int i = 0; i < MAX_X_POS; i ++) {

			_vec2 pos1;
			Vertex * v1;
			pos1.x = x * X_SCALAR + X_TRANSLATE;
			pos1.z = z * Z_SCALAR + Z_TRANSLATE;

			v1 = &(DepthMap.find(pos1)->second);
			glm::vec3 p1 = v1->Position;

			_vec2 pos2;
			Vertex * v2;
			pos2.x = x * X_SCALAR + X_TRANSLATE;
			pos2.z = (z + 1) * Z_SCALAR + Z_TRANSLATE;

			v2 = &(DepthMap.find(pos2)->second);
			glm::vec3 p2 = v2->Position;


			_vec2 pos3;
			Vertex * v3;
			pos3.x = (x + 1) * X_SCALAR + X_TRANSLATE;
			pos3.z = z * Z_SCALAR + Z_TRANSLATE;

			v3 = &(DepthMap.find(pos3)->second);
			glm::vec3 p3 = v3->Position;

			_vec2 pos4;
			Vertex * v4;
			pos4.x = (x + 1) * X_SCALAR + X_TRANSLATE;
			pos4.z = (z + 1) * Z_SCALAR + Z_TRANSLATE;

			v4 = &(DepthMap.find(pos4)->second);
			glm::vec3 p4 = v4->Position;

			float yDistP1P2 = glm::abs(p2.y - p1.y);
			float yDistP4P2 = glm::abs(p2.y - p4.y);

			float yDistP1P3 = glm::abs(p3.y - p1.y);
			float yDistP4P3 = glm::abs(p3.y - p4.y);

			float maxHorizontalYDist = glm::max(yDistP1P3, yDistP4P2);
			float maxVerticalYDist = glm::max(yDistP1P2, yDistP4P3);

			if (maxHorizontalYDist <= HEIGHT_SCALAR && maxVerticalYDist <= HEIGHT_SCALAR) {
				// Normals for v1, v2, v3
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

				// Normals for v3, v2, v4
				u = p2 - p4;
				v = p2 - p3;
				normal = glm::cross(u, v);
				normal = glm::normalize(normal);

				vertices.push_back(*v3);
				vertices.push_back(*v2);

				v4->Normal = normal;
				v4->TexCoords = glm::vec2(p4.x / MAX_X_POS, p4.z / MAX_Z_POS);
				vertices.push_back(*v4);

				float angle = glm::dot(normal, glm::vec3(1, 0, 0));
				if (angle < 0.10) {
					SpawnMap.insert(std::pair<_vec2, bool>(pos4, false));
				}
			}
			else {
				int numOfHorizontalTriangle = glm::ceil(maxHorizontalYDist / HEIGHT_SCALAR);
				numOfHorizontalTriangle = glm::max(numOfHorizontalTriangle, 1);

				int numOfVerticalTriangle = glm::ceil(maxVerticalYDist / HEIGHT_SCALAR);
				numOfVerticalTriangle = glm::max(numOfVerticalTriangle, 1);

				std::vector<vec3> leftPos;
				std::vector<vec3> topPos;
				std::vector<vec3> rightPos;
				std::vector<vec3> bottomPos;
				for (int i = 0; i <= numOfVerticalTriangle; ++i) {
					vec3 newLeftPos = p1 + (p2 - p1) * ((float)i / numOfVerticalTriangle);
					leftPos.push_back(newLeftPos);

					vec3 newRightPos = p3 + (p4 - p3) * ((float)i / numOfVerticalTriangle);
					rightPos.push_back(newRightPos);
				}

				for (int i = 0; i <= numOfHorizontalTriangle; ++i) {
					vec3 newTopPos = p1 + (p3 - p1) * ((float)i / numOfHorizontalTriangle);
					topPos.push_back(newTopPos);

					vec3 newBottomPos = p2 + (p4 - p2) * ((float)i / numOfHorizontalTriangle);
					bottomPos.push_back(newBottomPos);
				}

				glm::vec3 normal = vec3(0.0f, 1.0f, 0.0f);

				for (int i = 0; i < numOfVerticalTriangle; ++i) {
					for (int j = 0; j < numOfHorizontalTriangle; ++j) {
						vec3 newPoint1 = leftPos[i] + (rightPos[i] - leftPos[i]) * ((float)j / numOfHorizontalTriangle);
						vec3 newPoint2 = leftPos[i + 1] + (rightPos[i + 1] - leftPos[i + 1]) * ((float)j / numOfHorizontalTriangle);
						vec3 newPoint3 = topPos[j + 1] + (bottomPos[j + 1] - topPos[j + 1]) * ((float)i / numOfVerticalTriangle);
						vec3 newPoint4 = topPos[j + 1] + (bottomPos[j + 1] - topPos[j + 1]) * ((float)(i + 1) / numOfVerticalTriangle);
						
						Vertex newV1;
						Vertex newV2;
						Vertex newV3;
						Vertex newV4;

						newV1.Position = newPoint1;						
						newV2.Position = newPoint2;						
						newV3.Position = newPoint3;						
						newV4.Position = newPoint4;

						// normal for v1, v2, v3
						glm::vec3 u = newPoint1 - newPoint2;
						glm::vec3 v = newPoint1 - newPoint3;
						normal = glm::cross(u, v);
						normal = glm::normalize(normal);

						newV1.Normal = normal;
						newV1.TexCoords = glm::vec2(p1.x / MAX_X_POS, p1.z / MAX_Z_POS);

						newV2.Normal = normal;
						newV2.TexCoords = glm::vec2(p2.x / MAX_X_POS, p2.z / MAX_Z_POS);

						newV3.Normal = normal;
						newV3.TexCoords = glm::vec2(p3.x / MAX_X_POS, p3.z / MAX_Z_POS);

						// Normals for v3, v2, v4
						u = newPoint2 - newPoint4;
						v = newPoint2 - newPoint3;
						normal = glm::cross(u, v);
						normal = glm::normalize(normal);

						newV4.Normal = normal;
						newV4.TexCoords = glm::vec2(p4.x / MAX_X_POS, p4.z / MAX_Z_POS);

						vertices.push_back(newV1);
						vertices.push_back(newV2);
						vertices.push_back(newV3);

						vertices.push_back(newV3);
						vertices.push_back(newV2);
						vertices.push_back(newV4);

						_vec2 newpos1;
						_vec2 newpos2;
						_vec2 newpos3;
						_vec2 newpos4;

						newpos1.x = newV1.Position.x;
						newpos1.z = newV1.Position.z;

						newpos2.x = newV2.Position.x;
						newpos2.z = newV2.Position.z;

						newpos3.x = newV3.Position.x;
						newpos3.z = newV3.Position.z;

						newpos4.x = newV4.Position.x;
						newpos4.z = newV4.Position.z;

						DepthMap.insert(std::pair<_vec2, Vertex>(newpos1, newV1));
						DepthMap.insert(std::pair<_vec2, Vertex>(newpos2, newV2));
						DepthMap.insert(std::pair<_vec2, Vertex>(newpos3, newV3));
						DepthMap.insert(std::pair<_vec2, Vertex>(newpos4, newV4));

						float angle = glm::dot(normal, glm::vec3(1, 0, 0));
						if (angle < 0.10) {
							SpawnMap.insert(std::pair<_vec2, bool>(pos4, false));
						}

					}
				}
			}					
			x++;
		}

		z++;
	}
}

bool Terrain::CheckNothingNearby(_vec2 pos) {

	bool nothing_nearby = true;

	for (int i = -5; i < 5; i++) {
		for (int j = -5; j < 5; j++) {			

			_vec2 test;
			test.x = pos.x + i * X_SCALAR;
			test.z = pos.z + j * Z_SCALAR;
			
			if (SpawnMap.count(test) > 0) {
				if (SpawnMap.find(test)->second == true) {
					nothing_nearby = false;
					break;
				}
			}
		}
	}

	return nothing_nearby;
}

void Terrain::ExpandTerrainBasedOnCamPos(vec3 position) {

	if (!GLRenderer::FINISHED_SPAWNING) { return; }

	GLRenderer::FINISHED_SPAWNING = false;

	float terrainLengthInX = MAX_X_POS * X_SCALAR;
	float terrainLengthInZ = MAX_Z_POS * Z_SCALAR;
	int xInTiles = glm::floor((position.x - X_TRANSLATE) / terrainLengthInX);
	int zInTiles = glm::floor((position.z - Z_TRANSLATE) / terrainLengthInZ);

	float tileMinX = X_TRANSLATE + xInTiles * terrainLengthInX;
	float tileMaxX = tileMinX + terrainLengthInX;
	float tileMinZ = Z_TRANSLATE + zInTiles * terrainLengthInZ;
	float tileMaxZ = tileMinZ + terrainLengthInZ;

	_vec2 newTile;
	if (position.x < (tileMinX + m_TerrainEdges.x)) {
		// add terrain in (xInTiles - 1, zInTiles)
		newTile.x = xInTiles - 1;
		newTile.z = zInTiles;
		ExpandTerrain(newTile);
		if (position.z < (tileMinZ + m_TerrainEdges.z)) {
			// add terrain in (xInTiles - 1, zInTiles - 1)
			newTile.x = xInTiles - 1;
			newTile.z = zInTiles - 1;
			ExpandTerrain(newTile);
		}
		else if (position.z >(tileMaxZ - m_TerrainEdges.z)) {
			// add terrain in (xInTiles - 1, zInTiles + 1 )
			newTile.x = xInTiles - 1;
			newTile.z = zInTiles + 1;
			ExpandTerrain(newTile);
		}
	}

	if (position.x > (tileMaxX - m_TerrainEdges.x)) {
		// add terrain in (xInTiles + 1, zInTiles)
		newTile.x = xInTiles + 1;
		newTile.z = zInTiles;
		ExpandTerrain(newTile);
		if (position.z < (tileMinZ + m_TerrainEdges.z)) {
			// add terrain in (xInTiles + 1, zInTiles - 1)
			newTile.x = xInTiles + 1;
			newTile.z = zInTiles - 1;
			ExpandTerrain(newTile);
		}
		else if (position.z >(tileMaxZ - m_TerrainEdges.z)) {
			// add terrain in (xInTiles + 1, zInTiles + 1 )
			newTile.x = xInTiles + 1;
			newTile.z = zInTiles + 1;
			ExpandTerrain(newTile);
		}
	}

	if (position.z < (tileMinZ + m_TerrainEdges.z)) {
		// add terrain in (xInTiles, zInTiles -1 )
		newTile.x = xInTiles;
		newTile.z = zInTiles - 1;
		ExpandTerrain(newTile);
	}

	if (position.z > (tileMaxZ - m_TerrainEdges.z)) {
		// add terrain in (xInTiles, zInTiles + 1 )
		newTile.x = xInTiles;
		newTile.z = zInTiles + 1;
		ExpandTerrain(newTile);
	}

	GLRenderer::FINISHED_SPAWNING = true;
}

void Terrain::SetupTerrain() {

	srand(time(NULL));

	//RequestUserInput();
	GenerateDepthMap();
	LoadVertices();

	SetupTerrainEdges();

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

void Terrain::SetupTerrainEdges() {
	m_TerrainEdges.x = MAX_X_POS * X_SCALAR * 0.3f;
	m_TerrainEdges.z = MAX_Z_POS * Z_SCALAR * 0.3f;
}

void Terrain::ExpandTerrain(_vec2 newTile) {

	for (int i = 0; i < m_Tiles.size(); ++i) {
		if (newTile.x == m_Tiles[i].x && newTile.z == m_Tiles[i].z) {
			return;
		}
	}

 	m_Tiles.push_back(newTile);

	float terrainLengthInX = MAX_X_POS * X_SCALAR;
	float terrainLengthInZ = MAX_Z_POS * Z_SCALAR;

	int x = 0;
	int z = 0;

	float u = 0.0f;
	float v = 0.0f;

	for (int j = 0; j < MAX_Z_POS; j++) {

		x = 0;
		for (int i = 0; i < MAX_X_POS; i++) {

			_vec2 pos1;
			Vertex v1;
			pos1.x = x * X_SCALAR + X_TRANSLATE;
			pos1.z = z * Z_SCALAR + Z_TRANSLATE;

			v1 = DepthMap.find(pos1)->second;
			v1.Position.x += newTile.x * terrainLengthInX;
			v1.Position.z += newTile.z * terrainLengthInZ;
			pos1.x += newTile.x * terrainLengthInX;
			pos1.z += newTile.x * terrainLengthInX;
			glm::vec3 p1 = v1.Position;

			_vec2 pos2;
			Vertex v2;
			pos2.x = x * X_SCALAR + X_TRANSLATE;
			pos2.z = (z + 1) * Z_SCALAR + Z_TRANSLATE;

			v2 = DepthMap.find(pos2)->second;
			v2.Position.x += newTile.x * terrainLengthInX;
			v2.Position.z += newTile.z * terrainLengthInZ;
			pos2.x += newTile.x * terrainLengthInX;
			pos2.z += newTile.z * terrainLengthInZ;
			glm::vec3 p2 = v2.Position;

			_vec2 pos3;
			Vertex v3;
			pos3.x = (x + 1) * X_SCALAR + X_TRANSLATE;
			pos3.z = z * Z_SCALAR + Z_TRANSLATE;

			v3 = DepthMap.find(pos3)->second;
			v3.Position.x += newTile.x * terrainLengthInX;
			v3.Position.z += newTile.z * terrainLengthInZ;
			pos3.x += newTile.x * terrainLengthInX;
			pos3.z += newTile.z * terrainLengthInZ;
			glm::vec3 p3 = v3.Position;

			_vec2 pos4;
			Vertex v4;
			pos4.x = (x + 1) * X_SCALAR + X_TRANSLATE;
			pos4.z = (z + 1) * Z_SCALAR + Z_TRANSLATE;

			v4 = DepthMap.find(pos4)->second;
			v4.Position.x += newTile.x * terrainLengthInX;
			v4.Position.z += newTile.z * terrainLengthInZ;
			pos4.x += newTile.x * terrainLengthInX;
			pos4.z += newTile.z * terrainLengthInZ;
			glm::vec3 p4 = v4.Position;

			// Normals for v1, v2, v3
			glm::vec3 u = p1 - p2;
			glm::vec3 v = p1 - p3;
			glm::vec3 normal = glm::cross(u, v);
			normal = glm::normalize(normal);

			v1.Normal = normal;
			v1.TexCoords = glm::vec2(p1.x / MAX_X_POS, p1.z / MAX_Z_POS);
			vertices.push_back(v1);

			v2.Normal = normal;
			v2.TexCoords = glm::vec2(p2.x / MAX_X_POS, p2.z / MAX_Z_POS);
			vertices.push_back(v2);

			v3.Normal = normal;
			v3.TexCoords = glm::vec2(p3.x / MAX_X_POS, p3.z / MAX_Z_POS);
			vertices.push_back(v3);

			// Normals for v3, v2, v4
			u = p2 - p4;
			v = p2 - p3;
			normal = glm::cross(u, v);
			normal = glm::normalize(normal);

			vertices.push_back(v3);
			vertices.push_back(v2);

			v4.Normal = normal;
			v4.TexCoords = glm::vec2(p4.x / MAX_X_POS, p4.z / MAX_Z_POS);
			vertices.push_back(v4);

			DepthMap.insert(std::pair<_vec2, Vertex>(pos1, v1));
			DepthMap.insert(std::pair<_vec2, Vertex>(pos2, v2));
			DepthMap.insert(std::pair<_vec2, Vertex>(pos3, v3));
			DepthMap.insert(std::pair<_vec2, Vertex>(pos4, v4));

			_vec2 test;
			test.x = pos3.x;
			test.z = pos3.z;

			float angle = glm::dot(normal, glm::vec3(1, 0, 0));
			if (angle < 0.10) {
				SpawnMap.insert(std::pair<_vec2, bool>(test, false));
			}
			x++;
		}

		z++;
	}

	GLRenderer::PENDING_UPDATE++;
}


void Terrain::RequestUserInput() {

	int num = 0;
	std::string input = "";

	while (num > 50 || num <= 0) {
		std::cout << "Enter the number of terrain disturbance lines: \n";
		try {
			std::cin >> input;
			num = std::stoi(input);

			if (num > 50 || num <= 0) {
				std::cout << "Number must be between 1 and 50!\n";
			}
		}
		catch (...) {
			std::cout << "Wrong input!\n";
			num = 0;
			input = "";
		}
	}

	NUM_OF_LINES = num;
}