#ifndef CLASS_MESH_H
#define CLASS_MESH_H


#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
		: vertices(vertices), indices(indices), textures(textures) {};

	void Draw(Shader &shader);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();

};


#endif // CLASS_MESH_H