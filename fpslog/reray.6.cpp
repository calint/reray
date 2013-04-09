#include<iostream>
using namespace std;
#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
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

#include<sstream>
namespace dbox{
	namespace clk{
		int fps=120;
		int dtms=1000/fps;
		float dt=dtms/1000.f;
		clock_t t0=clock();
		clock_t t1=t0;
		long long tk=0;
		inline void timerrestart(){t1=clock();}
//		inline clock_t timerdclk(){return clock()-t1;}
		inline float timerdt(){return (float)(clock()-t1)/CLOCKS_PER_SEC;}
	}
	namespace metrics{
		int globs;
		int coldetsph;
		int frames;
		float dtupd;
		float dtrend;
		int p3s;
		int m3s;
		int collisions;
		int mwrfsh;
		int mpmul;
		int mmmul;
		int ngrids;
		float dtcoldetgrd;
		float dtcoldetbrute;
		float dtnet;
		int viewcull;
		int globsrend;
		int rays;
		float rayone;
		int gridsculled;
		int gridsrend;
	}
	inline float dt(const float f=1){return f*clk::dt;}
	//? gloxrnd
	inline float rnd(const float from,const float tonotincluding){return from+(tonotincluding-from)*rand()/RAND_MAX;}
	inline float rndo(const float tonotincluding){return tonotincluding*rand()/RAND_MAX;}
	inline float rndn(const float s){return rnd(-s,s);}
	const float pi=3.1415926f;
	const float degtoradp=pi/180;
	inline float degtorad(const float deg=1){return deg*degtoradp;}
	ostringstream sts;
}
using namespace dbox;


#define flf()l("···",__FILE__,__LINE__,__FUNCTION__);
static inline ostream&l(const char*s="",const char*file="",int lineno=0,const char*func=""){cerr<<file;if(lineno){cerr<<":"<<lineno;}cerr<<" "<<func<<"  "<<s;return cerr;}

#include<execinfo.h>
class signl{
	const int i;
	const char*s;
public:
	signl(const int i=0,const char*s="signal"):i(i),s(s){
		cerr<<" ••• signl "<<i<<" · "<<s<<endl;
		const int nva=10;
		void*va[nva];
		int n=backtrace(va,nva);
		backtrace_symbols_fd(va,n,1);
	}
	inline int num()const{return i;}
	inline const char* str()const{return s;}
};

#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
namespace net{
	const int nplayers=2;
	const int nkeys=32;
	const int keyslen=nplayers*nkeys;
	const char*host="127.0.0.1";
	const char*port="8085";
//	const char*playername="noname";
	bool sockio=false;

	char keys[nplayers][nkeys];
	int player=0;
	int sockfd=0;
	struct addrinfo*ai=0;
	void sendkeys(){
		const ssize_t bytes_sent=send(sockfd,keys[player],nkeys,0);
		//flf();l("sent keys for player ")<<player<<"  bytessent("<<bytes_sent<<endl;
		if(bytes_sent==-1){flf();l(strerror(errno))<<endl;throw signl(1,"sendkeys");}
	}
	void print(){
		cout<<hex;
		for(int i=0;i<nplayers;i++){
			cout<<"k["<<i<<"](";
			for(int j=0;j<nkeys;j++){
				if(j>0)cout<<" ";
				cout<<int(keys[i][j]);
			}
			cout<<")"<<endl;
		}

	}
	void reckeys(){
		const ssize_t reclen=recv(sockfd,keys,keyslen,0);
		if(reclen==0){flf();l("closed")<<endl;exit(1);}
		if(reclen==-1){flf();l(strerror(errno))<<endl;exit(2);}
		if(reclen!=keyslen)throw signl(3,"uncompleterec");//?
//		print();
	}
	void open(){
		flf();l()<<"connect "<<host<<":"<<port<<endl;

		struct addrinfo hints;
		memset(&hints,0,sizeof hints);
		hints.ai_family=AF_UNSPEC;
		hints.ai_socktype=SOCK_STREAM;
		if(getaddrinfo(host,port,&hints,&ai)){flf();l(strerror(errno))<<endl;throw signl();}
		sockfd=socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol);
		if(sockfd==-1){flf();l(strerror(errno))<<endl;throw signl();}
	//	flf();l()<<"socket "<<sockfd<<"  errno("<<errno<<")"<<endl;
		if(connect(sockfd,ai->ai_addr,ai->ai_addrlen)){flf();l(strerror(errno))<<endl;throw signl();}
		flf();l("connected")<<endl;
		const char c[]="0123456789abcdef";
		char playerid[33];
		srand((unsigned int)time(NULL));
		for(int i=0;i<nkeys;i++){
			playerid[i]=c[(int)rand()%16];
		}
		playerid[32]=0;
		flf();cout<<"  keys:"<<sizeof keys<<"\n";
		flf();cout<<"  player id: "<<playerid<<"\n";
		memset(keys,0,sizeof keys);
		if(!sockio){
			strncpy(keys[player],playerid,nkeys);
		}else{//?
			string s="get /gloxnet .\r\ncookie:i=";
			s.append(playerid).append("\r\n\r\n");
			const char*sc=s.c_str();
			flf();l(sc)<<endl;
			const size_t sclen=s.length();
			flf();l()<<sclen<<endl;
			const ssize_t bytes_sent=send(sockfd,sc,(size_t)sclen,0);
			if(bytes_sent!=(signed)sclen){flf();l(strerror(errno))<<endl;throw signl(1,"sockio");}
//			const ssize_t bytes_sent2=send(sockfd,sc,(size_t)sclen,0);
//			if(bytes_sent2!=(signed)sclen){flf();l(strerror(errno))<<endl;throw signl(2,"sockio");}
		}
		sendkeys();

		flf();l("waiting for other players.")<<endl;
		reckeys();
		flf();l("all players connected.")<<endl;
		int i=0;
		for(auto s:keys){
			if(!strcmp(playerid,s)){
				player=i;
				break;
			}
			i++;
		}
		flf();l("u r player #")<<player<<endl;
		print();
		memset(keys,0,sizeof keys);
	}
