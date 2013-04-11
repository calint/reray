#include "../dbox/dbox.cpp"
using namespace dbox;
int main(int argc,char**argv){
	while(argc--)puts(*argv++);
	init();

	vbo vb;
	vb.glload();

	new glob(wd,pt(0,0,0),pt(),.01f,1,0,vb);
	for(int i=0;i<1000;i++){
		new glob(wd,pt(rnd(-1,1),rnd(-1,1),0),pt(),.01f,1,0,vb);
	}

	wn=new windo();
	wn->pos(pt(0,0,1),pt());
	wn->dpos(pt(0,0,-.1f),pt());

	run();
	return 0;
}
