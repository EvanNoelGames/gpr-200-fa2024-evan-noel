#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <evan/shader.h>

#include <evan/texture2d.h>
#include <ew/external/stb_image.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Textures", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	float vertices[] = {
		// positions          // texture coords
		0.25f,   0.25f, 0.0f, 1.0f, 1.0f,  // top right
		0.25f,  -0.5f,  0.0f, 1.0f, 0.0f,  // bottom right
		-0.25f, -0.5f,  0.0f, 0.0f, 0.0f,  // bottom left
		-0.25f,  0.25f, 0.0f, 0.0f, 1.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// Create EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// Create Vertex Buffer Object
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Put data into the currently bound VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Make the texture
	evan::Texture2D smile("assets/smile.png", GL_NEAREST, GL_CLAMP_TO_EDGE, true);
	unsigned int texture = smile.GetID();

	// Make the shader object and provide the vertex and fragment shaders
	evan::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// get uniform variable locations
	int timeLocation = glGetUniformLocation(shader.ID, "uTime");

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Clear framebuffer
		glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// update the time
		float time = (float)glfwGetTime();

		glUniform1f(timeLocation, time);

		shader.use();

		// DRAW CALL
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Drawing happens here
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
