#include "all_headers.h"

#define X_TRANSLATE -13000
#define Z_TRANSLATE -13000
#define NUM_OF_TEXTURES 3


/* TODO:
		Procedural generation of terrain
*/

class Terrain {
	friend class GLRenderer;
protected:

	std::multimap<_vec2, Vertex> DepthMap;
	std::multimap<_vec2, bool> SpawnMap;

	std::vector<Vertex> vertices;
	GLuint terrain_texture;

	void InsertPoint(Vertex v);

	void Draw(GLFWwindow * win);

	bool CheckNothingNearby(_vec2 pos);		// Checks for spawned models near given position

	void ExpandTerrainBasedOnCamPos(glm::vec3 position);

	void RequestUserInput();

	static int MAX_X_POS;
	static int MAX_Z_POS;

	static int HEIGHT_SCALAR;
	static int X_SCALAR;
	static int Z_SCALAR;
	int NUM_OF_LINES = 15;

private:
	void SetupTerrain();
	void GenerateDepthMap();
	void ModifyTerrain();
	void LoadVertices();
	void ExpandTerrain(_vec2 newTile);
	void SetupTerrainEdges();
	void AdjustDepthMapEdge();

	GLuint VAO, VBO;

	int m_TextureNum;

	std::vector<_vec2> m_Tiles;
	_vec2 m_TerrainEdges;
};