//	void stop(){
//		if(sockfd&&close(sockfd)){flf();l(strerror(errno))<<endl;}
//		if(ai)freeaddrinfo(ai);
//	}
}

namespace shader{
	GLuint prog=0;
	GLint umxmw=0;
	GLint umxwv=0;
	GLint udopersp=0;
	GLint urendzbuf=0;
	GLint utx=0;
	void init(){
		const GLuint vtxshdr=glCreateShader(GL_VERTEX_SHADER);
//		const GLchar*vtxshdrsrc[]={"void main(){gl_TexCoord[0]=gl_TextureMatrix[0]*gl_ModelViewMatrix*gl_Vertex;gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;}"};
//		const GLchar*vtxshdrsrc[]={"varying vec3 vnml;void main(){gl_TexCoord[0]=gl_TextureMatrix[0]*gl_ModelViewMatrix*gl_Vertex;gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;vnml=normalize(gl_NormalMatrix*gl_Normal);}"};
//		const GLchar*vtxshdrsrc[]={"void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;gl_TexCoord[1]=gl_Vertex;}"};
//		const GLchar*vtxshdrsrc[]={"void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;gl_TexCoord[1]=gl_MultiTexCoord1;}"};
//		const GLchar*vtxshdrsrc[]={"void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;gl_TexCoord[1]=gl_MultiTexCoord1;gl_TexCoord[2]=gl_TextureMatrix[2]*gl_ModelViewMatrix*gl_Vertex;}"};
//		const GLchar*vtxshdrsrc[]={"varying vec3 vnml;void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_FrontColor=gl_Color;gl_TexCoord[1]=gl_MultiTexCoord1;gl_TexCoord[2]=gl_TextureMatrix[2]*gl_ModelViewMatrix*gl_Vertex;vnml=normalize(gl_NormalMatrix*gl_Normal);}"};
//		const GLchar*vtxshdrsrc[]={"#version 150 core\nuniform mat4 umxmw;uniform mat4 umxwv;uniform bool udopersp=true;in vec3 in_pos;in vec4 in_rgba;in vec2 in_txc;out vec4 rgba;out vec2 txcoord;void main(){rgba=in_rgba;txcoord=in_txc;vec4 pw=umxmw*vec4(in_pos,1);vec4 pv=umxwv*pw;if(udopersp){pv.w=-pv.z;pv.z*=pv.z;}else{pv.z=-pv.z;}gl_Position=pv;}"};
		const GLchar*vtxshdrsrc[]={"#version 150 core\nuniform mat4 umxmw;uniform mat4 umxwv;uniform bool udopersp=true;in vec3 in_pos;in vec4 in_rgba;in vec2 in_txc;out vec4 rgba;out vec2 txcoord;void main(){rgba=in_rgba;txcoord=in_txc;vec4 pw=umxmw*vec4(in_pos,1);vec4 pv=umxwv*pw;if(udopersp){pv.w=-pv.z;pv.z*=pv.z;}else{pv.z=-pv.z;}gl_Position=pv;}"};
//		const GLchar*vtxshdrsrc[]={"#version 150 core\nuniform mat4 umxmw;uniform mat4 umxwv;uniform bool udopersp=true;in vec3 in_pos;in vec4 in_rgba;in vec2 in_txc;out vec4 rgba;out vec2 txcoord;void main(){rgba=in_rgba;txcoord=in_txc;vec4 pw=umxmw*vec4(in_pos,1);vec4 pv=umxwv*pw;if(udopersp){pv.w=-pv.z;pv.z*=pv.z;}else{pv.z=-pv.z;}gl_Position=vec4(in_pos,1);}"};
		const GLint vtxshdrsrclen[]={GLint(strlen(vtxshdrsrc[0]))};
		glShaderSource(vtxshdr,1,vtxshdrsrc,vtxshdrsrclen);
		glCompileShader(vtxshdr);
		GLint sts;
		glGetShaderiv(vtxshdr,GL_COMPILE_STATUS,&sts);
		const int n=4*1024;
		char buf[n];
		GLsizei nchs;
		if(!sts){
			glGetShaderInfoLog(vtxshdr,n,&nchs,buf);
			cerr<<"vertex shader did not compile"<<endl<<buf;
			throw signl();
		}
		const GLuint frgshdr=glCreateShader(GL_FRAGMENT_SHADER);
//		const GLchar*frgshdrsrc[]={"uniform sampler2D ushad;void main(){vec4 shad;shad=texture2DProj(ushad,gl_TexCoord[0]);float la=shad.z<gl_TexCoord[0].z/gl_TexCoord[0].w?.5:.7;gl_FragColor=la*gl_Color;}"};
//		const GLchar*frgshdrsrc[]={"varying vec3 vnml;uniform sampler2D ushadow0;void main(){vec4 shado;shado=texture2DProj(ushadow0,gl_TexCoord[0]);float la=shado.z<gl_TexCoord[0].z/gl_TexCoord[0].w?0.:.2;float wa=gl_FragCoord.w;vec3 lhta=vec3(1,0,0);float ln=dot(vnml,lhta);gl_FragColor=clamp(la*.5+wa*.5+ln*.2,0.,1.)*gl_Color;}"};
//		const GLchar*frgshdrsrc[]={"uniform sampler2D utex;void main(){vec4 tex;tex=texture2D(utex,gl_TexCoord[1].st);gl_FragColor=tex;}"};
//		const GLchar*frgshdrsrc[]={"uniform sampler2D utex;uniform sampler2D ushad;void main(){vec4 tex;tex=texture2D(utex,gl_TexCoord[1].st);vec4 shad;shad=texture2DProj(ushad,gl_TexCoord[2]);float la=shad.z<gl_TexCoord[2].z/gl_TexCoord[2].w?-.2:0.;gl_FragColor=la*vec4(1,1,1,1)+tex+gl_Color;}"};
//		const GLchar*frgshdrsrc[]={"uniform sampler2D ushad;uniform sampler2D utex;varying vec3 vnml;void main(){vec4 tex;tex=texture2D(utex,gl_TexCoord[1].st);vec4 shad;shad=texture2DProj(ushad,gl_TexCoord[2]);float la=shad.z<gl_TexCoord[2].z/gl_TexCoord[2].w?.5:1.;vec3 lht=vec3(1,0,0);float ln=dot(normalize(vnml),lht);gl_FragColor=la*(tex+ln+gl_Color);}"};
//		const GLchar*frgshdrsrc[]={"uniform sampler2D ushad;uniform sampler2D utex;varying vec3 vnml;void main(){vec4 tex;tex=texture2D(utex,gl_TexCoord[1].st);vec4 shad;shad=texture2DProj(ushad,gl_TexCoord[2]);float la=shad.z<gl_TexCoord[2].z/gl_TexCoord[2].w?.5:1.;vec3 lht=vec3(1,0,0);float ln=dot(normalize(vnml),lht);float wa=gl_FragCoord.w;gl_FragColor=la*(ln*.2+wa*.5+tex+gl_Color);}"};
		const GLchar*frgshdrsrc[]={"#version 150 core\nuniform sampler2D utx;in vec4 rgba;in vec2 txcoord;out vec4 out_Color;void main(){out_Color=rgba;}"};
		const GLint frgshdrsrclen[]={GLint(strlen(frgshdrsrc[0]))};
		glShaderSource(frgshdr,1,frgshdrsrc,frgshdrsrclen);
		glCompileShader(frgshdr);
		glGetShaderiv(frgshdr,GL_COMPILE_STATUS,&sts);
		if(!sts){
			glGetShaderInfoLog(frgshdr,n,&nchs,buf);
			cerr<<"frag shader did not compile"<<endl<<buf<<endl;
			throw signl();
		}
		prog=glCreateProgram();
		glAttachShader(prog,vtxshdr);
		glAttachShader(prog,frgshdr);
		glLinkProgram(prog);
		glGetProgramiv(prog,GL_LINK_STATUS,&sts);
		if(!sts){
			glGetProgramInfoLog(prog,n,&nchs,buf);
			cerr<<"program did not link"<<endl<<buf<<endl;
			throw signl();
		}
		if((umxmw=glGetUniformLocation(prog,"umxmw"))==-1)throw signl(0,"umxmw not found");
//		if((umxwv=glGetUniformLocation(prog,"umxwv"))==-1)throw signl(0,"umxwv not found");
//		if((udopersp=glGetUniformLocation(prog,"udopersp"))==-1)throw signl(0,"udopersp not found");
//		if((urendzbuf=glGetUniformLocation(prog,"urendzbuf"))==-1)throw signl(0,"urendzbuf not found");
//		if((utx=glGetUniformLocation(prog,"utx"))==-1)throw signl(0,"utx not found");
		glUseProgram(prog);

		if(glGetError())throw signl(0,"shader::init");
	}
};

