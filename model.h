#pragma once

#include "mesh.h"
#include "shader.h"

// Open Asset Import Library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include <string>
#include <vector>

using namespace std;

class Model {
	public:
		Model(char *path) {
			loadModel(path);
		}

		void draw(Shader &shader) {
			for (unsigned int i = 0; i < meshes.size(); i++) {
				meshes[i].draw(shader);
			}
		}

	private:
		vector<Mesh> meshes;
		vector<Texture> texturesLoaded;
		string directory;

		void loadModel(string path) {
			// Import scene
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

			// Check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				cout << "Scene import failed: " << importer.GetErrorString() << endl;
				return;
			}

			// Store parent directory
			directory = path.substr(0, path.find_last_of('/'));

			// Recursively process nodes
			processNode(scene->mRootNode, scene);
		}

		void processNode(aiNode *node, const aiScene *scene) {
			// Process all meshes in current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				// Nodes contain indices to scene's mesh array
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}

			// Process child nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
			// Process vertices
			vector<Vertex> vertices;
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;

				// Process position
				aiVector3D pos = mesh->mVertices[i];
				vertex.position = glm::vec3(pos.x, pos.y, pos.z);

				// Process normal
				aiVector3D normal = mesh->mNormals[i];
				vertex.normal = glm::vec3(normal.x, normal.y, normal.z);

				// Process texcoords
				if (mesh->mTextureCoords[0]) {
					aiVector3D texCoords = mesh->mTextureCoords[0][i];
					vertex.texCoords = glm::vec2(texCoords.x, texCoords.y);
				} else {
					// No texcoords
					vertex.texCoords = glm::vec2(0.0f, 0.0f);
				}

				// Push vertex
				vertices.push_back(vertex);
			}

			// Process indices
			vector<unsigned int> indices;
			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
				// Get primitive
				aiFace face = mesh->mFaces[i];

				// Iterate over indices
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					indices.push_back(face.mIndices[j]);
				}
			}

			// Process material
			vector<Texture> textures;
			if (mesh->mMaterialIndex >= 0) {
				// Mesh contains index into scene's material array
				aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

				// Load diffuse maps
				vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

				// Load specular maps
				vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			}

			return Mesh(vertices, indices, textures);
		}

		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
			// Iterate over textures of provided type
			vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
				// Get texture location
				aiString str;
				mat->GetTexture(type, i, &str);

				// Check if texture has already been loaded
				bool skip = false;
				for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
					if (texturesLoaded[j].path.compare(str.C_Str()) == 0) {
						textures.push_back(texturesLoaded[j]);
						skip = true;
						break;
					}
				}

				if (!skip) {
					// Process texture
					Texture texture;
					texture.id = textureFromFile(str.C_Str(), directory);
					texture.type = typeName;
					texture.path = str.C_Str();

					// Push texture
					textures.push_back(texture);
					texturesLoaded.push_back(texture);
				}
			}

			return textures;
		}

		unsigned int textureFromFile(const char *path, const string &directory, bool gamma) {
			string filename = directory + '/' + string(path);

			// Create texture
			unsigned int textureID;
			glGenTextures(1, &textureID);

			// Load texture and generate mipmaps
			int width, height, numComponents;
			unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numComponents, 0);
			if (data) {
				GLenum format;
				if (numComponents == 1) {
					format = GL_RED;
				} else if (numComponents == 3) {
					format = GL_RGB;
				} else if (numComponents == 4) {
					format = GL_RGBA;
				}

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				// Set wrap and filter options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Magnification doesn't use mipmaps
			} else {
				cout << "Texture failed to load at path: " << path << endl;
			}
			stbi_image_free(data);

			return textureID;
		}
};