﻿#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 
#include <vector>

#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT 

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

#include "Camera.h"
#include "Shader.h"

Camera* pCamera = nullptr;

void Cleanup()
{
	delete pCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

unsigned int CreateTexture(const std::string& strTexturePath);


void grassRotate(Shader& shaderBlending, glm::mat4& model)
{
	const float pi = 3.1415;
	std::vector<float> angles =
	{
		0.0f,
		pi / 4.0f,
		pi / 2.0f,
		3.0f * pi / 4.0f
	};

	for (const auto& angle : angles)
	{
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		shaderBlending.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void draw5x5Grass(Shader& shaderBlending, glm::mat4& model)
{
	for (float width = -4.9; width <= 4.9; width += 0.4)
	{
		for (float height = -4.9; height <= 4.9; height += 0.6)
		{
			glm::mat4 model2 = glm::translate(model, glm::vec3(width, 0.f, height));
			shaderBlending.SetMat4("model", model2);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			grassRotate(shaderBlending, model2);
		}
	}
}


int main(int argc, char** argv)
{
	std::string strFullExeFileName = argv[0];
	std::string strExePath;
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx)
		strExePath = strFullExeFileName.substr(0, last_slash_idx);


	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 9", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Floor vertices
	float floorVertices[] =
	{
		// positions          // texture Coords 
		5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

		5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
		5.0f, -0.5f, -5.0f,  1.0f, 1.0f
	};

	// Floor VAO si VBO
	unsigned int floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	// Grass vertices
	float grassVertices[] =
	{
		// positions          // texture Coords 
		0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,	0.5f,	0.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,

		0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	0.0f,	0.0f,	1.0f,
		0.5f,	-0.5f,	0.0f,	1.0f,	1.0f
	};

	// Grass VAO si VBO
	unsigned int grassVAO, grassVBO;
	glGenVertexArrays(1, &grassVAO);
	glGenBuffers(1, &grassVBO);
	glBindVertexArray(grassVAO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// Floor texture
	unsigned int floorTexture = CreateTexture(strExePath + "\\ColoredFloor.jpg");

	// Grass texture
	unsigned int grassTexture = CreateTexture(strExePath + "\\grass3.png");

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 0.0, 3.0));

	Shader shaderFloor("Floor.vs", "Floor.fs");
	Shader shaderBlending("Blending.vs", "Blending.fs");
	shaderBlending.SetInt("texture1", 0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0);

		shaderFloor.Use();
		glm::mat4 projection = pCamera->GetProjectionMatrix();
		glm::mat4 view = pCamera->GetViewMatrix();
		shaderFloor.SetMat4("projection", projection);
		shaderFloor.SetMat4("view", view);

		// Draw floor
		glBindVertexArray(floorVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4();
		shaderFloor.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);



		shaderBlending.Use();
		shaderBlending.SetMat4("projection", projection);
		shaderBlending.SetMat4("view", view);

		// Draw vegetation
		glBindVertexArray(grassVAO);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		model = glm::mat4();
		shaderFloor.SetMat4("model", model);

		draw5x5Grass(shaderBlending, model);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	// Clear floor VAO

	// Clear grass VAO

	// glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}

unsigned int CreateTexture(const std::string& strTexturePath)
{
	unsigned int textureId = -1;

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture: " << strTexturePath << std::endl;
	}
	stbi_image_free(data);

	return textureId;
}