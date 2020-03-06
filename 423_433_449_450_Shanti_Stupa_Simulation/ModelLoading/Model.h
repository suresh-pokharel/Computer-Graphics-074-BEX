#pragma once
#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "src\vendor\stb_image\stb_image.h"
#include "Shaders.h"
#include "Mesh.h"
class Model
{
public:
	/*  Model Data */
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) {
		loadModel(path);
	}
	~Model();
	void Draw(Shaders shader);
private:


	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
		aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
};

