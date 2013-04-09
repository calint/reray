clang++ -o reray src/reray.cpp \
	glfw/lib/osx/libglfw.a \
	-framework cocoa \
	-framework opengl \
 	-framework iokit \
	-O0 -g3 \
	-pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion \
	-std=gnu++11 \
	-Wfatal-errors &&


ls -la reray &&

./reray
