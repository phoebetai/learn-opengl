#pragma once

#include "shader.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
	public:
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
			this->vertices = vertices;
			this->indices  = indices;
			this->textures = textures;

			setupMesh();
		}

		void draw(Shader &shader) {
			unsigned int diffuseNum  = 1;
			unsigned int specularNum = 1;

			for (unsigned int i = 0; i < textures.size(); i++) {
				// Assign texture name
				string number;
				string name = textures[i].type;
				if (name == "texture_diffuse") {
					number = to_string(diffuseNum++);
				} else if (name == "texture_specular") {
					number = to_string(specularNum++);
				}

				// Bind texture to sampler location
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);

				// Set as shader param
				shader.setInt(("" + name + number).c_str(), i);
			}
			// Reset to default
			glActiveTexture(GL_TEXTURE0);
			
			// Draw mesh
			glBindVertexArray(vertexArrayObj);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

	private:
		unsigned int vertexArrayObj, vertexBufferObj, elementBufferObj;

		void setupMesh() {
			// Create objects
			glGenVertexArrays(1, &vertexArrayObj);
			glGenBuffers(1, &vertexBufferObj);
			glGenBuffers(1, &elementBufferObj);

			// Bind first to store state
			glBindVertexArray(vertexArrayObj);

			// Initialize vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		
			// Initialize index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// Configure vertex attributes
			{
				// Postion
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
				glEnableVertexAttribArray(0);

				// Normal
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
				glEnableVertexAttribArray(1);

				// Texcoords
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
				glEnableVertexAttribArray(2);
			}

			// Unbind
			glBindVertexArray(0);
		}
};