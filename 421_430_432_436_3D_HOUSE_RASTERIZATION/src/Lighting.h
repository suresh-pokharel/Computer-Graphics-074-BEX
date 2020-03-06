#include "includes.h"
#include "Shader.h"
#include "Camera.h"
class Lighting {
private:
	Shader m_Shader;
	Camera m_Camera;
	glm::vec3 m_pointLightPositions[4] = {
	glm::vec3(1.4f,  -0.8f,  -3.8f),
	glm::vec3(8.6f, -0.1f, -7.5f),
	glm::vec3(6.2f,  -0.6f, -4.4f),
	glm::vec3(4.4f,  0.3f, -9.6f)
	};

	

public:
	Lighting(Shader shader , Camera camera)
		:m_Shader(shader), m_Camera(camera)
	{

	}

	void setDirLight(bool dirLightEnabled)
	{
		if (dirLightEnabled)
		{
			m_Shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			m_Shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
			m_Shader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
			m_Shader.setVec3("dirLight.specular", 0.8f, 0.8f, 0.8f);
		}
		else
		{
			m_Shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			m_Shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05);
			m_Shader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);
			m_Shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
		}
	}

	void setPointLight()
	{
		//m_Shader.setVec3("pointLights[0].position", m_pointLightPositions[0]);
		//m_Shader.setVec3("pointLights[0].ambient", 1.05f, 1.05f, 1.05f);
		//m_Shader.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
		//m_Shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		//m_Shader.setFloat("pointLights[0].constant", 1.0f);
		//m_Shader.setFloat("pointLights[0].linear", 1.09);
		//m_Shader.setFloat("pointLights[0].quadratic", 1.032);
		// point light 2
		//m_Shader.setVec3("pointLights[1].position", m_pointLightPositions[1]);
		//m_Shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		//m_Shader.setVec3("pointLights[1].diffuse", 0.5f, 0.5f, 0.5f);
		//m_Shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		//m_Shader.setFloat("pointLights[1].constant", 1.0f);
		//m_Shader.setFloat("pointLights[1].linear", 0.09);
		//m_Shader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		m_Shader.setVec3("pointLights[2].position", m_pointLightPositions[2]);
		m_Shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		m_Shader.setVec3("pointLights[2].diffuse", 0.5f, 0.5f, 0.5f);
		m_Shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		m_Shader.setFloat("pointLights[2].constant", 1.0f);
		m_Shader.setFloat("pointLights[2].linear", 0.09);
		m_Shader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		m_Shader.setVec3("pointLights[3].position", m_pointLightPositions[3]);
		m_Shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		m_Shader.setVec3("pointLights[3].diffuse", 0.5f, 0.5f, 0.5f);
		m_Shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		m_Shader.setFloat("pointLights[3].constant", 1.0f);
		m_Shader.setFloat("pointLights[3].linear", 0.09);
		m_Shader.setFloat("pointLights[3].quadratic", 0.032);

	}

	void setSpotLight()
	{
		m_Shader.setVec3("spotLight.position", m_Camera.Position);
		m_Shader.setVec3("spotLight.direction", m_Camera.Front);
		m_Shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		m_Shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		m_Shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		m_Shader.setFloat("spotLight.constant", 1.0f);
		m_Shader.setFloat("spotLight.linear", 0.09);
		m_Shader.setFloat("spotLight.quadratic", 0.032);
		m_Shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		m_Shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	}
};