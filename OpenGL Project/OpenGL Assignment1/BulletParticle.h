#include "all_headers.h"


class BulletParticle {
	friend class GLRenderer;

public:
	BulletParticle(glm::vec3 start, glm::vec3 dir);

protected:

	glm::vec3 start_pos;
	glm::vec3 current_pos;
	glm::vec3 direction;

	bool alive = false;
	float distance_travelled;

	float speed = 100.0f;
	glm::vec3 scale = glm::vec3(0.025, 0.025, 0.025);

	Model * sphere;

	void Setup();
	void Draw(GLFWwindow* win);
	void Advance();
	void Destroy();
};