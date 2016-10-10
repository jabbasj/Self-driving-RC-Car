#include "all_headers.h"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;		// Faces
	glm::vec2 TexCoords;	// Texture coords, also known as UVs
};

struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};

// Used as key for terrain depth map
struct _vec2 {
	float x;
	float z;

	bool operator<(const _vec2& v) const {

		if (x < v.x)
			return true;
		if (v.x < x)
			return false;
		if (z < v.z)
			return true;
		if (v.z < z)
			return false;
		return false;
	}
};


// Struct for file information -> used to save/load maps
struct ModelData {
	int id;
	std::string name;
	std::string path;
	glm::mat4 transformation;			// Transformation set to the model
	bool centered = false;				// If centroid is at object origin
	bool instanced;
};


struct LightInfo {
	int Type;								//	0: point light, 1: spot light, 2: directed light

	glm::vec3 La;							// Ambient light intensity
	glm::vec3 Ld;							// Diffuse light intensity
	glm::vec3 Ls;							// Specular light intensity

	glm::vec3 LightPosition_worldspace;		// Light Position in world space
	glm::vec3 SpotLightDirection_worldspace;// Spot Light Direction in world space

	float CutOff;							// For Spotlight
	float OuterCutOff;						// For Spotlight

	glm::vec3 LightColor;					// For simplicity, to be removed
	float LightPower;						// For simplicity, to be removed

	bool dynamic = false;

	GLuint Type_ID;
	GLuint LightPosition_worldspace_ID;
	GLuint SpotLightDirection_worldspace_ID;
	GLuint La_ID;
	GLuint Ld_ID;
	GLuint Ls_ID;
	GLuint LightColor_ID;
	GLuint LightPower_ID;
	GLuint CutOff_ID;
	GLuint OuterCutOff_ID;
};