#include<cmath>

class p3{
	float x,y,z;
public:
	inline p3():x(0),y(0),z(0){metrics::p3s++;}
	inline p3(const p3&p){metrics::p3s++;x=p.x;y=p.y;z=p.z;}
	inline p3(const float x,const float y,const float z):x(x),y(y),z(z){metrics::p3s++;}
	inline p3(const p3&from,const p3&to):x(to.x-from.x),y(to.y-from.y),z(to.z-from.z){metrics::p3s++;}
	inline p3(const p3&from,const p3&to,const float len):x(to.x-from.x),y(to.y-from.y),z(to.z-from.z){metrics::p3s++;norm(len);}
	inline ~p3(){metrics::p3s--;}
	inline float getx()const{return x;}
	inline p3&setx(const float f){x=f;return*this;}
	inline float gety()const{return y;}
	inline p3&sety(const float f){y=f;return*this;}
	inline float getz()const{return z;}
	inline p3&transl(const float dx,const float dy,const float dz){x+=dx;y+=dy;z+=dz;return*this;}
	inline p3&transl(const p3&d){x+=d.x;y+=d.y;z+=d.z;return*this;}
	inline p3&transl(const p3&d,const float dt){x+=d.x*dt;y+=d.y*dt;z+=d.z*dt;return*this;}
	inline float magn()const{return sqrt(x*x+y*y+z*z);}
	inline float magn2()const{return x*x+y*y+z*z;}
	inline p3&set(const p3&p){x=p.x;y=p.y;z=p.z;return*this;}
	inline p3&set(const float x,const float y,const float z){this->x=x;this->y=y;this->z=z;return*this;}
	inline p3&neg(){x=-x;y=-y;z=-z;return*this;}
	inline p3&negy(){y=-y;return*this;}
	inline p3&scale(const float s){x*=s;y*=s;z*=s;return*this;}
	inline p3&scale(const float sx,const float sy,const float sz){x*=sx;y*=sy;z*=sz;return*this;}
	inline bool operator==(const p3&p)const{return x==p.x&&y==p.y&&z==p.z;}
	inline float dot(const p3&p)const{return x*p.x+y*p.y+z*p.z;}
	inline p3&vecprod(const p3&v1,const p3&v2){x=v1.y*v2.z-v1.z*v2.y;y=v1.z*v2.x-v1.x*v2.z;z=v1.x*v2.y-v1.y*v2.x;return*this;}
	inline p3&pow2(){x*=x;y*=y;z*=z;return*this;}
	inline float sum()const{return x+y+z;}
	inline p3&mult(const p3&p){x*=p.x;y*=p.y;z*=p.z;return*this;}
	inline p3&clr(){x=y=z=0;return*this;}
	p3&norm(const float length=1){
		float q=sqrt(x*x+y*y+z*z);
		if(q==0){
			x=y=z=0;
			return*this;
		}
		const float t=length/q;
		x=t*x;y=t*y;z=t*z;
		return*this;
	}
	friend ostream&operator<<(ostream&,const p3&);
	friend istream&operator>>(istream&,p3&);
};
inline ostream&operator<<(ostream&os,const p3&p){os<<p.x<<" "<<p.y<<" "<<p.z;return os;}
inline istream&operator>>(istream&is,p3&p){is>>p.x;is.ignore();is>>p.y;is.ignore();is>>p.z;return is;}

