#include "all_headers.h"

class GLWindow {

public:
	GLFWwindow* window;

	bool InitializeGLFW();
	bool InitializeGLEW();
	bool CreateGLFWWindow(const char * name, int width = 1024, int height = 768);
	void MakeContext();

	void SetKeyListener();
	static void KeyCallBack(GLFWwindow * win, int key, int scancode, int action, int mods);
	static void ErrorCallBack(int error, const char * description);

	void CloseWindows();
};