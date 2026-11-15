#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <utility>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"

enum texture_type
{
    DIFF,
    SPEC,
    SHINE
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 texCoord;
};

struct Texture
{
    unsigned int id;
    texture_type type;
};

struct Material
{
    Texture diff;
    Texture spec;
    Texture shininess;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) :
                                                vertices(vertices), indices(indices), material(material)
    {
        setupMesh();
    }

    void Draw(Shader &shader)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diff.id);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.spec.id);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.shininess.id);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    void setupMesh()
    {
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);
    }
};

class Model
{
public:
    Model(const char *path, std::function<void()> pumpEvents)
        : pumpEvents(std::move(pumpEvents))
    {
        loadModel(path);
    }

    void Draw(Shader& shader)
    {
        for (int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);
    }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    unsigned int fallbackTextureID = 0;
    std::unordered_map<std::string, unsigned int> loadedTextures;
    std::function<void()> pumpEvents;

    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            meshes.push_back(processMesh((aiMesh*)scene->mMeshes[node->mMeshes[i]], scene));
            pumpEvents();
        }

        for (int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex v;
            v.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            v.norm = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

            vertices.push_back(v);
        }

        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            {
                indices.push_back(mesh->mFaces[i].mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *m = scene->mMaterials[mesh->mMaterialIndex];
            material.diff = loadMaterialTexture(m, aiTextureType_DIFFUSE, DIFF);
            material.spec = loadMaterialTexture(m, aiTextureType_SPECULAR, SPEC);
            material.shininess = loadMaterialTexture(m, aiTextureType_SHININESS, SHINE);
        }

        return Mesh(vertices, indices, material);
    }

    Texture loadMaterialTexture(aiMaterial *m, aiTextureType type, texture_type textureType)
    {
        aiString string;
        if (m->GetTextureCount(type) > 0 && m->GetTexture(type, 0, &string) == AI_SUCCESS)
        {
            const std::string filename = directory + '/' + string.C_Str();
            const auto cachedTexture = loadedTextures.find(filename);
            if (cachedTexture != loadedTextures.end())
            {
                return {cachedTexture->second, textureType};
            }

            pumpEvents();
            const unsigned int textureID = TextureFromFile(string.C_Str(), directory);
            pumpEvents();
            if (textureID != 0)
            {
                loadedTextures.emplace(filename, textureID);
                return {textureID, textureType};
            }
        }

        if (fallbackTextureID == 0)
        {
            const unsigned char white[] = {255, 255, 255, 255};
            glGenTextures(1, &fallbackTextureID);
            glBindTexture(GL_TEXTURE_2D, fallbackTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        return {fallbackTextureID, textureType};
    }

    unsigned int TextureFromFile(const char *path, const std::string &directory)
    {
        std::string filename = directory + '/' + std::string(path);

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 2)
                format = GL_RG;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
            {
                std::cout << "Unsupported texture format at path: " << filename << std::endl;
                stbi_image_free(data);
                glDeleteTextures(1, &textureID);
                return 0;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }

        return textureID;
    }
};