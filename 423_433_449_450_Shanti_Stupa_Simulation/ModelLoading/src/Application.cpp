#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include"..\Shaders.h"
#include "..\Model.h"
#include"..\Camera.h"
#include"..\Textures.h"
void Input(GLFWwindow* window, float &a, float &b, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &deltaTime);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
Camera *camera = new Camera();

int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	if (glewInit() != GLEW_OK)
			{
				std::cout << "Failed to initialize GLEW" << std::endl;
				getchar();
				return -1;
			}
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	Shaders shader, skyShader;
	shader.LoadFromFile(GL_VERTEX_SHADER, "./res/Shaders/shader.vertex");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "./res/Shaders/shader.fragment");
	shader.createProgram();

	skyShader.LoadFromFile(GL_VERTEX_SHADER, "./res/Shaders/skyShader.vertex");
	skyShader.LoadFromFile(GL_FRAGMENT_SHADER, "./res/Shaders/skyShader.fragment");
	skyShader.createProgram();



	float planeVertices[] = {
		50.0f, -0.5f, 50.0f, 2.0f, 0.0f, 0.0f, 50.0f, 0.0f,
		-50.0f, -0.5f, 50.0f, 0.0f, 0.0f, 0.0f, 50.0f, 0.0f,
		-50.0f, -0.5f, -50.0f, 0.0f, 2.0f, 0.0f, 50.0f, 1.0f,

		50.0f, -0.5f, 50.0f, 2.0f, 0.0f, 0.0f, 100.0f, 0.0f,
		-50.0f, -0.5f, -50.0f, 0.0f, 2.0f, 0.0f, 100.0f, 0.0f,
		50.0f, -0.5f, -50.0f, 2.0f, 2.0f, 0.0f, 100.0f, 0.0f,
	};

	//plane VBO;

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glBindVertexArray(0);

	float skyboxVertices[] = {
		// positions          
		-20.0f,  20.0f, -40.0f,
		-20.0f, -1.25f, -40.0f,
		20.0f, -1.25f, -40.0f,
		20.0f, -1.25f, -40.0f,
		20.0f,  20.0f, -40.0f,
		-20.0f,  20.0f, -40.0f,

		-20.0f, -1.25f,  40.0f,
		-20.0f, -1.25f, -40.0f,
		-20.0f,  20.0f, -40.0f,
		-20.0f,  20.0f, -40.0f,
		-20.0f,  20.0f,  40.0f,
		-20.0f, -1.25f,  40.0f,

		20.0f, -1.25f, -40.0f,
		20.0f, -1.25f,  40.0f,
		20.0f,  20.0f,  40.0f,
		20.0f,  20.0f,  40.0f,
		20.0f,  20.0f, -40.0f,
		20.0f, -1.25f, -40.0f,

		-20.0f, -1.25f,  40.0f,
		-20.0f,  20.0f,  40.0f,
		20.0f,  20.0f,  40.0f,
		20.0f,  20.0f,  40.0f,
		20.0f, -1.25f,  40.0f,
		-20.0f, -1.25f,  40.0f,

		-20.0f,  20.0f, -40.0f,
		20.0f,  20.0f, -40.0f,
		20.0f,  20.0f,  40.0f,
		20.0f,  20.0f,  40.0f,
		-20.0f,  20.0f,  40.0f,
		-20.0f,  20.0f, -40.0f,

		-20.0f, -1.25f, -40.0f,
		-20.0f, -1.25f,  40.0f,
		20.0f, -1.25f, -40.0f,
		20.0f, -1.25f, -40.0f,
		-20.0f, -1.25f,  40.0f,
		20.0f, -1.25f,  40.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);


	float a = 0.0f;
	float b = 0.0f;
	//Model ourModel("model/scene.fbx");
	//	Model ourModel("Model1.obj");
//Model ourModel("./res/models/house/house.obj");
	Model ourModel("./res/models/Model1/Model1.obj");
	//Textures *texture1 = new Textures("./res/Textures/concrete.jpg", false);
	//Textures *texture = new Textures("./res/Textures/Emoji.jpg", false);
	Textures* texture = new Textures("./res/Textures/grass.jpg", false);
	

	
	std::vector<std::string> faces
	{
		"./res/Textures/city/left.jpg",
		"./res/Textures/city/right.jpg",
		"./res/Textures/city/top.jpg",
		"./res/Textures/city/bottom.jpg",
		"./res/Textures/city/front.jpg",
		"./res/Textures/city/back.jpg"
	};

	Textures *cubeTexture = new Textures(faces, false);
	//Model ourModel1("./res/models/Cart/model.obj");
	shader.useProgram();
	glUniform1i(glGetUniformLocation(shader.getProgramID(), "texture_diffuse1"), 0);
	skyShader.useProgram();
	glUniform1i(glGetUniformLocation(skyShader.getProgramID(), "skybox"), 0);

	glm::vec3 lightPos = glm::vec3(-5.0f, 2.0f, 10.0f);

	while (!glfwWindowShouldClose(window))
			{
		Input(window, a, b, camera->cameraPos, camera->cameraFront, camera->cameraUp, deltaTime);
				float currentFrame = glfwGetTime();
				deltaTime = currentFrame - lastFrame;
				lastFrame = currentFrame;
		               
				glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				skyShader.useProgram();
				glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)800.0f / (float)600.0f, 0.1f, 100.0f);
				glm::mat4 view = glm::mat4(1.0f);
                glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
				view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp); // remove translation from the view matrix
				glUniformMatrix4fv(glGetUniformLocation(skyShader.getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(skyShader.getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
				// skybox cube
				glBindVertexArray(skyboxVAO);
				glActiveTexture(GL_TEXTURE0);
				cubeTexture->bindCubeTextures();
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
	
		 		shader.useProgram();

				glUniform3f(glGetUniformLocation(shader.getProgramID(), "material.ambient"), 0.8f, 0.8f, 0.8f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "material.diffuse"), 0.8f, 0.5f, 1.0f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "material.specular"), 0.5f, 0.5f, 0.5f);

				glUniform3f(glGetUniformLocation(shader.getProgramID(), "light.ambient"), 0.8f, 0.8f, 0.8f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "light.diffuse"), 0.5f, 0.5f, 0.5f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "light.specular"), 1.0f, 1.0f, 1.0f);

				glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightColor"), 1.0f, 1.0f, 1.0f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "objectColor"), 1.0f, 1.0f, 1.0f);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "viewPos"), camera->cameraPos.x, camera->cameraPos.y, camera->cameraPos.z);
				glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);

				
				view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
				glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.2f, -3.5f));
	     		model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));
				model = glm::scale(model, glm::vec3(4.0f ,4.0f ,4.0f ));
			 	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		
				glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
				ourModel.Draw(shader);

				glBindVertexArray(VAO);
				glActiveTexture(GL_TEXTURE0);
				texture->bindTextures();
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -0.7f, -3.0f));
				glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);

				

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	return 0;
}
void Input(GLFWwindow* window, float &a, float &b, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &deltaTime)
{


	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)  cameraPos += glm::cross(cameraFront, cameraUp) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)   cameraPos -= glm::cross(cameraFront, cameraUp) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)    cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    cameraPos += cameraSpeed * cameraFront;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera->mouse_callback(window, xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera->scroll_callback(window, yoffset);
}