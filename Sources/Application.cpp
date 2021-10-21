#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "stb_image/stb_image.h"
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float op = 1.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float delta = 0.0f;
float lastTime = 0.0f;
int nbFrames = 0;

bool wasButtonPressed = false;
bool State = true;
bool FullScreen = false;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void showFPS(GLFWwindow* window)
{
	// Measure speed
	double currentTime = glfwGetTime();
	delta = static_cast<float>(currentTime) - lastTime;
	nbFrames++;
	if (delta >= 1.0)// If last cout was more than 1 sec ago
	{ 
		//std::cout << 1000.0 / double(nbFrames) << std::endl;

		double fps = double(nbFrames) / delta;

		std::stringstream ss;
		ss << "Gamedale: Magnum " << " [" << fps << " FPS]";

		glfwSetWindowTitle(window, ss.str().c_str());

		nbFrames = 0;
		lastTime = static_cast<float>(currentTime);
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	static int winPosX = (mode->width - SCR_WIDTH) / 2;
	static int winPosY = (mode->height - SCR_HEIGHT) / 2;
	
    static int winSizeX = SCR_WIDTH;
    static int winSizeY = SCR_HEIGHT;

	if (!wasButtonPressed)
	{
		if ((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS))
			glfwSetWindowShouldClose(window, true);
		if ((glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) && (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			State = 0;
			wasButtonPressed = true;
		}
		else if ((glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) && (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			State = 1;
			firstMouse = true;
			wasButtonPressed = true;
		}
		
		if ((glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) && (FullScreen == true))
		{
			glfwSetWindowMonitor(window, nullptr, winPosX, winPosY, winSizeX, winSizeY, GLFW_REFRESH_RATE);
			wasButtonPressed = true;
			FullScreen = false;
		}
		else if ((glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) && (FullScreen == false))
		{
			glfwGetWindowPos(window, &winPosX, &winPosY);   // Save the window position before going fullscreen
			glfwGetWindowSize(window, &winSizeX, &winSizeY); // Save the window size
			glfwHideWindow(window); // Hide so that the legacy (Windows 7?) fallback border won't be visible for a split second
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_REFRESH_RATE);
			glfwFocusWindow(window); // Set the focus on the window once in fullscreen so the user doesn't have to click to interact with the window
			wasButtonPressed = true;
			FullScreen = true;
		}
	}
	if ((glfwGetKey(window, GLFW_KEY_F1)|glfwGetKey(window, GLFW_KEY_F2)) == GLFW_RELEASE)
			wasButtonPressed = false;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.Reset();
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera.InZoom();
	else
		camera.UnZoom();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos) - lastX;
	float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	if (State == 1) camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main()
{
	/*HWND windowe;
	AllocConsole();
	windowe = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(windowe, 0);*/
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gamedale: Magnum", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	GLFWimage images[1] = {};
	images[0].pixels = stbi_load("Resources/Textures/Icon_Green_Smile.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	Shader lightingShader("Resources/Shaders/Color.vs.glsl", "Resources/Shaders/Color.vs.glsl");
	Shader lightCubeShader("Resources/Shaders/Light.vs.glsl", "Resources/Shaders/Light.fs.glsl");

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	float in = 0.0;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Input
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// Rendering commands
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// also draw the lamp object
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Check and call events and swap the buffers
		showFPS(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}//(float)glfwGetTime()