#include "grid_aoi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

Node::Node(int _entityId, float _aoi)
{
	entityId = _entityId;
	aoi = _aoi;
	if (aoi > MAX_AOI) {
		printf("aoi too big:%f, expected<%d", aoi, MAX_AOI);
		aoi = MAX_AOI;
	}

	pos.x = INVALID_X;
	pos.y = INVALID_Y;
}

inline int Min(int a, int b) { return (a < b ? a : b); }
inline int Max(int a, int b) { return (a < b ? b : a); }

inline void cb_enter(struct lua_State *L, int entityId1, int entityId2)
{
	lua_getfield(L, -1, "OnEnter");
	lua_pushvalue(L, -2);
	lua_pushinteger(L, entityId1);
	lua_pushinteger(L, entityId2);
	lua_call(L, 3, 0);
}

inline void cb_leave(struct lua_State *L, int entityId1, int entityId2)
{
	lua_getfield(L, -1, "OnLeave");
	lua_pushvalue(L, -2);
	lua_pushinteger(L, entityId1);
	lua_pushinteger(L, entityId2);
	lua_call(L, 3, 0);
}

inline float Dis(Pos * pos1, Pos * pos2)
{
	int dx = pos1->x - pos2->x;
	int dy = pos2->y - pos2->y;
	return sqrt(dx*dx + dy*dy);
}

inline bool IsInAOI(Pos *pos1, Pos *pos2, float aoi)
{
	//做粗略的判断减少Dis的判断
	if ((abs(pos1->x - pos2->x) <= aoi) && abs(pos1->y - pos2->y) <= aoi) {
		//如果是要求比较精确的AOI则需要判断圆形的，而不是正方形的
#ifdef __EXACT_AOI
		if (Dis(pos1, pos2) < aoi * aoi)
			return true;
#else 
		return true;
#endif
	}
	return false;
}

Manager::Manager(size_t width, size_t length)
{
	xgrid_num = width / GRID_SIZE;
	ygrid_num = length / GRID_SIZE;
	if (xgrid_num > MAX_LEN) {
		printf("width too huge:%d limited:%d", xgrid_num, MAX_LEN);
		xgrid_num = MAX_LEN;
	}
	if (ygrid_num > MAX_LEN) {
		printf("length too huge:%d limited:%d", ygrid_num, MAX_LEN);
		ygrid_num = MAX_LEN;
	}
	grids.resize(xgrid_num + 1);
	for (unsigned int i = 0; i < grids.size(); i++) {
		grids[i].resize(ygrid_num + 1);
	}
}

bool Manager::enter(struct lua_State *L, int entityId, int aoi, int x, int y)
{
	Node *node = new Node(entityId, aoi);
	if (node->pos.x != INVALID_X || node->pos.y != INVALID_Y) {
		luaL_error(L, "node has enter some place:entityId=%d,x=%d,y=%d", node->entityId, node->pos.x, node->pos.y);
		return false;
	}
	int nxgrid_num = x / GRID_SIZE;
	int nygrid_num = y / GRID_SIZE;
	if ((nxgrid_num<0 || nxgrid_num>xgrid_num) || (nygrid_num<0 || nygrid_num>ygrid_num)) {
		luaL_error(L, "enter pos is not valid:entityId=%d,x=%d,y=%d", node->entityId, x, y);
		return false;
	}

	//x直径为3
	int xl_index = (nxgrid_num - 1 < 0) ? 0 : (nxgrid_num - 1);
	int xr_index = (nxgrid_num + 1 > xgrid_num) ? xgrid_num : (nxgrid_num + 1);

	//y直径为3
	int yl_index = (nygrid_num - 1 < 0) ? 0 : (nygrid_num - 1);
	int yr_index = (nygrid_num + 1 > ygrid_num) ? ygrid_num : (nygrid_num + 1);

	Pos pos;
	pos.x = x;
	pos.y = y;

	//遍历周围的九个格子
	for (int i = xl_index; i <= xr_index; i++) {
		for (int j = yl_index; j <= yr_index; j++) {
			list_node list = grids[i][j];
			for (list_itor it = list.begin(); it != list.end(); ++it) {
				assert(*it != node);
				Node * itv = *it;
				//it进入node的视野
				if (IsInAOI(&pos, &(itv->pos), node->aoi)) {
					cb_enter(L, node->entityId, itv->entityId);
				}
				//node进入it的视野
				if (IsInAOI(&(itv->pos), &pos, itv->aoi)) {
					cb_enter(L, itv->entityId, node->entityId);
				}
			}
		}
	}
	node->pos = pos;
	grids[nxgrid_num][nygrid_num].push_back(node);
	nodes[entityId] = node;
	return true;
}

