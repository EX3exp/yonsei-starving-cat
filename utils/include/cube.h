
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | v7----|-v4
//  |/      |/
//  v2------v3
//
// Vertex shader: the location (0: position attrib (vec3), 1: normal attrib (vec3),
//                              2: color attrib (vec4), and 3: texture coordinate attrib (vec2))
// Fragment shader: should catch the vertex color from the vertex shader

#ifndef CUBE_H
#define CUBE_H

#include "shader.h"
#include <map>
class Cube {
public:
    
    // vertex position array
    GLfloat cubeVertices[72]  = { // initialized as size = 1 for each dimension, 72 elements
        .5f, .5f, .5f,  -.5f, .5f, .5f,  -.5f,-.5f, .5f,  .5f,-.5f, .5f, // v0,v1,v2,v3 (front)
        .5f, .5f, .5f,   .5f,-.5f, .5f,   .5f,-.5f,-.5f,  .5f, .5f,-.5f, // v0,v3,v4,v5 (right)
        .5f, .5f, .5f,   .5f, .5f,-.5f,  -.5f, .5f,-.5f, -.5f, .5f, .5f, // v0,v5,v6,v1 (top)
        -.5f, .5f, .5f,  -.5f, .5f,-.5f,  -.5f,-.5f,-.5f, -.5f,-.5f, .5f, // v1,v6,v7,v2 (left)
        -.5f,-.5f,-.5f,   .5f,-.5f,-.5f,   .5f,-.5f, .5f, -.5f,-.5f, .5f, // v7,v4,v3,v2 (bottom)
        .5f,-.5f,-.5f,  -.5f,-.5f,-.5f,  -.5f, .5f,-.5f,  .5f, .5f,-.5f  // v4,v7,v6,v5 (back)
    };
    
    // normal array
    GLfloat cubeNormals[72] = {  // 72 elements
        0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,  // v0,v1,v2,v3 (front)
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,  // v0,v5,v6,v1 (top)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  // v1,v6,v7,v2 (left)
        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,  // v7,v4,v3,v2 (bottom)
        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1   // v4,v7,v6,v5 (back)
    };
    
    // colour array
    GLfloat cubeColors[96] = { // initialized as RGBA sollid color for each face, 96 elements
        1, 0, 0, 1,   1, 0, 0, 1,   1, 0, 0, 1,   1, 0, 0, 1, // v0,v1,v2,v3 (front)
        1, 1, 0, 1,   1, 1, 0, 1,   1, 1, 0, 1,   1, 1, 0, 1, // v0,v3,v4,v5 (right)
        0, 1, 0, 1,   0, 1, 0, 1,   0, 1, 0, 1,   0, 1, 0, 1, // v0,v5,v6,v1 (top)
        0, 1, 1, 1,   0, 1, 1, 1,   0, 1, 1, 1,   0, 1, 1, 1, // v1,v6,v7,v2 (left)
        0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 1, // v7,v4,v3,v2 (bottom)
        1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1  // v4,v7,v6,v5 (back)
    };
    
    // texture coord array
    
    GLfloat cubeTexCoords[48] = { // 48 elements
        1, 1,   0, 1,   0, 0,   1, 0,               // v0,v1,v2,v3 (front)
        0, 1,   0, 0,   1, 0,   1, 1,               // v0,v3,v4,v5 (right)
        1, 0,   1, 1,   0, 1,   0, 0,               // v0,v5,v6,v1 (top)
        1, 1,   0, 1,   0, 0,   1, 0,               // v1,v6,v7,v2 (left)
        1, 1,   0, 1,   0, 0,   1, 0,               // v7,v4,v3,v2 (bottom)
        0, 0,   1, 0,   1, 1,   0, 1                // v4,v7,v6,v5 (back)
    };
    
    // index array for glDrawElements()
    // A cube requires 36 indices = 6 sides * 2 tris * 3 verts

    GLuint cubeIndices[36] = { // 36 elements
        0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
        4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
        8, 9, 10,  10, 11, 8,    // v0-v5-v6, v6-v1-v0 (top)
        12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
        16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
        20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
    };

    unsigned int VAO, VBO, EBO;
    std::map<std::u32string, unsigned int> textures; // Map to store textures by name
    std::u32string currentTextureName; // Name of the current texture

    unsigned int vSize = sizeof(cubeVertices);
    unsigned int nSize = sizeof(cubeNormals);
    unsigned int cSize = sizeof(cubeColors);
    unsigned int tSize = sizeof(cubeTexCoords);

    Cube() {
    };

    Cube(float sx, float sy, float sz) {
        scale2(sx, sy, sz);
    };

    Cube(float dx, float dy, float dz, float s) {
        scale(s);
        translate(dx, dy, dz);
    };

    Cube(float dx, float dy, float dz, float sx, float sy, float sz) {
        scale2(sx, sy, sz);
        translate(dx, dy, dz);
    };

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Copy vertex attribute data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize + nSize + cSize + tSize, 0, GL_STATIC_DRAW); // Reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, cubeVertices);                  // Copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, cubeNormals);               // Copy norms after verts
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, cubeColors);          // Copy cols after norms
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize + cSize, tSize, cubeTexCoords); // Copy texs after cols

        // Copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

        // Attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // Position attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vSize); // Normal attrib
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(vSize + nSize)); // Color attrib
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vSize + nSize + cSize)); // Tex coord
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    void draw(Shader* shader) {
        shader->use();
        if (!currentTextureName.empty()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[currentTextureName]);
        }
        shader->setInt("material.diffuse", 0);
        shader->setInt("material.specular", 1);
        shader->setFloat("material.shininess", 32);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

    void addTexture(const std::u32string& name, string texturePath) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load and generate the texture
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture: " << texturePath << std::endl;
        }
        stbi_image_free(data);

        textures[name] = textureID;
    }

    void switchTexture(const std::u32string& name) {
        if (textures.find(name) != textures.end()) {
            currentTextureName = name;
        }
        else {
            std::cout << "Texture " << " not found!" << std::endl;
        }
    }

    void translate(float dx, float dy, float dz) {
        for (int i = 0; i < 72; i++) {
            if (i % 3 == 0) cubeVertices[i] += dx;
            else if (i % 3 == 1) cubeVertices[i] += dy;
            else if (i % 3 == 2) cubeVertices[i] += dz;
        }
    };

    void scale(float s) {
        for (int i = 0; i < 72; i++)
            cubeVertices[i] *= s;
    };

    void scale2(float sx, float sy, float sz) {
        for (int i = 0; i < 72; i++) {
            if (i % 3 == 0) cubeVertices[i] *= sx;
            else if (i % 3 == 1) cubeVertices[i] *= sy;
            else if (i % 3 == 2) cubeVertices[i] *= sz;
        }
    }

    unsigned int loadTexture(const char* texFileName) {
        unsigned int texture;

        // Create texture ids.
        glGenTextures(1, &texture);

        // All upcomming GL_TEXTURE_2D operations now on "texture" object
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters for wrapping.
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture parameters for filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
        unsigned char* image = stbi_load(texFileName, &width, &height, &nrChannels, 0);
        if (!image) {
            printf("texture %s loading error ... \n", texFileName);
        }
        else printf("texture %s loaded\n", texFileName);

        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        return texture;
    }
};

#endif