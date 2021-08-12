#include "grid_aoi.h"

extern "C" {
    int luaopen_aoi(struct lua_State *L);

}

#define check_grid_manager(L) (Manager *)luaL_checkudata(L, 1, "Manager")

static int new_manager(struct lua_State *L) {
    int num = lua_gettop(L);
	if(num != 2)
	{
		printf("new_manager param number is not correct!");
		return 0;
    }
    int width = lua_tointeger(L, 1);
    int length = lua_tointeger(L, 2);
    Manager *manager = (Manager *) lua_newuserdata(L, sizeof(Manager));
    manager = new Manager(width, length);
    //通常，用来区别一个 userdata 类型与另外一个 userdata 类型的方法是为其创建一个特定的元表。
    //每当我们创建一个 userdata 的时候，我们就为其指定一个对应的 元表；每当我们获取一个 userdata 的时候，我们都要检查其是否拥有对应的 元表。
    //Lua 代码是无法修改 userdata 的元表，所以其无法欺骗这种检查。需要空间来存在这个新的 metatable，以便在创建 userdata 的时候能够访问它，同时用它来检查一个 userdata 是否有正确的类型。有两种可选的方法：1）存储在全局注册表；2）作为库函数中的 上值 存储
    luaL_getmetatable(L, "Manager");
    lua_setmetatable(L, -2);    //userdata.__index=Manager
    return 1;
}
//bool enter(struct lua_State *map, int entityId, int aoi, int x, int y);
static int lenter(struct lua_State *L) {
	int num = lua_gettop(L);
	if(num != 5)
	{
		printf("lenter param number is not correct!");
		return 0;
    }
    Manager *manager = check_grid_manager(L);
    int entityId = lua_tointeger(L, 2);
    int aoi = lua_tointeger(L, 3);
    int x = lua_tointeger(L, 4);
    int y = lua_tointeger(L, 5);
    bool ret = manager->enter(L,entityId,aoi,x,y);
    lua_pushboolean(L, ret);
    return 1;
}

//bool leave(lua_State *map, int entityId);
static int lleave(struct lua_State *L) {
    int num = lua_gettop(L);
	if(num != 2)
	{
		printf("lleave param number is not correct!");
		return 0;
    }
    Manager *manager = check_grid_manager(L);
    int entityId = lua_tointeger(L, 2);
    bool ret = manager->leave(L,entityId);
    lua_pushboolean(L, ret);
    return 1;
}

//bool move(lua_State *map, int entityId, int aoi, int x, int y);
static int lmove(struct lua_State *L) {
    int num = lua_gettop(L);
	if(num != 5)
	{
		printf("lmove param number is not correct!");
		return 0;
    }
    Manager *manager = check_grid_manager(L);
    int entityId = lua_tointeger(L, 2);
    int aoi = lua_tointeger(L, 3);
    int x = lua_tointeger(L, 4);
    int y = lua_tointeger(L, 5);
    bool ret = manager->move(L,entityId,aoi,x,y);
    lua_pushboolean(L, ret);
    return 1;
}

static int lauto_gc(struct lua_State *L)
{
    Manager *manager = check_grid_manager(L);
    delete manager;
	return 0;
}


static const luaL_Reg lib_m[] = {
    {"enter", lenter},
    {"leave", lleave},
    {"move", lmove},
    { NULL, NULL },
};

static const luaL_Reg lib_f[] = {
    {"new", new_manager},
    { NULL, NULL },
};

//require "aoi"
int luaopen_aoi(struct lua_State *L) {
    luaL_checkversion(L);

	if(L == NULL)
	{
		return 1;
	}

    //LUA_REGISTRYINDEX全局注册表中, 注册一个key为className的metatable
    if (!luaL_newmetatable(L, "Manager")) {
        luaL_error(L, "关联元表失败\n");
    }

    lua_pushcfunction(L, lauto_gc);
    //t[k]=v，t是给出的索引处的值，而v是栈顶的那个值，会将值弹出栈
    lua_setfield(L, -2, "__gc");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
    
    //把数组l中的所有函数注册到栈顶的表中,一个针对对象的方法
	luaL_setfuncs(L, lib_m, 0);

    //创建一张新的表，并把列表l中的函数注册进去,一个针对常规的函数
	luaL_newlib(L, lib_f);

    return 1;
}