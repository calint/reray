#include "../dbox/dbox.cpp"
using namespace dbox;
namespace app{
	class vbodots:public vbo{
		inline int elemtype()const{return 5;}
		inline int nvertices()const{return 1024*1024;}
		virtual void vertices(float fa[])const{
			const int n=nvertices();
			int j=0;
			for(int i=0;i<n;i++){
				fa[j++]=rnd(-1,1);//x
				fa[j++]=rnd(-1,1);//y
				fa[j++]=rnd(-1,1);//z

				fa[j++]=rnd(0,1);//r
				fa[j++]=rnd(0,1);//g
				fa[j++]=rnd(0,1);//b
				fa[j++]=rnd(0,1);//a

				fa[j++]=0;//s
				fa[j++]=0;//t
			}
		}
	public:
		static vbodots inst;
	};
	vbodots vbodots::inst;
	class objdots:public glob{
	public:
		objdots(glob&g=wd,const pt&p=pt(),const pt&a=pt(),const float r=1,const float density_gcm3=1,const float bounciness=.5f,vbo&vb=vbodots::inst):
			glob(g,p,a,r,density_gcm3,bounciness,vb)
		{
			bits|=1;
//			issolid();
		}
	};

	void run(int argc,char**argv){
		while(argc--)puts(*argv++);

		dbox::init();

		vbo vb;
		vb.glload();
		app::vbodots::inst.glload();

		app::objdots*o=new app::objdots();
		o->dpos(pt(0,0,0),pt(0,0,90));
		o->setscl(pt(.5f,.5f,.5f));
//		o=new app::objdots();
//		o->dpos(pt(0,0,0),pt(0,0,20));
//
//		const int n=128;
//		for(int i=0;i<n;i++){
//			new glob(wd,pt(rnd(-1,1),rnd(-1,1),0),pt(),.01f,1,0,vb);
//		}

		wn=new windo();
		wn->pos(pt(0,0,1),pt());
		wn->dpos(pt(0,0,-.1f),pt());

		dbox::run();
	}
}
//////////////////////////////////////////////////////////////////////////
int main(int argc,char**argv){app::run(argc,argv);return 0;}
