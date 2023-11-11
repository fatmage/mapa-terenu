#include "Camera.hpp"

extern GLFWwindow *window;

extern bool firstView;


Camera::Camera() {
    setMainWindow(600,600);
    lastX = 0;
    lastY = 0;
    setSecondaryWindow(240,240);
    pitch = 90.0f;
    yaw = 0.0f;

    cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraPos2 = glm::vec3(-9.0f, 4.0f, 6.0f);
    cameraActPos2 = glm::vec3(0.0, 0.0, 0.0);
    cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    cameraFront2 = glm::vec3(0.0f, 0.0f, 1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); 
    cameraUp2 = glm::vec3(0.0, 0.0, 1.0);
}

void Camera::resize(int w, int h) {
    setMainWindow(w, h);
	setSecondaryWindow(w, h);
}

void Camera::setMainWindow(int w, int h) {
    mainWindowWidth = w;
    mainWindowHeight = h;
}

void Camera::setSecondaryWindow(int w, int h) {
    secondaryWindowHeight = h/2.5;
    secondaryWindowWidth = h/2.5;
}


int Camera::getMainWidth() {
    return mainWindowWidth;
}

int Camera::getMainHeight() {
    return mainWindowHeight;
}

int Camera::getSecondaryWidth() {
    return secondaryWindowWidth;
}

int Camera::getSecondaryHeight() {
    return secondaryWindowHeight;
}

void Camera::setPosition(GLfloat x, GLfloat y, GLfloat z) {
    cameraPos.x = x;
    cameraPos.y = y;
    cameraPos.z = z;
}

void Camera::setSecondaryPosition(GLfloat lon, GLfloat lat, GLfloat ht) {
    cameraPos2.x = lon;
    cameraPos2.y = lat;
    cameraPos2.z = ht;

    updateActPos2();
}

void Camera::updateActPos2() {
    cameraActPos2 = glm::vec3(cameraPos2.z, 0.0, 0.0);

    glm::mat3 rot_x = glm::mat3(1.0,       0.0,        0.0,
                  0.0, cos(cameraPos2.x), -sin(cameraPos2.x),
                  0.0, sin(cameraPos2.x),  cos(cameraPos2.x));

    glm::mat3 rot_y =  glm::mat3(cos(cameraPos2.y), 0.0, sin(cameraPos2.y),
                         0.0, 1.0,       0.0,
                  -sin(cameraPos2.y), 0.0, cos(cameraPos2.y));

    rot_x = glm::transpose(rot_x);
    rot_y = glm::transpose(rot_y);

    cameraActPos2 = rot_y * cameraActPos2;
    cameraActPos2 = rot_x * cameraActPos2;

    cameraUp2 = glm::normalize(cameraActPos2);
}

glm::mat4 Camera::getMainProjection() {
    return glm::perspective(glm::radians(60.0f),
                            (float)mainWindowWidth / (float)mainWindowHeight, 
                            0.00009f,             
                            500.0f);
}

void Camera::changeZoom(GLfloat delta) {
    zoom += delta;
}

glm::mat4 Camera::getSecondaryProjection() {
    return glm::perspective(glm::radians(60.0f),
                            (float)mainWindowWidth / (float)mainWindowHeight, 
                            0.1f,             
                            20000.0f);
}


glm::mat4 Camera::getMainView() {
    return glm::lookAt( cameraPos,
                        glm::vec3(cameraPos.x, cameraPos.y, 0.0),
                        cameraUp);

}

glm::mat4 Camera::getSecondaryView() {

    return glm::lookAt( cameraActPos2,
                        cameraActPos2 + cameraFront2,
                        cameraUp2);
}

void Camera::moveForward(GLfloat speed, GLfloat delta, bool mode) {
    if (mode)
        cameraPos += speed * delta * glm::vec3(0.0, 1.0, 0.0);
    else  {
        cameraPos2.y += 0.004;
        updateActPos2();
    }
        //cameraPos2 += (speed * 100000) * delta * cameraFront2;
    
}

void Camera::moveBackward(GLfloat speed, GLfloat delta, bool mode) {
    if (mode)
        cameraPos += speed * delta * glm::vec3(0.0, -1.0, 0.0);
    else {
        cameraPos2.y -= 0.004;
        updateActPos2();
    }
        //cameraPos2 -= (speed * 100000) * delta * cameraFront2;

}

