#include "all_headers.h"

BulletParticle::BulletParticle(glm::vec3 start, glm::vec3 dir) {
	start_pos = start;
	current_pos = start;
	direction = dir;
	distance_travelled = 0;

	Setup();
}

void BulletParticle::Setup() {
	sphere = new Model("./models/bullet/sphere.obj", "bullet");

	sphere->AbsoluteTranslate(start_pos);
	sphere->Scale(scale);
	sphere->scale = scale;
	alive = true;
}

void BulletParticle::Destroy() {
	alive = false;
}

void BulletParticle::Draw(GLFWwindow* win) {

	Advance();

	if (distance_travelled > 10000) {
		Destroy();
	}
	else {
		sphere->Draw(win);
	}
}

void BulletParticle::Advance() {
	current_pos += direction * speed;

	sphere->AbsoluteTranslate(direction * speed / scale.x);

	distance_travelled = glm::distance(current_pos, start_pos);
}