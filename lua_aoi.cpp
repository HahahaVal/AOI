#include "grid_aoi.h"
#include <lua.h>
#include <lauxlib.h>

using namespace std;

static int new_manager(lua_State *L) {
    int width = lua_tointeger(L, 1);
    int length = lua_tointeger(L, 2);
    Manager *ctx = (Manager *) lua_newuserdata(L, sizeof(Manager));
    *ctx = new Manager(width, length);
    //绑定元表
    luaL_getmetatable(L, "grid_manager");
    lua_setmetatable(L, -2);
    return 1;
}

static int lenter(lua_State *L) {
    //检查参数1的元表是否为grid_manager
    void *ctx =  luaL_checkudata(L, -1, "grid_manager");
    if(!ctx) {
        luaL_error(L, "ctx is NULL\n");
    }
    
    return 0
}

static int lleave(lua_State *L) {
    //检查参数1的元表是否为grid_manager
    void *ctx =  luaL_checkudata(L, -1, "grid_manager");
    if(!ctx) {
        luaL_error(L, "ctx is NULL\n");
    }
    return 0
}

static int lmove(lua_State *L) {
    //检查参数1的元表是否为grid_manager
    void *ctx =  luaL_checkudata(L, -1, "grid_manager");
    if(!ctx) {
        luaL_error(L, "ctx is NULL\n");
    }
    return 0
}

static int lauto_gc(lua_State *L)
{
    //检查参数1的元表是否为grid_manager
    void *ctx =  luaL_checkudata(L, -1, "grid_manager");
    if(!ctx) {
        luaL_error(L, "ctx is NULL\n");
    }
    delete *ctx;
	return 0;
}


static const luaL_Reg l[] = {
    {"enter", lenter},
    {"leave", lleave},
    {"move", lmove},
    { NULL, NULL },
};

static const luaL_Reg l2[] = {
    {"new", new_manager},
    { NULL, NULL },
};

//require "grid_core"
extern int luaopen_grid_core(lua_State *L) {
    luaL_checkversion(L);

	if(L == nullptr)
	{
		return 1;
	}

    //LUA_REGISTRYINDEX注册表中, 注册一个key为className的metatable，并制定其中的一些成员，用于之后生成的userdata
    if (!luaL_newmetatable(L, "grid_manager")) {
        luaL_error(L, "关联元表失败\n");
    }

    lua_pushcfunction(L, lauto_gc);
    //t[k]=v，t是给出的索引处的值，而v是栈顶的那个值，会将值弹出栈
    lua_setfield(L, -2, "__gc");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
    
    //把数组l中的所有函数注册到栈顶的表中
	luaL_setfuncs(L, l, 0);

    //创建一张新的表，并把列表l中的函数注册进去
	luaL_newlib(L, l2);

    return 1;
}
