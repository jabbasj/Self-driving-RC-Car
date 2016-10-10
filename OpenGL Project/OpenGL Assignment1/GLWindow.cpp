#include "all_headers.h"


// Initialise GLFW
bool GLWindow::InitializeGLFW() {
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return false;
	}
	return true;
}

// Initialize GLEW
bool GLWindow::InitializeGLEW() {
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW\n";
		glfwTerminate();
		return false;
	}

	return true;
}

bool GLWindow::CreateGLFWWindow(const char * name, int width, int height) {

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (window == NULL){
		std::cout << "Failed to open GLFW window. OpenGL Version incompatible maybe?\n";
		CloseWindows();
		return false;
	}

	// Create its openGL context
	MakeContext();

	return true;
}


// Close all OpenGL windows and terminate GLFW
void GLWindow::CloseWindows() {
	glfwTerminate();
}


// Create the window's OpenGL context and/or sets it as current
void GLWindow::MakeContext() {
	glfwMakeContextCurrent(window);
}


// static
// Key Press Handler & closing Handler
void GLWindow::KeyCallBack(GLFWwindow * win, int key, int scancode, int action, int mods) {

	// Close Window
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(win) != 0) {
		glfwSetWindowShouldClose(win, true);
	}

	// Wireframe
	if (glfwGetKey(win, GLFW_KEY_KP_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Filled
	if (glfwGetKey(win, GLFW_KEY_KP_2) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Points
	if (glfwGetKey(win, GLFW_KEY_KP_3) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

}

// static
// Error handler
void GLWindow::ErrorCallBack(int error, const char * description) {
	std::cout << "Error!!\n";
	std::cout << description << std::endl;
	system("pause");
}


void GLWindow::SetKeyListener() {

	// Allows to receive key presses
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(window, cursor);

	// Allows to receive mouse button presses
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Center the mouse
	glfwSetCursorPos(window, width / 2, height / 2);

	// Set callbacks
	glfwSetKeyCallback(window, KeyCallBack);
	glfwSetErrorCallback(ErrorCallBack);

}