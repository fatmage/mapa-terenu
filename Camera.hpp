#ifndef CAMERA_HPP
#define CAMERA_HPP

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

#include <stdlib.h>
#include <string.h>

class Camera {
    private:

    float pitch, yaw;

    float lastX, lastY;

    GLfloat zoom = 60.0;

    public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraUp2;
    glm::vec3 cameraPos2;
    glm::vec3 cameraActPos2;
    glm::vec3 cameraFront2;
    int mainWindowWidth, mainWindowHeight;
    int secondaryWindowWidth, secondaryWindowHeight;

    Camera();
    glm::mat4 getMainView();
    glm::mat4 getSecondaryView();
    glm::mat4 getMainProjection();
    glm::mat4 getSecondaryProjection();
    void updateActPos2();
    void setMainWindow(int w, int h);
    void setSecondaryWindow(int w, int h);
    int getMainWidth();
    int getMainHeight();
    int getSecondaryWidth();
    int getSecondaryHeight();
    void resize(int w, int h);
    void moveForward(GLfloat speed, GLfloat delta, bool mode);
    void moveBackward(GLfloat speed, GLfloat delta, bool mode);
    void moveLeft(GLfloat speed, GLfloat delta, bool mode);
    void moveRight(GLfloat speed, GLfloat delta, bool mode);
    void moveUp(GLfloat speed, GLfloat delta, bool mode);
    void moveDown(GLfloat speed, GLfloat delta, bool mode);
    void rotateUp(GLfloat speed);
    void rotateDown(GLfloat speed);
    void rotateLeft(GLfloat speed);
    void rotateRight(GLfloat speed);
    void setPosition(GLfloat x, GLfloat y, GLfloat z);
    void setSecondaryPosition(GLfloat lon, GLfloat lat, GLfloat ht);
    void rotateMouse(float xpos, float ypos);
    void setMainViewport();
    void setSecondaryViewport();
    void changeZoom(GLfloat delta);

};

#endif