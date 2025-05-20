EXECUTABLE = obj-viewer

CC = g++
FLAGS = --std=c++17 -Wall -Wextra -g

SOURCES = \
	main.cpp \
	ObjLoader.cpp \
	Model.cpp

$(EXECUTABLE):
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCES)

clean:
	rm -rf $(EXECUTABLE) *.dSYM
