#include <citro2d.h>
#include <3ds.h>
#include "GameManager.h"
#include <iostream>
int main()
{
	int s = 0;
	GameManager gm(s);

	gm.init();
	while (aptMainLoop())
	{
		gm.update(s);
		gm.draw();

		if (s == -1)
		{
			break;
		}

		gm.updateTimer();
	}

	gm.exit();
	return 0;
}