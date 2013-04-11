clang++ -o reray src/*.cpp \
	glfw/lib/osx/libglfw.a \
	-framework cocoa \
	-framework opengl \
 	-framework iokit \
	-pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion \
	-std=gnu++11 \
	-O3 \
	-Wfatal-errors &&


ls -la reray &&

./reray

# 	-I/usr/X11/include/ -L/usr/X11/lib/ -lpng \
