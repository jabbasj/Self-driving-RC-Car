#include "all_headers.h"

#define X_TRANSLATE 0
#define Z_TRANSLATE 0
#define NUM_OF_TEXTURES 3


class Terrain {
	friend class GLRenderer;
	friend class Autopilot;
protected:

	std::multimap<_vec2, Vertex> DepthMap;
	std::multimap<_vec2, int> StreetMap;
	std::vector<Vertex> vertices;
	GLuint terrain_texture;

	void InsertPoint(Vertex v);
	void Draw(GLFWwindow * win);

	static int MAX_X_POS;
	static int MAX_Z_POS;

	static int HEIGHT_SCALAR;
	static int X_SCALAR;
	static int Z_SCALAR;

private:
	void SetupTerrain();
	void GenerateDepthMap();
	void GenerateStreetMap();
	void LoadVertices();

	GLuint VAO, VBO;

	int m_TextureNum;

	std::vector<_vec2> m_Tiles;
	_vec2 m_TerrainEdges;
};