class m3{
public:
	void togl(GLfloat m[])const{
		 m[0]=xx; m[1]=xy; m[2]=xz; m[3]=xo;
		 m[4]=yx; m[5]=yy; m[6]=yz; m[7]=yo;
		 m[8]=zx; m[9]=zy;m[10]=zz;m[11]=zo;
		m[12]=ox;m[13]=oy;m[14]=oz;m[15]=oo;
	}
	inline m3(){metrics::m3s++;ident();}
	inline m3(const p3&p,const p3&a){metrics::m3s++;mw(p,a);}
	inline m3(const GLfloat*m){metrics::m3s++;set(m);}
	inline ~m3(){metrics::m3s--;}
	inline p3 xaxis()const{return p3(xx,xy,xz);}
	inline p3 yaxis()const{return p3(yx,yy,yz);}
	inline p3 zaxis()const{return p3(zx,zy,zz);}
	inline m3&xplane(GLfloat*v){v[0]=xx;v[1]=yx;v[2]=zx;v[3]=ox;return*this;}
	inline m3&yplane(GLfloat*v){v[0]=xy;v[1]=yy;v[2]=zy;v[3]=oy;return*this;}
	inline m3&zplane(GLfloat*v){v[0]=xz;v[1]=yz;v[2]=zz;v[3]=oz;return*this;}
	inline m3&wplane(GLfloat*v){v[0]=xo;v[1]=yo;v[2]=zo;v[3]=oo;return*this;}
	m3&ident(){xx=1;xy=0;xz=0;xo=0; yx=0;yy=1;yz=0;yo=0; zx=0;zy=0;zz=1;zo=0; ox=oy=oz=0; oo=1;return*this;}
	m3&mw(const p3&p,const p3&a){//? Mszxyt
		ident();
		roty(degtorad(a.gety()));
		rotx(degtorad(a.getx()));
		rotz(degtorad(a.getz()));
		ox=p.getx();
		oy=p.gety();
		oz=p.getz();
		return*this;
	}
	const m3&trnsf(const p3&src,p3&dst)const{
		metrics::mpmul++;
		const float x=src.getx();
		const float y=src.gety();
		const float z=src.getz();
		const float nx=x*xx+y*yx+z*zx+ox;
		const float ny=x*xy+y*yy+z*zy+oy;
		const float nz=x*xz+y*yz+z*zz+oz;
		dst.set(nx,ny,nz);
		return*this;
	}
	m3&set(const GLfloat m[16]){
		xx=m[ 0];yx=m[ 4];zx=m[ 8];ox=m[12];
		xy=m[ 1];yy=m[ 5];zy=m[ 9];oy=m[13];
		xz=m[ 2];yz=m[ 6];zz=m[10];oz=m[14];
		xo=m[ 3];yo=m[ 7];zo=m[11];oo=m[15];
		return*this;
	}
	m3&mul(const m3&m){
		metrics::mmmul++;
		const float nxx=m.xx*xx+m.yx*xy+m.zx*xz+m.ox*xo;
		const float nyx=m.xx*yx+m.yx*yy+m.zx*yz+m.ox*yo;
		const float nzx=m.xx*zx+m.yx*zy+m.zx*zz+m.ox*zo;
		const float nox=m.xx*ox+m.yx*oy+m.zx*oz+m.ox*oo;

		const float nxy=m.xy*xx+m.yy*xy+m.zy*xz+m.oy*xo;
		const float nyy=m.xy*yx+m.yy*yy+m.zy*yz+m.oy*yo;
		const float nzy=m.xy*zx+m.yy*zy+m.zy*zz+m.oy*zo;
		const float noy=m.xy*ox+m.yy*oy+m.zy*oz+m.oy*oo;

		const float nxz=m.xz*xx+m.yz*xy+m.zz*xz+m.oz*xo;
		const float nyz=m.xz*yx+m.yz*yy+m.zz*yz+m.oz*yo;
		const float nzz=m.xz*zx+m.yz*zy+m.zz*zz+m.oz*zo;
		const float noz=m.xz*ox+m.yz*oy+m.zz*oz+m.oz*oo;

		const float nxo=m.xo*xx+m.yo*xy+m.zo*xz+m.oo*xo;
		const float nyo=m.xo*yx+m.yo*yy+m.zo*yz+m.oo*yo;
		const float nzo=m.xo*zx+m.yo*zy+m.zo*zz+m.oo*zo;
		const float noo=m.xo*ox+m.yo*oy+m.zo*oz+m.oo*oo;

		xx=nxx;yx=nyx;zx=nzx;ox=nox;
		xy=nxy;yy=nyy;zy=nzy;oy=noy;
		xz=nxz;yz=nyz;zz=nzz;oz=noz;
		xo=nxo;yo=nyo;zo=nzo;oo=noo;

		return*this;
	}
	friend ostream&operator<<(ostream&,const m3&);
	friend istream&operator>>(istream&,m3&);
private:
	float xx,yx,zx,ox;
	float xy,yy,zy,oy;
	float xz,yz,zz,oz;
	float xo,yo,zo,oo;
	m3&rotx(const float a){
		const float c=cos(a),s=sin(a);
		const float nyx=yx*c+zx*s,nyy=yy*c+zy*s,nyz=yz*c+zz*s,nyo=yo*c+zo*s;
		const float nzx=zx*c-yx*s,nzy=zy*c-yy*s,nzz=zz*c-yz*s,nzo=zo*c-yo*s;
		yx=nyx;yy=nyy;yz=nyz;yo=nyo;
		zx=nzx;zy=nzy;zz=nzz;zo=nzo;
		return*this;
	}
	m3&roty(const float a){
		const float c=cos(a),s=sin(a);
		const float nxx=xx*c-zx*s,nxy=xy*c-zy*s,nxz=xz*c-zz*s,nxo=xo*c-zo*s;
		const float nzx=zx*c+xx*s,nzy=zy*c+xy*s,nzz=zz*c+xz*s,nzo=zo*c+xo*s;
		xx=nxx;xy=nxy;xz=nxz;xo=nxo;
		zx=nzx;zy=nzy;zz=nzz;zo=nzo;
		return*this;
	}
	m3&rotz(const float a){
		const float c=cos(a),s=sin(a);
		const float nxx=xx*c+yx*s,nxy=xy*c+yy*s,nxz=xz*c+yz*s,nxo=xo*c+yo*s;
		const float nyx=yx*c-xx*s,nyy=yy*c-xy*s,nyz=yz*c-xz*s,nyo=yo*c-xo*s;
		xx=nxx;xy=nxy;xz=nxz;xo=nxo;
		yx=nyx;yy=nyy;yz=nyz;yo=nyo;
		return*this;
	}
};
ostream&operator<<(ostream&os,const m3&m){
	cout<<"["<<p3(m.xx,m.yx,m.zx)<<" "<<m.ox<<"] [";
	cout<<p3(m.xy,m.yy,m.zy)<<" "<<m.oy<<"] [";
	cout<<p3(m.xz,m.yz,m.zz)<<" "<<m.oz<<"] [";
	cout<<p3(m.xo,m.yo,m.zo)<<" "<<m.oo<<"]";
	return os;
}
istream&operator>>(istream&is,m3&m){
	p3 p;
	is.ignore();
	is>>p;m.xx=p.getx();m.yx=p.gety();m.zx=p.getz();is.ignore();is>>m.ox;is.ignore(3);
	is>>p;m.xy=p.getx();m.yy=p.gety();m.zy=p.getz();is.ignore();is>>m.oy;is.ignore(3);
	is>>p;m.xz=p.getx();m.yz=p.gety();m.zz=p.getz();is.ignore();is>>m.oz;is.ignore(3);
	is>>p;m.xo=p.getx();m.yo=p.gety();m.zo=p.getz();is.ignore();is>>m.oo;is.ignore();
	return is;
}
class p3n:public p3{
	p3 n;
public:
	p3n(const p3&p,const p3&n):
		p3(p),
		n(n)
	{}
	float dist(const p3&p)const{
		const p3 v(*this,p);
		return n.dot(v);
	}
};