bool Manager::move(struct lua_State *L, int entityId, int x, int y)
{
	Node *node = nodes[entityId];
	if (node == NULL)
	{
		luaL_error(L, "entity not exit:entityId=%d", node->entityId);
		return false;
	}

	//原坐标
	int oxgrid_num = node->pos.x / GRID_SIZE;
	int oygrid_num = node->pos.y / GRID_SIZE;
	//新坐标
	int nxgrid_num = x / GRID_SIZE;
	int nygrid_num = y / GRID_SIZE;
	if ((nxgrid_num<0 || nxgrid_num>xgrid_num) || (nygrid_num<0 || nygrid_num>ygrid_num)) {
		luaL_error(L, "pos is not valid:entityId=%d,x=%d,y=%d", node->entityId, x, y);
		return false;
	}

	//删除节点
	bool del_ret = false;
	list_node &list = grids[oxgrid_num][oygrid_num];
	for (list_itor it = list.begin(); it != list.end(); ++it) {
		if (*it == node) {
			list.erase(it);
			del_ret = true;
			break;
		}
	}
	Pos oldpos = node->pos;
	node->pos.x = INVALID_X;
	node->pos.y = INVALID_Y;
	assert(del_ret);

	Pos pos;
	pos.x = x;
	pos.y = y;

	//新旧两点间格子的遍历,旧格子的视野移除和新格子的视野加入
	int min_xnum = Min(oxgrid_num, nxgrid_num);
	int max_xnum = Max(oxgrid_num, nxgrid_num);
	int xl_index = ((min_xnum - 1) < 0) ? 0 : (min_xnum - 1);
	int xr_index = ((max_xnum + 1) > xgrid_num) ? xgrid_num : (max_xnum + 1);

	int min_ynum = Min(oygrid_num, nygrid_num);
	int max_ynum = Max(oygrid_num, nygrid_num);
	int yl_index = ((min_ynum - 1) < 0) ? 0 : (min_ynum - 1);
	int yr_index = ((max_ynum + 1) > ygrid_num) ? ygrid_num : (max_ynum + 1);

	for (int i = xl_index; i <= xr_index; i++) {
		for (int j = yl_index; j <= yr_index; j++) {
			list_node list = grids[i][j];
			for (list_itor it = list.begin(); it != list.end(); ++it) {
				Node * itv = *it;
				assert(itv != node);
				//老坐标在node视野内，新坐标不在node视野内
				if (IsInAOI(&(oldpos), &(itv->pos), node->aoi) && !IsInAOI(&pos, &(itv->pos), node->aoi)) {
					cb_leave(L, node->entityId, itv->entityId);
				}
				//老坐标不在node视野内，新坐标在视野内，it 进入 node的视野.
				if (!IsInAOI(&(oldpos), &(itv->pos), node->aoi) && IsInAOI(&pos, &(itv->pos), node->aoi)) {
					cb_enter(L, node->entityId, itv->entityId);
				}

				//老坐标在it视野内，新坐标不在it视野内
				if (IsInAOI(&(itv->pos), &(oldpos), itv->aoi) && !IsInAOI(&(itv->pos), &pos, itv->aoi)) {
					cb_leave(L, itv->entityId, node->entityId);
				}
				//老坐标不在it视野内，新坐标在it视野内
				if (!IsInAOI(&(itv->pos), &(oldpos), itv->aoi) && IsInAOI(&(itv->pos), &pos, itv->aoi)) {
					cb_enter(L, itv->entityId, node->entityId);
				}
			}
		}
	}
	node->pos = pos;
	grids[nxgrid_num][nygrid_num].push_back(node);
	return true;
}

bool Manager::leave(struct lua_State *L, int entityId)
{
	Node *node = nodes[entityId];
	if (node == NULL)
	{
		luaL_error(L, "entity not exit:entityId=%d", node->entityId);
		return false;
	}

	Pos pos = node->pos;
	if (pos.x == INVALID_X || pos.y == INVALID_Y) {
		luaL_error(L, "node has gone away:entityId=%d, pos.x=%d, pos.y=%d", node->entityId, pos.x, pos.y);
		return false;
	}
	int nxgrid_num = pos.x / GRID_SIZE;
	int nygrid_num = pos.y / GRID_SIZE;

	//删除节点
	bool del_ret = false;
	list_node &list = grids[nxgrid_num][nygrid_num];
	for (list_itor it = list.begin(); it != list.end(); ++it) {
		if (*it == node) {
			list.erase(it);
			del_ret = true;
			break;
		}
	}
	node->pos.x = INVALID_X;
	node->pos.y = INVALID_Y;
	assert(del_ret);

	//x直径为3
	int xl_index = (nxgrid_num - 1 < 0) ? 0 : (nxgrid_num - 1);
	int xr_index = (nxgrid_num + 1 > xgrid_num) ? xgrid_num : (nxgrid_num + 1);

	//y直径为3
	int yl_index = (nygrid_num - 1 < 0) ? 0 : (nygrid_num - 1);
	int yr_index = (nygrid_num + 1 > ygrid_num) ? ygrid_num : (nygrid_num + 1);

	//遍历周围的九个格子
	for (int i = xl_index; i <= xr_index; i++) {
		for (int j = yl_index; j <= yr_index; j++) {
			list_node list = grids[i][j];
			for (list_itor it = list.begin(); it != list.end(); ++it) {
				Node * itv = *it;
				//it离开node的视野范围
				if (IsInAOI(&(pos), &(itv->pos), node->aoi)) {
					cb_leave(L, node->entityId, itv->entityId);
				}
				//node离开it的视野范围
				if (IsInAOI(&(itv->pos), &(pos), itv->aoi)) {
					cb_leave(L, itv->entityId, node->entityId);
				}
			}
		}
	}
	nodes[entityId] = NULL;
	delete node;
	return true;
}
