#include<iostream>
using namespace std;
#include "../glfw/glfw.h"
#include<sys/time.h>

class tmr{
	struct timeval t0;
public:
	tmr(){restart();}
	void restart(){gettimeofday(&t0,NULL);}
	float dt(){
	    struct timeval tv;
	    gettimeofday(&tv,NULL);
		const time_t diff_s=tv.tv_sec-t0.tv_sec;
		const int diff_us=tv.tv_usec-t0.tv_usec;
		t0=tv;
		return (float)diff_s+diff_us/1000000.f;
	}
};


int main(){
	if(!glfwInit())return -1;
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(1024,512,8,8,8,8,32,0,GLFW_WINDOW))return -1;
	cout<<"  version: "<<glGetString(GL_VERSION)<<endl;
	glfwEnable(GLFW_STICKY_KEYS);
	tmr t;
	long long frm=0;
	glfwSwapInterval(0);
	if(glGetError()!=GL_NO_ERROR){cout<<"opengl in error state";return -1;}
	while(glfwGetWindowParam(GLFW_OPENED)){
		frm++;
//		glMatrixMode(GL_PROJECTION_MATRIX);
//		if(glGetError()!=GL_NO_ERROR){cout<<"opengl in error state 2";return -1;}
		cout<<t.dt()<<endl;
		glfwSwapBuffers();
	}
	cout<<frm/t.dt()<<endl;
	return 0;
}
