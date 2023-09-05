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

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL_7", NULL, NULL);
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
	unsigned int shaderProgram = loadShader("ver7.vs", "frag7.fs");
	unsigned int lightProgram = loadShader("lightVer.vs", "lightFrag.fs");

	float cubeVertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 lightPos(1.2f, 0.7f, 2.2f);

	unsigned int VAOcube;
	glGenVertexArrays(1, &VAOcube);
	glBindVertexArray(VAOcube);

	unsigned int VBOcube;
	glGenBuffers(1, &VBOcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int VAOlight, VBOlight;
	glGenVertexArrays(1, &VAOlight);
	glBindVertexArray(VAOlight);
	glGenBuffers(1, &VBOlight);
	glBindBuffer(GL_ARRAY_BUFFER, VBOlight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO has been registered by 'glVertexAttribPointer'
	glBindVertexArray(0); //unbind VAO

	//texture------------------------
	GLuint texture = loadTexture("container2.png");
	GLuint specularMap = loadTexture("container2_specular.png");

	//make spaces for multiple texture units
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);


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
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(0.0f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

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

		//light related unoforms------
		//value_ptr() <- convert data in glm form
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		unsigned int lightColLoc = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3fv(lightColLoc, 1, glm::value_ptr(lightColor));
		unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));

		glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

		glUniform3fv(glGetUniformLocation(shaderProgram, "light.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, glm::value_ptr(lightPos));
		//glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, glm::value_ptr(cameraPos)); //flashlight effect
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.position"), 1, glm::value_ptr(cameraPos)); //flashlight effect
		//glUniform3fv(glGetUniformLocation(shaderProgram, "light.direction"), 1, glm::value_ptr(cameraFront)); //flashlight effect
		//glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.direction"), 1, glm::value_ptr(cameraFront)); //flashlight effect
		glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), glm::cos(glm::radians(12.5f)));
		//glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shaderProgram, "light.outerCutoff"), glm::cos(glm::radians(17.5f)));
		//glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.outerCutoff"), glm::cos(glm::radians(17.5f)));
		glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), 0.14);
		glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), 0.07);
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambientStrength"), 1, glm::value_ptr(glm::vec3(0.2f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuseStrength"), 1, glm::value_ptr(glm::vec3(0.5f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "light.specularStrength"), 1, glm::value_ptr(glm::vec3(1.0f)));

		glUniform1f(glGetUniformLocation(shaderProgram, "dirLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "dirLight.linear"), 0.14);
		glUniform1f(glGetUniformLocation(shaderProgram, "dirLight.quadratic"), 0.07);
		glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.ambientStrength"), 1, glm::value_ptr(glm::vec3(0.2f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.diffuseStrength"), 1, glm::value_ptr(glm::vec3(0.5f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.specularStrength"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.linear"), 0.14);
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), 0.07);
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.ambientStrength"), 1, glm::value_ptr(glm::vec3(0.2f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.diffuseStrength"), 1, glm::value_ptr(glm::vec3(0.5f)));
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.specularStrength"), 1, glm::value_ptr(glm::vec3(1.0f)));
		for (int i = 0; i < 4; i++) {
			std::string name = "pointLights[" + std::to_string(i);
			glUniform3fv(glGetUniformLocation(shaderProgram, (name + "].position").c_str()), 1, glm::value_ptr(pointLightPositions[i]));
			glUniform1f(glGetUniformLocation(shaderProgram, (name + "].constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram, (name + "].linear").c_str()), 0.14);
			glUniform1f(glGetUniformLocation(shaderProgram, (name + "].quadratic").c_str()), 0.07);
		}

		//activate texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);

		//draw
		glBindVertexArray(VAOcube);
		glm::mat4 models = model;
		for (int i = 0; i < 10; i++) {
			models = glm::scale(models, glm::vec3(0.9f, 0.9f, 0.9f));
			models = glm::translate(models, cubePositions[i]);
			float angle = 20.0f * i;
			models = glm::rotate(models, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection * view * models));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glDrawArrays(GL_TRIANGLES, 0, 36);


		//Light cube
		glUseProgram(lightProgram);
		glm::mat4 modelL = glm::translate(model, lightPos);
		modelL = glm::scale(modelL, glm::vec3(0.2f));
		unsigned int mvpLocLight = glGetUniformLocation(lightProgram, "mvp");
		lightColLoc = glGetUniformLocation(lightProgram, "lightColor");
		glUniform3fv(lightColLoc, 1, glm::value_ptr(lightColor));
		glBindVertexArray(VAOlight);
		for (int i = 0; i < 4; i++) {
			modelL = glm::translate(modelL, pointLightPositions[i]);
			glUniformMatrix4fv(mvpLocLight, 1, GL_FALSE, glm::value_ptr(projection * view * modelL));
			//glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glUniformMatrix4fv(mvpLocLight, 1, GL_FALSE, glm::value_ptr(projection * view * modelL));




		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);
	glDeleteProgram(lightProgram);
	glDeleteBuffers(1, &VBOcube);
	glDeleteBuffers(1, &VAOcube);
	glDeleteBuffers(1, &VAOlight);

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