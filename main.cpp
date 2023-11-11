// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <unordered_set>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#include "DrawableObject.hpp"
#include "Camera.hpp"


#define SRTM_SIZE 1201

Camera camera = Camera();	


// Tutorial 1
GLuint LoadShaders(const char * vertex_path,const char * fragment_path) {

    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string vsCode;
    std::ifstream VertexShaderStream(vertex_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        vsCode = sstr.str();
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string fsCode;
    std::ifstream FragmentShaderStream(fragment_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        fsCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    //printf("Compiling shader: %s\n", vertex_path);
    char const *vsPointer = vsCode.c_str();
    glShaderSource(vsID, 1, &vsPointer , NULL);
    glCompileShader(vsID);

    // Check Vertex Shader
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(vsID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }



    // Compile Fragment Shader
    //printf("Compiling shader: %s\n", fragment_path);
    char const * FragmentSourcePointer = fsCode.c_str();
    glShaderSource(fsID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(fsID);

    // Check Fragment Shader
    glGetShaderiv(fsID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(fsID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }


    GLuint programID = glCreateProgram();
    glAttachShader(programID, vsID);
    glAttachShader(programID, fsID);
    glLinkProgram(programID);

    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ) {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    
    glDetachShader(programID, vsID);
    glDetachShader(programID, fsID);
    
    glDeleteShader(vsID);
    glDeleteShader(fsID);

    return programID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera.resize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.rotateMouse(xpos, ypos);
}

// glew and glfw initialization from tutorial02 (Zadanie 1) 
int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: ./swiat [directory] [additional options: -lon, -lat]\n");
		return 0;
	}

	// Initialise GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(camera.getMainWidth(), camera.getMainHeight(), "Wizualizacja terenu", NULL, NULL);
	if (window == NULL) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glViewport(0, 0, camera.getMainWidth(), camera.getMainHeight());
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	const char * directory;
	bool lonRestricted = false, latRestricted = false;
	int lonStart = -400, lonEnd = 400;
	int latStart = -400, latEnd = 400;

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-lon", 4) == 0) {
			lonRestricted = true;
			char dirl, dirp;
			sscanf(argv[++i], "%c%d", &dirl, &lonStart);
			sscanf(argv[++i], "%c%d", &dirp, &lonEnd);
			if (dirl == 'W' || dirl == 'W') {
				lonStart = -lonStart;
			}
			if (dirp == 'W' || dirp == 'W') {
				lonEnd = -lonEnd;
			}
		} else if (strncmp(argv[i], "-lat", 4) == 0) {
			latRestricted = true;
			char dirg, dird;
			sscanf(argv[++i], "%c%d", &dird, &latStart);
			sscanf(argv[++i], "%c%d", &dirg, &latEnd);
			if (dird == 'S' || dird == 'S') {
				latStart = -latStart;
			}
			if (dirg == 'S' || dirg == 'S') {
				latEnd = -latEnd;
			}
		} else {
			directory = argv[i];
		}
		
	}

	// bedizemy wczytywac do vectora;

    GLuint programIDflat = LoadShaders("flat.vs", "flat.fs");
    GLuint programIDsphere = LoadShaders("sphere.vs", "sphere.fs");
	GLuint programIDdefault = LoadShaders("default.vs", "default.fs");

	std::vector<GLfloat> data_points;
	std::vector<glm::vec2> loaded_coords;
	int not_loaded[90][180] = {};

    std::vector<GLuint> VAO;
	std::vector<GLuint> VBO;
	GLuint not_loaded_VAO;
	GLuint not_loaded_VBO;
	GLuint EBO;


	glGenVertexArrays(1, &not_loaded_VAO);
	glGenBuffers(1, &not_loaded_VBO);

	glBindVertexArray(not_loaded_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, not_loaded_VBO);


	GLfloat displacements[] = {0.0, 0.0,
							   2.0, 0.0,
							   2.0, 2.0,
							   0.0, 2.0,
							   0.0, 0.0,
							   2.0, 2.0};


	// buforujemy dane
	glBufferData(GL_ARRAY_BUFFER, sizeof(displacements) * sizeof(GLfloat), displacements, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	
    std::vector<GLuint> indices[6];

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		for (int i = 0; i < sizeof(indices); i++)
		indices[i].clear();

	// kazdy element
	GLuint a = 0, b = 1, c = 1201, d = 1202;

	while (d < 1201*1201) {
		for (int j = 0; j < 1200; j++) {
			indices[0].push_back(a);
			indices[0].push_back(b);
			indices[0].push_back(c);

			indices[0].push_back(c);
			indices[0].push_back(b);
			indices[0].push_back(d);
			a++;
			b++;
			c++;
			d++;
		}
		a++;
		b++;
		c++;
		d++;
	}

	// co drugi element
	
	a = 0;
	b = 2;
	c = 2402;
	d = 2404;

	while ( d < 1201*1201) {
		while (d % 1201 > c % 1201) {
			indices[1].push_back(a);
			indices[1].push_back(b);
			indices[1].push_back(c);

			indices[1].push_back(c);
			indices[1].push_back(b);
			indices[1].push_back(d);
			a += 2;
			b += 2;
			c += 2;
			d += 2;
		}
		a++;
		b++;
		c++;
		d++;
		a += 1201;
		b += 1201;
		c += 1201;
		d += 1201;
	}

	// co czwarty element

	a = 0;
	b = 4;
	c = 4804;
	d = 4808;

	while ( d < 1201*1201) {
		while (d % 1201 > c % 1201) {
			indices[2].push_back(a);
			indices[2].push_back(b);
			indices[2].push_back(c);

			indices[2].push_back(c);
			indices[2].push_back(b);
			indices[2].push_back(d);
			a += 4;
			b += 4;
			c += 4;
			d += 4;
		}
		a++;
		b++;
		c++;
		d++;

		a += 3603;
		b += 3603;
		c += 3603;
		d += 3603;
	}

	// co osmy element
	a = 0;
	b = 8;
	c = 9608;
	d = 9616;

	while ( d < 1201*1201) {
		while (d % 1201 > c % 1201) {
			indices[3].push_back(a);
			indices[3].push_back(b);
			indices[3].push_back(c);

			indices[3].push_back(c);
			indices[3].push_back(b);
			indices[3].push_back(d);
			a += 8;
			b += 8;
			c += 8;
			d += 8;
		}
		a++;
		b++;
		c++;
		d++;
		a += 8407;
		b += 8407;
		c += 8407;
		d += 8407;
	}

	// co szesnasty
	a = 0;
	b = 16;
	c = 19216;
	d = 19232;

	while ( d < 1201*1201) {
		while (d % 1201 > c % 1201) {
			indices[4].push_back(a);
			indices[4].push_back(b);
			indices[4].push_back(c);

			indices[4].push_back(c);
			indices[4].push_back(b);
			indices[4].push_back(d);
			a += 16;
			b += 16;
			c += 16;
			d += 16;
		}
		a += 1;
		b += 1;
		c += 1;
		d += 1;
		a += 18015;
		b += 18015;
		c += 18015;
		d += 18015;
	}

	// co trzydziesty drugi
	a = 0;
	b = 32;
	c = 38432;
	d = 38464;

	while ( d < 1201*1201) {
		while (d % 1201 > c % 1201) {
			indices[5].push_back(a);
			indices[5].push_back(b);
			indices[5].push_back(c);

			indices[5].push_back(c);
			indices[5].push_back(b);
			indices[5].push_back(d);
			a += 32;
			b += 32;
			c += 32;
			d += 32;
		}


		a += 17;
		b += 17;
		c += 17;
		d += 17;
		a += 37231;
		b += 37231;
		c += 37231;
		d += 37231;
	}


	

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[0].size() * sizeof(GLuint), indices[0].data(), GL_STATIC_DRAW);


	GLfloat lewo = 500, prawo = -500, gora = 500, dol = -500;

	for (const auto & entry : std::filesystem::directory_iterator(directory)) {
		
		std::ifstream file(entry.path().c_str(), std::ios::in | std::ios::binary);
		int thislon, thislat;
		char dirV, dirH;
		const char* filename = strchr(entry.path().c_str(), '/');
		sscanf(filename, "/%c%d%c%d.hgt", &dirV, &thislat, &dirH, &thislon);

		if (dirV == 'S' || dirV == 's')
			thislat = -thislat;
		if (dirH == 'W' || dirH =='w')
			thislon = -thislon;


		if (thislon < lonStart || thislon > lonEnd)
			continue;
		if (thislat < latStart || thislat > latEnd)
			continue;


		std::cout << entry.path() << std::endl;

		int16_t data[1201][1201];
		unsigned char buffer[2];
		for (int i = 0; i < 1201; i++) {
			for (int j = 0; j < 1201; j++) {
				file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
				data[i][j] = (buffer[0] << 8) | buffer[1];
			}
    	}
		for (int i = 0; i < 1201; i++) {
			for (int j = 0; j < 1201; j++) {
				int x, y;
				if (dirV == 'N' || dirV == 'n')
					y = 1200 - i;
				else 
					y = i;

				if (dirH == 'E' || dirH == 'e')
					x = j;
				else 
					x = 1200 - j;

				data_points.push_back((GLfloat)data[y][x]);
			}
    	}
		
		if (thislon < lewo)
			lewo = thislon;
		if (thislon+1 > prawo)
			prawo = thislon+1;
		if (thislat < gora)
			gora = thislat;
		if (thislat+1 > dol)
			dol = thislat+1;

		// dodajemy do danych

		loaded_coords.push_back(glm::vec2(thislat, thislon));
		not_loaded[(thislat+89)/2][(thislon+178)/2] = 1;


		// tworzymy vao i vbo
		GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// buforujemy dane
		glBufferData(GL_ARRAY_BUFFER, data_points.size() * sizeof(GLfloat), data_points.data(), GL_STATIC_DRAW);
    	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1*sizeof(GLfloat), (void*)0);
    	glEnableVertexAttribArray(0);

		VAO.push_back(vao);
		VBO.push_back(vbo);
		data_points.clear();
	}

	lewo = lewo * (M_PI / 180.0);
	prawo = prawo * (M_PI / 180.0);

	gora = gora * (M_PI / 180.0);
	dol = dol * (M_PI / 180.0);

	gora = log(abs(1/cos(gora) + tan(gora)));
	dol = log(abs(1/cos(dol) + tan(dol)));

	float szer = prawo - lewo;
	float wys = gora - dol;
	float cameraHeight = sqrt(szer*szer + wys*wys)*0.836025405;
	
	
	camera.setPosition((lewo+prawo)/2.0, (gora+dol)/2.0, cameraHeight);

	camera.setSecondaryPosition(radians(lewo+179), radians(dol+89), 9000);

	

	GLfloat deltaTime = 0.0f;
	GLfloat lastTime = glfwGetTime();
	GLfloat start = lastTime;
	GLfloat timecount = 0.0;
	int fpscount = 0;

	int detailLevel = 5;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);

	bool autoDetail = true;
	bool mode = true; // true - flat, false - sphere
	GLfloat speed = 0.005;

	do {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		timecount += deltaTime;
		fpscount++;
		if (timecount > 1.0) {
			timecount -= 1.0;
			printf("FPS %d detailLevel %d\n", fpscount, detailLevel+1);
			if (autoDetail) {
				if (fpscount < 10 && detailLevel < 5) {
					detailLevel++;
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
				}
				if (fpscount > 20 && detailLevel > 0) {
					detailLevel--;
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
				}
			}
			fpscount = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			mode = true;	
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			mode = false;		
		}

		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
			if (!autoDetail)
				printf("AutoDetail enabled\n");
			autoDetail = true;		
		}
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 0;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 1;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 2;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 3;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 4;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
			autoDetail = false;
			detailLevel = 5;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[detailLevel].size() * sizeof(GLuint), indices[detailLevel].data(), GL_STATIC_DRAW);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveForward(20.0*speed, deltaTime, mode);
			else
				camera.moveForward(speed, deltaTime, mode);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveBackward(20.0*speed, deltaTime, mode);
			else
				camera.moveBackward(speed, deltaTime, mode);
			
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveLeft(20.0*speed, deltaTime, mode);
			else
				camera.moveLeft(speed, deltaTime, mode);
			
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveRight(20.0*speed, deltaTime, mode);
			else
				camera.moveRight(speed, deltaTime, mode);
			
		}
		if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveUp(20.0*speed, deltaTime, mode);
			else
				camera.moveUp(speed, deltaTime, mode);

		}
		if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				camera.moveDown(20.0*speed, deltaTime, mode);
			else
				camera.moveDown(speed, deltaTime, mode);
			
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (mode) {	// mapa
			glUseProgram(programIDflat);
			glm::mat4 viewmat = camera.getMainView();
			int location = glGetUniformLocation(programIDflat, "view");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewmat));

			glm::mat4 projectionmat = camera.getMainProjection();
			location = glGetUniformLocation(programIDflat, "projection");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionmat));
			for (int i = 0; i < VAO.size(); i++) {
				glBindVertexArray(VAO[i]);

				glUniform1f(glGetUniformLocation(programIDflat, "lat"), loaded_coords[i].x);
				glUniform1f(glGetUniformLocation(programIDflat, "lon"), loaded_coords[i].y);

				glDrawElements(GL_TRIANGLES, indices[detailLevel].size(), GL_UNSIGNED_INT, (void*)0);
			}
		} else { // kula
			glm::mat4 viewmat = camera.getSecondaryView();
			glm::mat4 projectionmat = camera.getSecondaryProjection();

			glBindVertexArray(not_loaded_VAO);
			glUseProgram(programIDdefault);
			
			int location = glGetUniformLocation(programIDdefault, "view");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewmat));

			location = glGetUniformLocation(programIDdefault, "projection");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionmat));
			
			glEnable(GL_CULL_FACE);
			

			for (int i = 1; i < 89; i++) {
				for (int j = 0; j < 180; j++) {
					if (!not_loaded[i][j]) {
						glUniform1f(glGetUniformLocation(programIDdefault, "lat"), (GLfloat)i*2);
						glUniform1f(glGetUniformLocation(programIDdefault, "lon"), (GLfloat)j*2);

						//glUniform3f(glGetUniformLocation(programIDdefault, "col"), 0.0, 0.0, 0.0);

						//glDrawArrays(GL_TRIANGLES, 0, sizeof(displacements));

						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

						glDrawArrays(GL_TRIANGLES, 0, sizeof(displacements));
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
				}
			}
			glDisable(GL_CULL_FACE);
			glBindVertexArray(0);
			
			glUseProgram(programIDsphere);	
			
			location = glGetUniformLocation(programIDsphere, "view");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewmat));

			location = glGetUniformLocation(programIDsphere, "projection");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionmat));

			
			for (int i = 0; i < VAO.size(); i++) { // renderujemy to co mamy
				glBindVertexArray(VAO[i]);

				glUniform1f(glGetUniformLocation(programIDsphere, "lat"), loaded_coords[i].x+89);
				glUniform1f(glGetUniformLocation(programIDsphere, "lon"), loaded_coords[i].y+179);

				glDrawElements(GL_TRIANGLES, indices[detailLevel].size(), GL_UNSIGNED_INT, (void*)0);
			}
			glBindVertexArray(0);
			
	

		}

	GLenum er;



		while (er=glGetError())
			fprintf(stderr, "\nOpenGL ERROR: 0x%04x    =============%s=============\n", er, "main afterdraw");


		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);



	glfwTerminate();



	return 0;
}

