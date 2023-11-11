default: main

%: %.cpp
	g++ -std=c++17 -I. $< -o swiat Camera.cpp -lGLEW  -lGL -lglfw 
clean:
	rm swiat
