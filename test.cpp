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

    manager->enter(L,1,2,3,4);
    return 1;
}