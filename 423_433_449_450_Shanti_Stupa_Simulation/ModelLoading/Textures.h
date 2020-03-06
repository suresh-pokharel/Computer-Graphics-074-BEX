#pragma once
#include<iostream>
#include<vector>
class Textures
{
public:
	Textures(const std::string &filePath, bool transparent);
	Textures(const std::vector<std::string> faces, bool transparent);
	void bindTextures();
	void unBindTextures();
	void bindCubeTextures();
	void unBindCubeTextures();
	~Textures();
private:
	int width, height, nrChannels;
	unsigned int id;
	unsigned char *data;
};

