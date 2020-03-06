#pragma once
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
class Camera
{
public:
	Camera();
	~Camera();
	void mouse_callback(GLFWwindow* window, double &xoffset, double &yoffset);
	void scroll_callback(GLFWwindow* window, double &yoffset);

	float yaw, pitch, fov;
	glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 15.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight;
};

