#include "grid_aoi.h"

const int MAPX = 300;
const int MAPZ = 300;
const int AOI = 30;
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int main(int argc, char* argv[])
{
	Manager *manager = new Manager(MAPX, MAPZ);
	delete manager;

	return 1;
}