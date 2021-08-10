#ifndef __GRID_AOI_H_
#define __GRID_AOI_H_

#include <list>
#include <vector>

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

//A为视野主体
typedef void(*AOI_CB) (void *A, void *B);

class Node 
{
	public:
		void *entity;   //实体
		float aoi;  	//视野半径
		AOI_CB enter_cb;    //进入节点视野的回调
		AOI_CB leave_cb;    //离开节点视野的回调
		Pos pos;    	//节点所在x,y

		Node(void *entity, float aoi, AOI_CB enter_cb, AOI_CB leave_cb);
};


typedef list<Node *> list_node;
typedef list_node::iterator list_itor;

class Manager
{ 
	private:
		int xgrid_num;  //x轴的格子数
		int ygrid_num;  //y轴的格子数
		vector< vector<list_node> > grids;  //二维数组保存将地图xy轴切割后的格子
	public:
		Manager(size_t width, size_t length); 
		~Manager();
		
		bool enter(Node *node, Pos *pos);
		bool leave(Node *node);
		bool move(Node *node, Pos *pos);
};

#endif