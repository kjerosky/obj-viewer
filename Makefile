EXECUTABLE = obj-viewer

CC = g++
FLAGS = --std=c++17 -Wall -Wextra -g

INCLUDE_PATHS = -I /opt/homebrew/include
LIBRARY_PATHS = -L /opt/homebrew/lib
LIBRARIES = -lSDL3 -lGLEW -framework OpenGL

SOURCES = \
	main.cpp \
	ObjLoader.cpp \
	Model.cpp

$(EXECUTABLE):
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCES) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LIBRARIES)

clean:
	rm -rf $(EXECUTABLE) *.dSYM
