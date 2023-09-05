#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mesh.h>
#include<shader.h>
#include <vector>
#include <string>
#include <stb_image.h>

unsigned int TextureFromFile(const char* path, const string& directory);

class Model {
public:
	Model(string path) {
		loadModel(path);
	}
	void Draw(unsigned int shaderID);
private:
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialtextures(aiMaterial* material, aiTextureType type, string typeName);
};
void Model::Draw(unsigned int shaderID) {
	cout << "Draw_Model is called\n";
	for (int i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].Draw(shaderID);
	}
	cout << "Draw_Model Done\n";
}
void Model::loadModel(string path) {
	cout << "Starting model loading...\n";
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP" << import.GetErrorString() << endl;
		return;
	}
	cout << "File reading done\n";
	this->directory = path.substr(0, path.find_last_of('/'));
	cout << "Processing Node..";
	processNode(scene->mRootNode, scene); //process all of the scene's nodes recursively
	cout << "Processing done\n";
}
void Model::processNode(aiNode* node, const aiScene* scene) {
	cout << "processNode: " << node->mName.C_Str() << endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++) { //for all the meshes included in the node
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(processMesh(mesh, scene));
	}
	cout<<"processNode_pushed all mesh of: "<< node->mName.C_Str() << endl;
	//same for its children
	cout << "Call_processNode_for_children: " << node->mName.C_Str() << endl;
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
	cout << "processNode_Done: " << node->mName.C_Str() << "------------" << endl;
}
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialtextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialtextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	
	return Mesh(vertices, indices, textures);
}
vector<Texture> Model::loadMaterialtextures(aiMaterial* material, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		cout << type << " " << i << endl;
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (int j = 0; j < this->textures_loaded.size(); j++) {
			if (std::strcmp(this->textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(this->textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			cout << "load texture from file: " << str.C_Str() <<endl;
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
	}
	return textures;
}
unsigned int TextureFromFile(const char* path, const string& directory) {
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 2) format = GL_RG;
	else if (nrChannels == 3) format = GL_RGB; 
	else if (nrChannels == 4) format = GL_RGBA;
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);  //free memory
	return texture;
}

#endif
