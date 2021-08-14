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
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	manager->enter(L, 1, 2, 3, 4);
	manager->enter(L, 2, 3, 4, 4);

	manager->move(L, 1, 3, 8);
	manager->move(L, 2, 4, 8);

	manager->leave(L, 1);
	manager->leave(L, 2);

	return 1;
}