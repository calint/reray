clang++ -o reray src/reray.cpp glfw/libglfw.a \
  -framework cocoa \
  -framework opengl \
  -framework iokit

ls -la reray

./reray
