#include <unistd.h>
#include <unordered_map>

#include "ext/ext.hpp"
#include "front/render.hpp"
#include "gui.hpp"


using namespace ext;
using namespace front;
using namespace gui;





int main(int argc, char ** argv)
{
	chdir("/home/sakar/workspace/nn5-go");

	char const* path_sfg = "";

	if (argc == 1) {
		// play mode

	}
	else if (argc == 2) {
		// replay mode
		path_sfg = argv[1];
	}
	else {
		ext::fail("usage: main [path_sfg]\n");
	}



	window.init(path_sfg);

	//keyboard.init();
	//mouse.init();

	front::Timer rt;
	SDL_Event o;
	while (not window.f_stop)
	{
		if (rt.left() <= 0)
		{
			window.render();
			rt.set(33);
		}

		SDL_WaitEventTimeout(nullptr, rt.left());
		while (SDL_PollEvent(&o))
		{
			window.handle(o);
		}

	}




	return 0;
}
