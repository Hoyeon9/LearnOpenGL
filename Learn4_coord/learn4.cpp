#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "stb_image.h"

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
float yaw = -90.0, pitch;
bool firstMouse = true;
float fov = 45.0f;

int main() {
	//---Creating window(from initialize GLFW~~)---
	glfwInit(); //initilalizes GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //(possible options, valule of our options)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //use major, minor both 3 version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core profile-not forward-compatible

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL_4", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific
		//glfwGetProcAddress: defines the correct function based on which OS we're compiling for
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//callback function, it will resize window size when the change is detected
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	//Loading shaders--------------------------
	unsigned int shaderProgram = loadShader("ver4.vs", "frag4.fs");

	//creating VBO-------------------------------------
	/*float vertices[] = {
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};*/

	float vertices[] = {
		//position         //colors          //texture coords
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.5f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f
	};

	/*unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO, EBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	unsigned int VAOcube;
	glGenVertexArrays(1, &VAOcube);
	glBindVertexArray(VAOcube);

	unsigned int VBOcube;
	glGenBuffers(1, &VBOcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO has been registered by 'glVertexAttribPointer'
	glBindVertexArray(0); //unbind VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //not before VAO unbinded

	//texture------------------------
	/*unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//setting wrapping, filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//loading
	int width, height, nrChannels;
	char const* texPath = "container.jpg";
	unsigned char* data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	stbi_failure_reason();
	std::cout << data;

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);  //free memory*/

	//draw only lines
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glEnable(GL_DEPTH_TEST);
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  1.7f, -2.0f),
		glm::vec3(-0.5f, -0.7f, 1.5f),
		glm::vec3(-1.8f, -0.6f, -2.3f),
		glm::vec3(0.8f, -0.1f, -1.2f),
		glm::vec3(-0.6f,  1.0f, -2.5f),
		glm::vec3(0.4f, -2.1f, -1.0f),
		glm::vec3(-0.5f,  0.6f, -0.8f),
		glm::vec3(1.2f,  0.2f, -3.5f),
		glm::vec3(0.4f,  -0.3f, -1.5f)
	};

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//rendering part
		glUseProgram(shaderProgram);

		//delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//mat------------
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
		unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
		glBindVertexArray(VAOcube);
		for (int i = 0; i < 10; i++) {
			model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection * view * model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection*view*model));
		//value_ptr() <- convert data in glm form

		//glBindTexture(GL_TEXTURE_2D, texture); //binding texture
		//glBindVertexArray(VAOcube);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBOcube);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	if (pitch + yoffset <90.0 && pitch + yoffset > -90.0) {
		pitch += yoffset;
	}
	 
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 0.5f) fov = 0.5f;
	if (fov > 60.0f) fov = 60.0f;
}