#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "Model.h"
#include "shader.h"


class ResourceManager
{
public:

	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Model> Models;

	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
	static Shader GetShader(std::string name);

	static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile);


	static Model LoadModel(const char* modelFile, std::string name);
	static Model GetModel(std::string name);
	static Model loadModelFromFile(const char* modelFile);

	static void Clear();
};

#endif