#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>
#include <stb_image.h>
#include "model.h"

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint loadTexture(char const* texPath);

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

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL_8", NULL, NULL);
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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	//Loading shaders--------------------------
	unsigned int shaderProgram = loadShader("ver8.vs", "frag8.fs");
	stbi_set_flip_vertically_on_load(true);
	//string modelPath = "backpack/backpack.obj";
	//string modelPath = "spider/spider.obj";
	string modelPath = "suz/suzanne.obj";
	//string modelPath = "others/teapot.obj";
	//string modelPath = "others/tetrahedron.obj";
	//string modelPath = "others/bones.obj";
	Model loadedModel = Model(modelPath);
	
	//string suzanne = "suz";
	//unsigned int suzTexture = TextureFromFile("diffuse.jpg", suzanne);
	
	glm::vec3 lightPos(1.2f, 0.7f, 2.2f);

	//make spaces for multiple texture units
	glUseProgram(shaderProgram);

	glEnable(GL_DEPTH_TEST);

	cout << "Main Loop---------------------------------------\n";
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//rendering part
		glUseProgram(shaderProgram);

		//mat------------
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection * view * model));

		glUniform1f(glGetUniformLocation(shaderProgram, "time"), glfwGetTime());

		//light related unoforms------
		//value_ptr() <- convert data in glm form
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		unsigned int lightColLoc = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3fv(lightColLoc, 1, glm::value_ptr(lightColor));
		unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));

		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

		glUniform3fv(glGetUniformLocation(shaderProgram, "light.direction"), 1, glm::value_ptr(glm::vec3(0.5f, 1.0f, 0.5f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, glm::value_ptr(lightPos));
		//glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, glm::value_ptr(cameraPos)); //flashlight effect
		//glUniform3fv(glGetUniformLocation(shaderProgram, "light.direction"), 1, glm::value_ptr(cameraFront)); //flashlight effect
		glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), glm::cos(glm::radians(12.5f))); //spotlight effect
		glUniform1f(glGetUniformLocation(shaderProgram, "light.outerCutoff"), glm::cos(glm::radians(17.5f))); //spotlight effect
		glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), 0.14);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), 0.07);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambientStrength"), 1, glm::value_ptr(glm::vec3(0.2f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuseStrength"), 1, glm::value_ptr(glm::vec3(0.5f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.specularStrength"), 1, glm::value_ptr(glm::vec3(0.5f)));

		//activate texture
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);*/
		//glActiveTexture(GL_TEXTURE0);
		//glUniform1i(glGetUniformLocation(shaderProgram, ("material.texture_diffuse1")), 0);
		//glBindTexture(GL_TEXTURE_2D, suzTexture);

		//draw
		loadedModel.Draw(shaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);

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

GLuint loadTexture(char const* texPath) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//setting wrapping, filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load texture image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 2) format = GL_RG;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);  //free memory

	return texture;
}