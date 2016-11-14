#include "all_headers.h"

// TODO: Random point lights in world
// TODO: Add enemy AI class with basic hp, add shooting particles / effects?
// TODO: Add support for resizing with mouse
// TODO: Add shadows ^.^

/* The engine & renderer combined.
   Manages the shaders, input/output, manipulates the models */
class GLRenderer
{
public:
	void PrepareScene();					// Loads the shaders and programs, initializes opengl params
	void Reshape(int w, int h);				// Changing viewport
	void DrawScene();						// Draws the scene
	void DestroyScene();					// Cleanup

	GLRenderer(GLFWwindow * _win);

	static bool FINISHED_SPAWNING;
	static int PENDING_UPDATE;

protected:

	bool fix_to_ground = false;
	bool ignore_roads = false;

	void SetData();
	void PrepareLights();
	void UpdateMatricesFromInputs();		// Handles Camera movement
	bool CollisionDetection();				// Bounding-box based collision detection with camera				-> TODO: Save matrix multiplications in the model
	void GroundDetection();					// Basic plane collision detection with camera
	void RayTracing();						/* Shoots at camera direction,
											   & computes intersection with closest sphere surrounding model 	-> TODO: Save matrix multiplications in the model*/

	void LoadBankModels();														// Loads all models into the "bank"
	void RenderModel(std::string name, bool transform = false);					// Copies model from bank

	void CopyModelAtMyLocation(Model* copy_this);								// Copy and moves model to camera
	Model* CopyModel(Model* copy_this);											// Copy constructor... avoids reloading object from files.

	int NUM_OF_MODELS = 1;
	void ScatterModels();			// Scatters random model from bank into the world every 10 seconds when holding W and expands terrain if needed
	void HandleModelManipulation();			// Scales (mouse button 4,5) / Translates (right/left click) selected model
	void HandleSpawning();					// Spawns copy of pointed model under camera (P KEY)
	void OutputModelMatrices();				// Writes all the existing model's Current Transformation Matrix into "Object Matrices.txt" (O KEY)
	void ReadModelMatrices();				// Read "Object Matrices.txt" and fills the ModelData array, which holds transformation for a model
	void RequestUserInput();

	void move(); //test streetmap

protected:
	GLProgram* m_pProgram;					// Program
	GLShader*  m_pVertSh;					// Vertex shader
	GLShader*  m_pFragSh;					// Fragment shader

	GLuint MatrixID;						// handle for our "MVP" uniform					-> TODO: Do the matrix multiplications in the shader
	GLuint MatrixID_terrain;

	GLFWwindow * win;

	Terrain m_Terrain;						// Terrain handler
	std::vector<Model *> m_Models;			// Model container
	std::vector<LightInfo> m_Lights;		// Light container
	std::vector<BulletParticle *> m_Bullets;
	Model * skybox;

	std::vector<Model *> m_ModelsBank;		// Models not rendered, used to copy

	glm::mat4 Projection;					// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 View;							// Camera matrix

	GLuint M_MatrixID;						// handle for our "M" uniform
	GLuint V_MatrixID;						// handle for our "V" uniform

	glm::mat4 MVP;							// Our ModelViewProjection : multiplication of our 3 matrices
											// Note: matrix multiplication is the other way around

	glm::vec3 position = glm::vec3(0, 0, 100); // Camera Information
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 direction;

	Model null_model;
	Model* selected = &null_model;				// Currently targeted by ray
	Model* ground = &null_model;

	float horizontalAngle = 3.14f;				// Initial horizontal angle : toward -Z		(angle around Y)
	float verticalAngle = 0.0f;					// Initial vertical angle : none			(angle around X)

	float FoV = 45.0f;							// Field of View
	float speed = 300.0f;						// 100 units / sec
	float mouseSpeed = 0.0025f;

	// Based on possibly non-unique name
	void ModelTranslate(glm::vec3 change, std::string name);
	void ModelRotate(float angle, glm::vec3 vector, std::string name);
	void ModelScale(glm::vec3 change, std::string name);
	void ModelCenter(std::string name);
	void ModelReset(std::string name);
	void ModelTransform(glm::mat4 tm, std::string name);

	// Based on specific model
	void ModelAbsoluteTranslate(glm::vec3 change, Model* update_this);
	void ModelRelativeTranslate(glm::vec3 change, Model* update_this);
	void ModelRotate(float angle, glm::vec3 vector, Model* update_this);
	void ModelRotateSelf(float angle, Model* update_this);
	void ModelScale(glm::vec3 change, Model* update_this);
	void ModelCenter(Model* update_this);
	void ModelReset(Model* update_this);
	void ModelTransform(glm::mat4 tm, Model* update_this);

	std::vector<ModelData> model_data;			// Container for ModelData struct -> info from "Object Matrices.txt" (used to place saved objects back in their position)
};