class bvol{
	const int count;
	const p3n*pns;
public:
	bvol(const int count,const p3n p[]):
		count(count),
		pns(p)
	{}
	int cull(const p3&p,const float r)const{
		for(int i=0;i<count;i++){
			const p3n&pp=pns[i];
			//const p3 v(pp,*this);
			//const float t=v.dot(pp.n);
			const float t=pp.dist(p);
			if(t>0){// infront
				if(t>r){
					return 1;
				}
			}
		}
		return 0;
	}
};


#include <list>
class vbo{
	GLuint glva;//vertex array
	GLuint glvib;//indices array
	GLuint glvb;
	GLsizei nind;
	int elemtype;
public:
	vbo():glva(0),glvib(0),glvb(0),nind(0),elemtype(0){}
	virtual~vbo(){}
	virtual int nvertices(){return 4;}
	virtual void vertices(float fa[])const{
		const float w=1;
		int c=0;
		//0
		fa[c++]=-w;fa[c++]=w;fa[c++]=0;//xyz
		fa[c++]= 1;fa[c++]=0;fa[c++]=0;fa[c++]=1;//rgba
		fa[c++]= 0;fa[c++]=0;//st
//		//1
		fa[c++]=-w;fa[c++]=-w;fa[c++]=0;//xyz
		fa[c++]= 0;fa[c++]=1;fa[c++]=0;fa[c++]=1;//rgba
		fa[c++]= 0;fa[c++]=1;//st
//		//2
		fa[c++]= w;fa[c++]=-w;fa[c++]=0;//xyz
		fa[c++]= 0;fa[c++]=0;fa[c++]=1;fa[c++]=1;//rgba
		fa[c++]= 1;fa[c++]=1;//st
//		//3
		fa[c++]= w;fa[c++]= w;fa[c++]=0;//xyz
		fa[c++]= 1;fa[c++]=1;fa[c++]=1;fa[c++]=1;//rgba
		fa[c++]= 1;fa[c++]=0;//st
	}
	virtual GLsizei nindices(){return 6;}
	virtual void indices(char ba[])const{
		int c=0;
		ba[c++]=0;ba[c++]=1;ba[c++]=2;
		ba[c++]=2;ba[c++]=3;ba[c++]=0;
	}
	void glload(){
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		const int stride=9;//xyz,rgba,st
		const int sizeofnum=sizeof(float);//sizeof(float)
		const int stridebytes=stride*sizeofnum;

		const int nv=nvertices();
		cout<<"  "<<nv<<" vertices, "<<stridebytes<<" B/vertex"<<endl;
		float*vb=new float[nv*stride];
		vertices(vb);
		glGenVertexArrays(1,&glva);
		glBindVertexArray(glva);
		glGenBuffers(1,&glvb);
		glBindBuffer(GL_ARRAY_BUFFER,glvb);
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		glBufferData(GL_ARRAY_BUFFER,nv*stridebytes,vb,GL_STATIC_DRAW);

		const GLenum err=glGetError();
		if(err==GL_INVALID_ENUM)cout<<"opengl error: GL_INVALID_ENUM"<<endl;
		else if(err==GL_INVALID_VALUE)cout<<"opengl error: GL_INVALID_VALUE"<<endl;
		else if(err==GL_INVALID_OPERATION)cout<<"opengl error: GL_INVALID_OPERATION"<<endl;
		else if(err==GL_OUT_OF_MEMORY)cout<<"opengl error: GL_OUT_OF_MEMORY"<<endl;

		//		glBufferData(GL_ARRAY_BUFFER,nv*stridebytes,vb,GL_STATIC_DRAW);
		if(err!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		glVertexAttribPointer(0,3,GL_FLOAT,false,stridebytes,0);// positions
		glVertexAttribPointer(1,4,GL_FLOAT,false,stridebytes,(const GLvoid*)(3*sizeofnum));// colors, 16 bytes offset
		glVertexAttribPointer(2,2,GL_FLOAT,false,stridebytes,(const GLvoid*)(7*sizeofnum));// texture, 32 bytes offset
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		delete vb;

		nind=nindices();
		cout<<"  "<<nind<<" indices, 1 B/index"<<endl;
		char*ib=new char[nind];
		indices(ib);
		glGenBuffers(1,&glvib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glvib);
		const GLsizeiptr n=(GLsizeiptr)(nind*(int)sizeof(char));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,n,ib,GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		delete ib;
//		final String imgpath=imgpath();
//		if(imgpath!=null){
//			System.out.println("  texture "+imgpath);
//			tx=loadtexture(imgpath,null,0,0);
//		}else{
//			final int[]txsize=imgsize();
//			if(txsize!=null){
//				final int wi=txsize[0],hi=txsize[1],bpp=txsize[2];
//				final int n=wi*hi*bpp;
//				System.out.println("  texture alloc "+wi+"x"+hi+"x"+bpp*8+" bpp");
//				final ByteBuffer txdata=ByteBuffer.allocateDirect(n);
//				System.out.println("  texture generate");
//				imggen(txdata);
//				txdata.flip();
//				System.out.println("  texture load");
//				tx=loadtexture(null,txdata,txsize[0],txsize[1]);
//			}
//		}
//		System.out.println();

	}
	void gldraw()const{
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");
		glBindVertexArray(glva);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");

//		if(tx!=null){
//			glBindTexture(GL_TEXTURE_2D,tx.id);
//			glEnableVertexAttribArray(2);
//			glUniform1i(shader.utx,0);
//		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glvib);
		if(elemtype==0){
			glDrawElements(GL_TRIANGLES,nind,GL_UNSIGNED_BYTE,0);
		}else if(elemtype==1){
			glDrawElements(GL_TRIANGLE_FAN,nind,GL_UNSIGNED_BYTE,0);
		}else if(elemtype==2){
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDrawElements(GL_TRIANGLES,nind,GL_UNSIGNED_BYTE,0);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}else if(elemtype==3){
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glDisableVertexAttribArray(2);
//			glDisableVertexAttribArray(1);
			glDrawElements(GL_TRIANGLE_FAN,nind,GL_UNSIGNED_BYTE,0);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}else if(elemtype==4){
			glDrawElements(GL_TRIANGLE_FAN,nind,GL_UNSIGNED_BYTE,0);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glPolygonOffset(-.1f,.1f);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDrawElements(GL_TRIANGLE_FAN,nind,GL_UNSIGNED_BYTE,0);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}else throw signl(0,"unknown elemtype");
		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error state");

	}
};

class glob:public p3{
	const int id;
	glob&g;
	p3 a;
	list<glob*>chs;
	list<glob*>chsrm;
	list<glob*>chsadd;
	int bits;
	m3 mxmw;
	p3 mxmwpos;
	p3 mxmwagl;
	bool rmed;
	float r;
	p3 dd;
	float bf;
	float m=1;
	long long tk;
	long long culldrawtk;
protected:
	p3 d;
	p3 da;
	p3 f;
	p3 fi;
	p3 pp;
	bool ppsaved;
	inline const list<glob*>getchs()const{return chs;}
	vbo*vb;
public:
	p3 np;
	p3 nd;
	static bool drawboundingspheres;
	static int drawboundingspheresdetail;