void Camera::moveLeft(GLfloat speed, GLfloat delta, bool mode) {
    if (mode)
        cameraPos += speed * delta * glm::vec3(-1.0, 0.0, 0.0);
    else {
        cameraPos2.x += 0.008;
        updateActPos2();
        //cameraPos2 -= glm::normalize(glm::cross(cameraFront2, cameraUp)) * (speed * 100000) * delta;
    }
}

void Camera::moveRight(GLfloat speed, GLfloat delta, bool mode) {
    if (mode)
        cameraPos += speed * delta * glm::vec3(1.0, 0.0, 0.0);
    else {
        cameraPos2.x -= 0.008;
        updateActPos2();
        //cameraPos2 += glm::normalize(glm::cross(cameraFront2, cameraUp)) * (speed * 100000) * delta;
    }
}

void Camera::moveUp(GLfloat speed, GLfloat delta, bool mode) {
    if (mode)
        cameraPos += (GLfloat)3.0 * speed * delta * glm::vec3(0.0, 0.0, 0.5);
    else {
        cameraPos2.z += 18000 * delta * speed;
        updateActPos2();
    }
        //cameraPos2 += cameraUp * (speed * 100000) * delta;

}

void Camera::moveDown(GLfloat speed, GLfloat delta, bool mode) {
    if (mode) {
        glm::vec3 old = cameraPos;
        cameraPos += (GLfloat)3.0 * speed * delta * glm::vec3(0.0, 0.0, -0.5);
        if (cameraPos.z <= 0.0001)
            cameraPos = old;
    } else {
        cameraPos2.z -= 18000 * delta * speed;
        if (cameraPos2.z < 6390.0)
            cameraPos2.z = 6390.0;
        updateActPos2();
    }
        //cameraPos2 -= cameraUp * (speed * 100000) * delta;



}

void Camera::rotateMouse(float xpos, float ypos) {
    static bool first = true;
    if (first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    const float sensitivity = 0.08f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    //if (pitch > 89.0f)
    //    pitch = 89.0f;
    //if (pitch < -89.0f)
    //    pitch = -89.0f;

    cameraFront2 = cameraUp2;

    glm::mat3 rot_x = glm::mat3(1.0,       0.0,        0.0,
                  0.0, cos(glm::radians(yaw)), -sin(glm::radians(yaw)),
                  0.0, sin(glm::radians(yaw)),  cos(glm::radians(yaw)));

    glm::mat3 rot_y =  glm::mat3(cos(glm::radians(pitch)), 0.0, sin(glm::radians(pitch)),
                         0.0, 1.0,       0.0,
                  -sin(glm::radians(pitch)), 0.0, cos(glm::radians(pitch)));

    rot_x = glm::transpose(rot_x);
    rot_y = glm::transpose(rot_y);

    cameraFront2 = rot_y * cameraFront2;
    cameraFront2 = rot_x * cameraFront2;


    //cameraFront2 = glm::normalize(glm::vec3( cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    //                                        sin(glm::radians(pitch)),
    //                                        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
    
}

void Camera::rotateLeft(GLfloat speed) {
    yaw -= 70.0 * speed;

    cameraFront = glm::normalize(glm::vec3( cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                            sin(glm::radians(pitch)),
                                            sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

void Camera::rotateRight(GLfloat speed) {
    yaw += 70.0 * speed;

    cameraFront = glm::normalize(glm::vec3( cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                            sin(glm::radians(pitch)),
                                            sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}


void Camera::rotateDown(GLfloat speed) {
    pitch += 70.0 * speed;

    if (pitch > 89.0f)
     pitch = 89.0f;

    cameraFront = glm::normalize(glm::vec3( cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                            sin(glm::radians(pitch)),
                                            sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

void Camera::rotateUp(GLfloat speed) {
    pitch -= 70.0 * speed;

    if (pitch < -89.0f)
        pitch = -89.0f;

    cameraFront = glm::normalize(glm::vec3( cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                            sin(glm::radians(pitch)),
                                            sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

void Camera::setMainViewport() {
	glViewport(0, 0, mainWindowWidth, mainWindowHeight);
}

void Camera::setSecondaryViewport() {
	glViewport(0, 0, secondaryWindowWidth, secondaryWindowHeight);
}





