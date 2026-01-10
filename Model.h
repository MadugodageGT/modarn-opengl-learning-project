#ifndef  CLASS_H_MODEL
#define  CLASS_H_MODEL

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//#include <map>
#include <vector>

class Model {

public: 

	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	Model(const char* path)
	{
		loadModel(path);
	};
	void Draw(Shader& shader);

private:


	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory);
};


#endif // ! CLASS_H_MODEL
