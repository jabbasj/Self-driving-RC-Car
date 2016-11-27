#pragma once
// ALL headers in 1 file!
// Avoids headache... slow compilation. 

//--- defines
#define _AFXDLL						// ????????
#define _CRT_SECURE_NO_DEPRECATE	// TODO: Remove

#define GLEW_STATIC					// Use static library
//#define GLM_SWIZZLE				// Allows .xyz(), etc... makes compiling very slow! not worth.

//#define PRINT_COLLISIONS_AND_RAYS

//--- includes
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <afxwin.h>
#include <vector>
#include <future>
#include <map>
#include <time.h>
#include <math.h>
#include <functional>
#include <set>

//--- assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

//--- soil
#include <SOIL.h>

//--- openGL Stuff
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/intersect.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/spline.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>

//--- My Classes
#include "my_structs.h"
#include "Terrain.h"
#include "Mesh.h"
#include "Model.h"
#include "GLShader.h"
#include "GLProgram.h"
#include "GLRenderer.h"
#include "GLWindow.h"
#include "Autopilot.h"
#include "AStar.hpp"