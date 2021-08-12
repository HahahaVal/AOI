#ifndef __GRID_AOI_H_
#define __GRID_AOI_H_

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <list>
#include <vector>
#include <map>

using namespace std;

#define INVALID_X -99999999
#define INVALID_Y -99999999
#define MAX_LEN 512
#define GRID_SIZE 30    // size*3的屏幕范围，3*3可见
#define MAX_AOI GRID_SIZE //最大视野不能比一个格子更大
#define __EXACT_AOI //是否精准AOI

typedef struct
{
	int x;
	int y;
}Pos;

class Node 
{
	public:
		int entityId;   //实体
		float aoi;  	//视野半径
		Pos pos;    	//节点所在x,y

		Node(int entityId, float aoi);
};


typedef list<Node *> list_node;
typedef list_node::iterator list_itor;

typedef map<int, Node *> map_node;

class Manager
{ 
	private:
		int xgrid_num;  //x轴的格子数
		int ygrid_num;  //y轴的格子数
		vector< vector<list_node> > grids;  //二维数组保存将地图xy轴切割后的格子
		map_node nodes;	//所有的node节点
	public:
		Manager(size_t width, size_t length); 
		~Manager(){};
		
		bool enter(struct lua_State *map, int entityId, int aoi, int x, int y);
		bool leave(struct lua_State *map, int entityId);
		bool move(struct lua_State *map, int entityId, int aoi, int x, int y);
};

#endif
