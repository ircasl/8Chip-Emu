// My 8 Chip-Emu
// A simple 8 Chip emulator using GLFW and an interpreter

#include <stdio.h>
#include "chip8.h"

#define GLFW_DLL //Define this MACRO so that GLFW know that the functions are defined in a dll
#include <glfw3.h>

// 8 Chip screen resolution
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// Let's define a initial zoom so that we can see the display better 
#define INITIAL_ZOOM 10 

// Actual Window size
struct WindowSize {
	int display_width = SCREEN_WIDTH * INITIAL_ZOOM;
	int display_height = SCREEN_HEIGHT * INITIAL_ZOOM;

	int& dw = display_width; //Make some references so that the code is easier to read
	int& dh = display_height;
}WS;

chip8 CPU;

// Let's declare all callBackFunctions here
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char** argv)
{
	//Comment out for now to test keyboard events

	if (argc < 2) // See if we received atleast a aplication to run
	{
		printf("usage: 8chip-emu.exe chip8app\n\n");
		return 1;
	}

	//// Call out Chip8 interpreter so that it loads the game to memory
	if (!CPU.loadApplication(argv[1]))
		return -1; //if this function doesn't return true there was an error

	//Let's now setup OpenGL
	GLFWwindow* window;

	// Define the error callback function to use
	glfwSetErrorCallback(error_callback);

	// Initialize the library 
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(WS.dw, WS.dh, "8-Chip Emu", NULL, NULL);
	if (!window)
	{
		glfwTerminate(); //Test if window creation was successful
		return -1;
	}

	//Set key processing for the window
	glfwSetKeyCallback(window, key_callback);

	// Make the window's context current 
	glfwMakeContextCurrent(window);

	// Loop until the user closes the window 
	while (!glfwWindowShouldClose(window))
	{
		// Render here 

		// Do stuff here
		CPU.emulateCycle(); //Let's advance a cycle
		
		if (CPU.DrawFlag == true) {
			glClear(GL_COLOR_BUFFER_BIT);

			//For now let's use the debug render
			//CPU.debugRender(); 

			// Swap front and back buffers 
			glfwSwapBuffers(window);

			// End of frame
			CPU.DrawFlag = false;
		}

		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// OpenGL keyProcessing
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;

		case GLFW_KEY_1:
			CPU.Key[0x1] = 1;
			break;
		case GLFW_KEY_2:
			CPU.Key[0x2] = 1;
			break;
		case GLFW_KEY_3:
			CPU.Key[0x3] = 1;
			break;
		case GLFW_KEY_4:
			CPU.Key[0xC] = 1;
			break;

		case GLFW_KEY_Q:
			CPU.Key[0x4] = 1;
			break;
		case GLFW_KEY_W:
			CPU.Key[0x5] = 1;
			break;
		case GLFW_KEY_E:
			CPU.Key[0x6] = 1;
			break;
		case GLFW_KEY_R:
			CPU.Key[0xD] = 1;
			break;

		case GLFW_KEY_A:
			CPU.Key[0x7] = 1;
			break;
		case GLFW_KEY_S:
			CPU.Key[0x8] = 1;
			break;
		case GLFW_KEY_D:
			CPU.Key[0x9] = 1;
			break;
		case GLFW_KEY_F:
			CPU.Key[0xE] = 1;
			break;

		case GLFW_KEY_Z:
			CPU.Key[0xA] = 1;
			break;
		case GLFW_KEY_X:
			CPU.Key[0x0] = 1;
			break;
		case GLFW_KEY_C:
			CPU.Key[0xB] = 1;
			break;
		case GLFW_KEY_V:
			CPU.Key[0xF] = 1;
			break;

		default:
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_1:
			CPU.Key[0x1] = 0;
			break;
		case GLFW_KEY_2:
			CPU.Key[0x2] = 0;
			break;
		case GLFW_KEY_3:
			CPU.Key[0x3] = 0;
			break;
		case GLFW_KEY_4:
			CPU.Key[0xC] = 0;
			break;

		case GLFW_KEY_Q:
			CPU.Key[0x4] = 0;
			break;
		case GLFW_KEY_W:
			CPU.Key[0x5] = 0;
			break;
		case GLFW_KEY_E:
			CPU.Key[0x6] = 0;
			break;
		case GLFW_KEY_R:
			CPU.Key[0xD] = 0;
			break;

		case GLFW_KEY_A:
			CPU.Key[0x7] = 0;
			break;
		case GLFW_KEY_S:
			CPU.Key[0x8] = 0;
			break;
		case GLFW_KEY_D:
			CPU.Key[0x9] = 0;
			break;
		case GLFW_KEY_F:
			CPU.Key[0xE] = 0;
			break;

		case GLFW_KEY_Z:
			CPU.Key[0xA] = 0;
			break;
		case GLFW_KEY_X:
			CPU.Key[0x0] = 0;
			break;
		case GLFW_KEY_C:
			CPU.Key[0xB] = 0;
			break;
		case GLFW_KEY_V:
			CPU.Key[0xF] = 0;
			break;

		default:
			break;
		}
	}
}

// OpenGL error callback function
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}