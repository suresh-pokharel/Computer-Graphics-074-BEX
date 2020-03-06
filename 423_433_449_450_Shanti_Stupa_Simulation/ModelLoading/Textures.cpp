#include "GL\glew.h"
#include "Textures.h"
#include "src\vendor\stb_image\stb_image.h"

Textures::Textures(const std::string & filePath, bool transparent)
{
	stbi_set_flip_vertically_on_load(1);
	data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data) std::cout << "Failed to Load Texture" << std::endl;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (!transparent)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

}

Textures::Textures(const std::vector<std::string> faces, bool transparent)
{
	stbi_set_flip_vertically_on_load(1);
	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (unsigned int i = 0; i < faces.size(); i++) {
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i].c_str() << std::endl;
			stbi_image_free(data);
		}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}

void Textures::bindTextures()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Textures::unBindTextures()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Textures::bindCubeTextures()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Textures::unBindCubeTextures()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Textures::~Textures()
{
	stbi_image_free(data);
	glDeleteTextures(1, &id);
}