	glob&pos(const p3&coord,const p3&agl){set(coord);a.set(agl);return*this;}
	glob&setvbo(vbo&v){vb=&v;return*this;}
	const m3&getmxmw(){refreshmxmw();return mxmw;}
	glob&dpos(const p3&coord,const p3&agl){d.set(coord);da.set(agl);return*this;}

	glob(glob&g,const p3&p=p3(),const p3&a=p3(),const float r=1,const float density_gcm3=1,const float bounciness=.5f):
		p3(p),id(metrics::globs++),g(g),a(a),bits(1),rmed(false),
		 r(r),bf(bounciness),m(density_gcm3*4/3*pi*r*r*r),
		 tk(0),culldrawtk(0),d(p3()),da(p3()),f(p3()),fi(p3()),pp(p),ppsaved(false),
		 vb(0),
		 np(p),nd(d)
	{
		if(&g==0)return;
		g.chsadd.push_back(this);
	}
	virtual~glob(){
		metrics::globs--;
		for(auto g:chs)
			delete g;
		chs.clear();
	}
	void rm(){
		if(rmed){
//			flf();l("rmingarmedobj")<<endl;
			return;
		}
		rmed=true;g.chsrm.push_back(this);
	}
	void coldet(glob&o){
		const p3 wpthis=g.posinwcs(*this);
		const p3 wpo=o.g.posinwcs(o);
		const p3 v(wpthis,wpo);
		const float d=v.magn();//? magn2
		const float rr=radius()+o.radius();
		metrics::coldetsph++;
//		flf();l()<<typeid(*this).name()<<"("<<wpthis<<")  "<<typeid(o).name()<<"("<<wpo<<")  "<<d<<"  "<<bv.r<<"   "<<o.bv.r<<endl;
		if(d>=rr){
			if(o.iscoldetrec()){
				for(auto gg:o.chs)
					coldet(*gg);
				return;
			}
			return;
		}
		if(issolid()&&o.issolid()){
			oncol(o);
			o.oncol(*this);
			return;
		}
		if(issolid()&&!o.issolid()){
			for(auto gg:o.chs)
				coldet(*gg);
			return;
		}
		if(!issolid()&&o.issolid()){
			for(auto gg:chs)
				o.coldet(*gg);
			return;
		}
	}
	void culldraw(const bvol&bv){
		if(culldrawtk==clk::tk){
//			flf();l("double rend");
			return;
		}
		culldrawtk=clk::tk;

		const float r=radius();
		const int cull=bv.cull(*this,r);
		if(cull){
			metrics::viewcull++;
			return;
		}
		metrics::globsrend++;
//		flf();l("included")<<endl;
//		glTranslatef(getx(),gety(),getz());
//		glRotatef(a.gety(),0,1,0);
//		glRotatef(a.getx(),1,0,0);
//		glRotatef(a.getz(),0,0,1);
		if(drawboundingspheres)drawboundingsphere();
		gldraw();
		for(auto g:chs){
//			glPushMatrix();
			g->culldraw(bv);
//			glPopMatrix();
		}//? coordsyschange
	}
	void dotck(){
		if(tk==clk::tk){
			flf();l("same tk");
			return;
		}
		tk=clk::tk;
		tick();
	}
	virtual void tick(){
		d.set(nd);
		set(np);
		chs.splice(chs.end(),chsadd);
		for(auto g:chs)g->dotck();
		for(auto g:chsrm){chs.remove(g);delete g;}
		chsrm.clear();
		if(!ppsaved){
			pp.set(*this);
			ppsaved=false;//?
		}
//		flf();l()<<"f("<<f<<") fi("<<fi<<") m("<<m<<") dd("<<dd<<") d("<<d<<") ("<<*this<<") dt("<<dt()<<") "<<endl;
		dd=p3(f).transl(fi).scale(1/m).scale(dt());
		fi.clr();
		d.transl(dd);
		transl(d);
		a.transl(da,dt());
		np.set(*this);
		nd.set(d);
	}
	virtual void gldraw(){
		if(vb){
			GLfloat mx[16];
			getmxmw().togl(mx);
			glUniformMatrix4fv(shader::umxmw,1,false,mx);
			vb->gldraw();
		}
		for(auto g:chs)g->gldraw();
	};
	inline p3&agl(){return a;}//?
	inline p3&getd(){return d;}//?
	inline float mass()const{return m;}
	inline glob&parent()const{return g;}
	inline int getid()const{return id;}
	inline const list<glob*>chls()const{return chs;}
	inline float radius()const{return r;}
	inline glob&radius(const float r){this->r=r;return*this;}
	inline const p3&angle()const{return a;}
	inline p3&dp(){return d;}
	inline bool iscolmx()const{return bits&16;}
	inline glob&setblt(const bool b){if(b)bits|=2;else bits&=0xfffffffd;return*this;}
	inline glob&setitem(const bool b){if(b)bits|=8;else bits&=0xfffffff8;return*this;}
protected:
	inline bool issolid()const{return bits&1;}
	inline glob&setsolid(const bool b){if(b)bits|=1;else bits&=0xfffffffe;return*this;}
	inline bool isblt()const{return bits&2;}
	inline bool iscoldetrec()const{return bits&4;}
	inline glob&setcoldetrec(const bool b){if(b)bits|=4;else bits&=0xfffffffb;return*this;}
	inline bool isitem()const{return bits&8;}
	inline glob&setcolmx(const bool b){if(b)bits|=16;else bits&=0xfffffff0;return*this;}
	void drawboundingsphere(){
//		const GLbyte i=127;
//		glColor3b(i,i,i);
		int detail=(int)(.4f*radius()*drawboundingspheresdetail);
		if(detail<drawboundingspheresdetail)
			detail=drawboundingspheresdetail;
		//glutSolidSphere(radius(),detail,detail);
	}
	//? static
	bool solvesecdegeq(const float a,const float b,const float c,float&t1,float&t2)const{
		const float pt2=2*a;
		if(pt2==0){return false;}
		const float pt1=sqrt(b*b-4*a*c);
		if(pt1!=pt1){
			flf();l()<<" nan "<<endl;
			return false;
		}
		t1=(-b-pt1)/pt2;
		t2=(-b+pt1)/pt2;
		return true;
	}
	virtual bool oncol(glob&o){//? defunc
		metrics::collisions++;
		//flf();l()<<typeid(*this).name()<<"["<<this->getid()<<"]"<<endl;
		if(!o.issolid())return true;
		const p3&p1=*this;
		const p3&u1=d;
		const p3&p2=o;
		const p3&u2=o.d;
		const float r1=radius();
		const float r2=o.radius();
		const float r0=r1+r2;
		const p3 du=p3(u2,u1);
		const float a=p3(du).dot(du);
		const p3 dp=p3(p2,p1);
		const float b=2*p3(dp).dot(du);
		const float c=p3(dp).dot(dp)-r0*r0;
		float t1=0,t2=0;
		if(!solvesecdegeq(a,b,c,t1,t2)){
//			const float d=p3(p1,p2).magn();
//			flf();cout<<"t1="<<t1<<" t2="<<t2<<" a="<<a<<" d="<<d<<" dr="<<r0<<endl;
			return true;//? objects in collision but have no velocities
		}
		float t=min(t1,t2);
		if(t<=-1)t=max(t1,t2);
		if(t>=0)t=min(t1,t2);
		if(t<=-1||t>=0){
//			flf();l("t1=")<<t1<<" t2="<<t2<<" t="<<t<<"  u1("<<u1<<")"<<endl;
//			return true;
		}
		np.set(p1).transl(u1,t);
		p3 np2(p2);
		np2.transl(u2,t);
		const p3 nml(np,np2,true);
		p3 vu1(nml);
		vu1.scale(u1.dot(nml));
		p3 vu2(nml);
		vu2.scale(u2.dot(nml));
		const float m1=m;
		const float m2=o.m;
		const float mm=1/(m1+m2);
		p3 v1(u1);
		v1.transl(vu1,-1);
		v1.transl(vu1,(m1-m2)*mm*bf);
		v1.transl(vu2,2*m2*mm*bf);
//		flf();l()<<"nml("<<nml<<") u1("<<u1<<") u2("<<u2<<") vu1("<<vu1<<") vu2("<<vu2<<") v1("<<v1<<") m1("<<m1<<") m2("<<m2<<")"<<endl;
		nd.set(v1);
		np.transl(nd,dt()*(1-t));
		return true;
	}

protected:
	p3 posinwcs(const p3&p){
		refreshmxmw();
		p3 d;
		mxmw.trnsf(p,d);//? ifidentskip
		return d;
	}
	bool refreshmxmw(){
		if(!&g)
			return false;
		bool refrsh=g.refreshmxmw();
		if(!refrsh)
			if(mxmwpos==*this&&mxmwagl==a)
				return false;
		metrics::mwrfsh++;

		mxmwagl=a;
		mxmwpos=*this;

		mxmw=g.mxmw;
		const m3 m(mxmwpos,mxmwagl);
		mxmw.mul(m);//? ifidentskip

		return true;
	}
};
bool glob::drawboundingspheres=true;
int glob::drawboundingspheresdetail=6;

class grid{
	p3 po;
	float s;
	list<glob*>ls;
	list<glob*>lsmx;
	grid*grds[8];
	const size_t splitthresh=100;
	const int subgridlevels=4;
public:
	grid(const float size,const p3&p=p3()):po(p),s(size),grds({0,0,0,0,0,0,0,0}){metrics::ngrids++;}
	~grid(){metrics::ngrids--;clear();}
	void gldraw(){
		//? sphere in viewpyr check
//		const GLbyte c=(GLbyte)(po.gety()/15*127);
//		glColor3b(0,0,c);
//		glPushMatrix();
//		glTranslatef(po.getx(),po.gety(),po.getz());
		//glutWireCube(s*2);
		//glutWireSphere(s,8,8);
//		glPopMatrix();
		for(auto gr:grds){
			if(!gr)
				continue;
			gr->gldraw();
		}
	}
	void clear(){
		ls.clear();
		lsmx.clear();
		for(auto&g:grds)
			if(g){
				g->clear();
				delete g;//? recycle
				g=0;
			}
	}
	void addall(const list<glob*>&ls){
		for(auto g:ls)
			putif(g,*g,g->radius());
		splitif(subgridlevels);//? splititonthefly
		//? ifallglobswhereaddedtoallsubgrids,stoprecurtion
	}
	void coldet(){
		if(!ls.empty()){
			auto i1=ls.begin();
			while(true){
				auto i2=ls.rbegin();
				if(*i1==*i2)
					break;
				glob&g1=*(*i1);
				do{	glob&g2=*(*i2);
					g1.coldet(g2);
					i2++;
				}while(*i1!=*i2);
				i1++;
			}
			if(!lsmx.empty())
				for(auto g1:ls)
					for(auto g2:lsmx)
						g1->coldet(*g2);
		}
		for(auto g:grds)
			if(g)
				g->coldet();
	}
	void culldraw(const bvol&bv){
		const int c=bv.cull(po,s*1.41f);//? radius
		if(c){
			metrics::gridsculled++;
			return;
		}
		metrics::gridsrend++;
		for(auto g:ls){
//			glPushMatrix();
			g->culldraw(bv);
//			glPopMatrix();
		}
		for(auto g:lsmx){
//			glPushMatrix();
			g->culldraw(bv);
//			glPopMatrix();
		}
		for(auto&g:grds)
			if(g)
				g->culldraw(bv);
	}
private:
	bool putif(glob*g,const p3&p,const float r){
		if((p.getx()+s+r)<po.getx())return false;
		if((p.getx()-s-r)>po.getx())return false;
		if((p.getz()+s+r)<po.getz())return false;
		if((p.getz()-s-r)>po.getz())return false;
		if((p.gety()+s+r)<po.gety())return false;
		if((p.gety()-s-r)>po.gety())return false;
		if(g->iscolmx()){
			lsmx.push_back(g);
//			flf();l()<<lsmx.size()<<endl;
		}else
			ls.push_back(g);
		return true;
	}
	bool splitif(const int nrec){
//		if((ls.size())<splitthresh)//? alg
		if((ls.size()+lsmx.size())<splitthresh)//? alg
			return false;
		if(nrec==0)
			return false;
		const float ns=s/2;
		grds[0]=new grid(ns,p3(po).transl(-ns,ns,-ns));//?
		grds[1]=new grid(ns,p3(po).transl( ns,ns,-ns));
		grds[2]=new grid(ns,p3(po).transl(-ns,ns, ns));
		grds[3]=new grid(ns,p3(po).transl( ns,ns, ns));

		grds[4]=new grid(ns,p3(po).transl(-ns,-ns,-ns));
		grds[5]=new grid(ns,p3(po).transl( ns,-ns,-ns));
		grds[6]=new grid(ns,p3(po).transl(-ns,-ns, ns));
		grds[7]=new grid(ns,p3(po).transl( ns,-ns, ns));

		for(auto g:grds){
			for(auto o:ls)
				g->putif(o,*o,o->radius());//?
			for(auto o:lsmx)
				g->putif(o,*o,o->radius());//?
			g->splitif(nrec-1);//?
		}
		ls.clear();
		lsmx.clear();
		return true;
	}
};
//#include<OpenGL/gl3.h>

class texture{
	GLint id,wi,hi;
};
class mtxstk{
//	const m3&top()const{return 0;}
//	const m3&pushmul(const m3&m){return 0;}
//	const m3&pop(){return 0;}
};

int main(){
	if(!glfwInit())return -1;
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(1024,512,8,8,8,8,32,0,GLFW_WINDOW))return -1;
	cout<<"  version: "<<glGetString(GL_VERSION)<<endl;
	shader::init();
	glfwSwapInterval(0);
	glfwEnable(GLFW_STICKY_KEYS);
	if(glGetError()!=GL_NO_ERROR){cout<<"opengl in error state after init";return -1;}

