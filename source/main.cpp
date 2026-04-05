#include <citro2d.h>
#include <3ds.h>
#include "GameManager.h"
#include <iostream>
int main()
{

	int s = 0;
	GameManager gm(s);
	gm.init();
	C3D_RenderTarget *topRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);

	std::cout << "a" << std::endl;
	while (aptMainLoop())
	{
		std::cout << "c" << std::endl;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		std::cout << "b" << std::endl;
		C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
		std::cout << "d" << std::endl;
		C2D_SceneBegin(topRight);
		std::cout << "init" << std::endl;
		gm.draw(topRight);
		gm.update(s);
		C3D_FrameEnd(0);
		if (s == -1)
		{
			break;
		}
	}

	gm.exit();
	return 0;
}