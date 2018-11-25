#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <vector>

//sasaki3
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//sasaki
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"

class Node
{
public:
    std::vector<class Node*> children;
    void addChild(Node *c){
        children.emplace_back(c);
    };
    virtual void draw(GLint shaderProgram, glm::mat4 M){};
};

class Transform : public Node
{
public:
    glm::mat4 MT;
    void update(float scale);
    void spin(glm::vec3 w);
    void rescale(float scale);
    
    virtual void draw(GLint shaderProgram, glm::mat4 M){
        glm::mat4 M_new = M * MT;
        for(int i=0; i<children.size(); i++ ){
            children[i]->draw(shaderProgram, M_new);
        }
    };
};

class Geometry : public Node
{
private:
    std::vector<glm::vec3> tmp_vertices;
    std::vector<glm::vec3> tmp_normals;
    std::vector<glm::vec2> tmp_uvs;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<std::vector<unsigned int> > tmp_indices;
    std::vector<std::vector<unsigned int> > indices;

    GLfloat vertices_data[100000000][8];
    GLuint indices_data[100000000][3];
    
public:
	Geometry(const char* filepath);
    ~Geometry();
    //glm::mat4 toWorld;
    glm::vec3 color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat ambientStrength{0.3f};
    GLfloat diffuseStrength{0.5f};
    GLfloat specularStrength{1.0f};
    
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview;
    
    void parse(const char* filepath);
    
    //sasaki3
    GLuint texture[1];
    unsigned char* loadPPM(const char* filename, int& width, int& height);
    void loadTexture();
    
    void setModelMatrix(glm::mat4 M);
    void render(GLint shaderProgram);
    virtual void draw(GLint shaderProgram, glm::mat4 M);
    glm::mat4 modelview;
};


#endif
