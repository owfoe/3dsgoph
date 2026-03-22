
#include <citro2d.h>
#include <3ds.h>
#include <GameManager.h>
int main() {
	int s = 0;
	GameManager gm(s);
	gm.init();


	while (aptMainLoop()) {
		gm.update(s);
		if (s == -1) {
			break;
		}
	}

	gm.exit();
	return 0;
}