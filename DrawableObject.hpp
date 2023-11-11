#ifndef DRAWABLEOBJECT_HPP
#define DRAWABLEOBJECT_HPP

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#define PI 3.1415926535897932384626433832795

#include <stdlib.h>

class DrawableObject {
    protected:
    GLuint vao, vbo, ebo;
    GLuint programID;

    public:
    void createBuffers() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }
    DrawableObject() {
      createBuffers();
    }
    ~DrawableObject() {    // Cleanup VBO,VBO,Prog
      glDeleteBuffers(1, &vbo);
      glDeleteVertexArrays(1, &vao);
      glDeleteProgram(programID);
    }
    void bindVAO() {
        glBindVertexArray(vao);
    }
    void bindProgram() {
        glUseProgram(programID);
    }
   void bindBuffers() {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    }

    void setUniform3f(const char* name, GLfloat v1, GLfloat v2, GLfloat v3) {
        int location = glGetUniformLocation(programID, name);
        if (location >= 0) {
            bindProgram();
            glUniform3f(location, v1, v2, v3);
        } else {
            //printf("No uniform variable in shader of program %d named %s!\n",programID, name);
        }
    }
    void setUniformvec3(const char* name, glm::vec3 vec) {
        int location = glGetUniformLocation(programID, name);
        if (location >= 0) {
            bindProgram();
            glUniform3f(location, vec.x, vec.y, vec.z);
        } else {
            //printf("No uniform variable in shader of program %d named %s!\n",programID, name);
        }
    }
    void setUniform1f(const char* name, GLfloat v) {
        int location = glGetUniformLocation(programID, name);
        if (location >= 0) {
            bindProgram();
            glUniform1f(location, v);
        } else {
            //printf("No uniform variable in shader of program %d named %s!\n",programID, name);
        }
    }
    void setUniformmat4(const char* name, glm::mat4 v) {
        int location = glGetUniformLocation(programID, name);
        if (location >= 0) {
            bindProgram();
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
        } else {
            //printf("No uniform variable in shader of program %d named %s!\n",programID, name);
        }
    }

};


#endif