	//init vbos
	vbo vb;
	vb.glload();

	glob wld(*(glob*)0);
	for(int i=0;i<2;i++){
		glob&g=*new glob(wld);
		g.pos(p3(),p3()).dpos(p3(),p3(0,0,360/60/(i+1))).setvbo(vb);
	}
	if(glGetError()!=GL_NO_ERROR){cout<<"opengl in error state after loading vbos";return -1;}

	tmr t,t1,t2;
	long long frm=0;
//	float a,da=pi;
	while(glfwGetWindowParam(GLFW_OPENED)){
//		if(glGetError()!=GL_NO_ERROR)throw signl(0,"opengl in error");
		frm++;
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		GLfloat mx[16];
		wld.getmxmw().togl(mx);
		glUniformMatrix4fv(shader::umxmw,1,false,mx);

		m3 mwv;
		mwv.ident();
		mwv.mw(p3(0,0,-1),p3());
		mwv.togl(mx);
//		const GLfloat mxwv[]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,-1,1};
		glUniformMatrix4fv(shader::umxwv,1,false,mx);
		wld.gldraw();

		clk::dt=t2.dt();
		clk::tk++;
		wld.dotck();

		cout<<frm<<" "<<t.dt()<<" "<<dt()<<endl;
		glfwSwapBuffers();
	}
	cout<<frm/t1.dt()<<endl;
	return 0;
}
