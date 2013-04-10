clang++ -o reray src/reray.cpp \
	glfw/lib/osx/libglfw.a \
	-framework cocoa \
	-framework opengl \
 	-framework iokit \
 	-I/usr/X11/include/ \
 	-L/usr/X11/lib/ \
 	-lpng \
	-pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion \
	-std=gnu++11 \
	-Wfatal-errors &&


ls -la reray &&

./reray
