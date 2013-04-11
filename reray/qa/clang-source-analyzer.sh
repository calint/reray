srcdir=../src
bindir=.
src=$srcdir/*.cpp
bin=$bindir/reray
#cc=g++
scn=~/lab/checker-267/scan-build
cc=clang++
cmd="$scn $cc -o $bin $src\
	../glfw/lib/osx/libglfw.a \
	-framework cocoa \
	-framework opengl \
 	-framework iokit \
	-pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion \
	-std=gnu++11 \
	-Wfatal-errors"&&
echo $cmd&&
$cmd&>clang-source-analyzer.log&&
rm $bin&&
echo


# 	-I/usr/X11/include/ \
# 	-L/usr/X11/lib/ \
# 	-lpng \
