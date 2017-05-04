#include "all_headers.h"
#define CAP_FPS

int _tmain(int argc, _TCHAR* argv[])
{
	/*** Add breakpoint @ memory leak ***/
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(5476);

	bool statusOkay = false;

	/*vvvvvvvvvv Initialize window,glew, glfw libraries vvvvvvvvvvv*/
	GLWindow myWindowManager;

	statusOkay = myWindowManager.InitializeGLFW();
	statusOkay = myWindowManager.CreateGLFWWindow("Autopilot Simulation", 1024, 768);
	statusOkay = myWindowManager.InitializeGLEW();

	if (!statusOkay) { return 0; }

	myWindowManager.SetKeyListener();
	
	GLRenderer myRenderer(myWindowManager.window);

	Autopilot myAutopilot(&myRenderer);

	myRenderer.m_Autopilot = &myAutopilot;
	myRenderer.PrepareScene();

#ifdef CAP_FPS
	//Code for FPS capping
	const int FRAMES_PER_SECOND = 30;
	const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	DWORD next_game_tick = GetTickCount();
	int sleep_time = 0;
#endif

	/*vvvvvvvvvvvvvvvvv Game Loop vvvvvvvvvvvvvvvvvvv*/
	do{
#ifdef CAP_FPS
		static unsigned int render_time = 0;
#endif
		// Clear & Draw
		myRenderer.DrawScene();

		// Handle events
		glfwPollEvents();

#ifdef CAP_FPS
		next_game_tick += SKIP_TICKS;
		sleep_time = next_game_tick - GetTickCount();
		if (sleep_time >= 0) {
			Sleep(sleep_time);
		}
#endif
	} while (!glfwWindowShouldClose(myWindowManager.window));

	/*^^^^^^^^^^^^^^^^^^^ Game Loop ^^^^^^^^^^^^^^^*/

	// Clean up
	myRenderer.DestroyScene();
	myWindowManager.CloseWindows();

	//system("pause");
	return 0;
}

