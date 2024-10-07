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

	// Make the textures
	evan::Texture2D smile("assets/smile.png", GL_NEAREST, GL_CLAMP_TO_EDGE, true);
	unsigned int smileTexture = smile.GetID();

	evan::Texture2D bg("assets/brick.png", GL_LINEAR, GL_REPEAT);
	unsigned int bgTexture = bg.GetID();

	evan::Texture2D bgX("assets/x.png", GL_LINEAR, GL_REPEAT, true);
	unsigned int bgXTexture = bgX.GetID();

	// Make the shader object and provide the vertex and fragment shaders
	evan::Shader smileShader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	evan::Shader bgShader("assets/backgroundVertexShader.vert", "assets/backgroundFragmentShader.frag");

	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// get uniform variable locations
	int smileTimeLocation = glGetUniformLocation(smileShader.ID, "uTime");
	int bgTimeLocation = glGetUniformLocation(bgShader.ID, "uTime");
	int bgFirstTexture = glGetUniformLocation(bgShader.ID, "uTexture");
	int bgSecondTexture = glGetUniformLocation(bgShader.ID, "uTexture2");

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Clear framebuffer
		glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// update the time
		float time = (float)glfwGetTime();

		// BG
		bgShader.use();
		glUniform1f(bgTimeLocation, time);
		glUniform1i(bgFirstTexture, bgTexture);
		glUniform1i(bgSecondTexture, bgXTexture);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, bgXTexture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, bgTexture);

		// DRAW CALL
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// SMILE
		smileShader.use();
		glUniform1f(smileTimeLocation, time);
		// DRAW CALL
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, smileTexture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Drawing happens here
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
