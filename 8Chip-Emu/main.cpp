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

	chip8 CPU;

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
		glClear(GL_COLOR_BUFFER_BIT);

		//Do stuff here

		// Swap front and back buffers 
		glfwSwapBuffers(window);
		// Poll for and process events 
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// OpenGL keyProcessing
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// OpenGL error callback function
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}