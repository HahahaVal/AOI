#include "grid_aoi.h"
#include "stdio.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>

Node::Node(void *_entity, float _aoi, AOI_CB _enter_cb, AOI_CB _leave_cb)
{
	entity = _entity;
	enter_cb = _enter_cb;
	leave_cb = _leave_cb;
	pos.x = INVALID_X;
	pos.y = INVALID_Y;

	aoi = _aoi;
	if(aoi > MAX_AOI) {
		printf("aoi too big:%f, expected<%d", aoi, MAX_AOI);
		aoi = MAX_AOI;
	}
}

inline int Min(int a, int b) { return (a<b ? a:b) ;}
inline int Max(int a, int b) { return (a<b ? b:a) ;}

inline float Dis(Pos * pos1, Pos * pos2)
{
	int dx = pos1->x - pos2->x;
	int dy = pos2->y - pos2->y;
	return sqrt(dx*dx+dy*dy);
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
	xgrid_num = width/GRID_SIZE;
	ygrid_num = length/GRID_SIZE;
	if(xgrid_num > MAX_LEN) {
		printf("width too huge:%d limited:%d", xgrid_num, MAX_LEN);
		xgrid_num = MAX_LEN;
	}
	if (ygrid_num > MAX_LEN) {
		printf("length too huge:%d limited:%d", ygrid_num, MAX_LEN);
		ygrid_num = MAX_LEN;
	}

	grids.resize(xgrid_num+1);
	for (unsigned int i=0; i<grids.size(); i++) {
		grids[i].resize(ygrid_num+1);
	}
}

bool Manager::Enter(Node *node, Pos *pos)
{
	if (node->pos.x != INVALID_X || node->pos.y != INVALID_Y) {
		printf("node has enter some place:entity=%p,x=%d,y=%d", node->entity, node->pos.x, node->pos.y);
		return false;
	}
	int nxgrid_num = pos->x/GRID_SIZE;
	int nygrid_num = pos->y/GRID_SIZE;
	if ((nxgrid_num<0 || nxgrid_num>xgrid_num) || (nygrid_num<0 || nygrid_num>ygrid_num)) {
		printf("enter pos is not valid:entity=%p,x=%d,y=%d", node->entity, pos->x, pos->y);
		return false;
	}

	//x直径为3
	int xl_index = (nxgrid_num-1<0) ? 0 : (nxgrid_num-1);
	int xr_index = (nxgrid_num+1>xgrid_num) ? xgrid_num : (xgrid_num+1);

	//y直径为3
	int yl_index = (nygrid_num-1<0) ? 0 : (nygrid_num-1);
	int yr_index = (nygrid_num+1>ygrid_num) ? ygrid_num : (nygrid_num+1);

	//遍历周围的九个格子
	for (int i=xl_index; i<=xr_index; i++) {
		for (int j=yl_index; j<=yr_index; j++) {
			list_node list = grids[i][j];
			for (list_itor it=list.begin(); it!=list.end(); ++it) {
				assert(*it != node);
				Node * itv = *it;
				//it进入node的视野
				if (IsInAOI(pos, &(itv->pos), node->aoi)) {
					node->enter_cb(node->entity, itv->entity);
				}
				//node进入it的视野
				if (IsInAOI(&(itv->pos), pos, itv->aoi)) {
					itv->enter_cb(itv->entity, node->entity);
				}
			}
		}
	}
	node->pos = *pos;
	grids[nxgrid_num][nygrid_num].push_back(node);
	return true;
}

bool Manager::Move(Node *node, Pos * pos)
{
	//原坐标
	int oxgrid_num = node->pos.x/GRID_SIZE;
	int oygrid_num = node->pos.y/GRID_SIZE;
	//新坐标
	int nxgrid_num = pos->x/GRID_SIZE;
	int nygrid_num = pos->y/GRID_SIZE;
	if ((nxgrid_num<0 || nxgrid_num>xgrid_num) || (nygrid_num<0 || nygrid_num>ygrid_num)) {
		printf("pos is not valid:entity=%p,x=%d,y=%d", node->entity, pos->x, pos->y);
		return false;
	}

	//删除节点
	bool del_ret = false;
	list_node list = grids[oxgrid_num][oygrid_num];
	for (list_itor it = list.begin(); it!=list.end(); ++it) {
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
	
	//新旧两点间格子的遍历,旧格子的视野移除和新格子的视野加入
	int min_xnum = Min(oxgrid_num, nxgrid_num);
	int max_xnum = Max(oxgrid_num, nxgrid_num);
	int xl_index = ((min_xnum-1)<0) ? 0 : (min_xnum-1);
	int xr_index = ((max_xnum+1)>xgrid_num) ? xgrid_num : (max_xnum+1);

	int min_ynum = Min(oygrid_num, nygrid_num);
	int max_ynum = Max(oygrid_num, nygrid_num);
	int yl_index = ((min_ynum-1)<0) ? 0 : (min_ynum-1);
	int yr_index = ((max_ynum+1)>ygrid_num) ? ygrid_num : (max_ynum+1);

	for (int i=xl_index; i<=xr_index; i++){
		for (int j=yl_index; j<=yr_index; j++){
			list_node list = grids[i][j];
			for (list_itor it=list.begin(); it!=list.end(); ++it) {
				Node * itv = *it;
				assert(itv != node);
				//老坐标在node视野内，新坐标不在node视野内
				if (IsInAOI(&(oldpos), &(itv->pos), node->aoi) && !IsInAOI(pos, &(itv->pos), node->aoi)) {
					node->leave_cb(node->entity, itv->entity);
				}
				//老坐标不在node视野内，新坐标在视野内，it 进入 node的视野.
				if (!IsInAOI(&(oldpos), &(itv->pos), node->aoi) && IsInAOI(pos, &(itv->pos), node->aoi)) {
					node->enter_cb(node->entity, itv->entity);
				}

				//老坐标在it视野内，新坐标不在it视野内
				if (IsInAOI(&(itv->pos), &(oldpos), itv->aoi) && !IsInAOI(&(itv->pos), pos, itv->aoi)) {
					itv->leave_cb(itv->entity, node->entity);
				}
				//老坐标不在it视野内，新坐标在it视野内
				if (!IsInAOI(&(itv->pos), &(oldpos), itv->aoi) && IsInAOI(&(itv->pos), pos, itv->aoi)) {
					itv->enter_cb(itv->entity, node->entity);
				}
			}
		}
	}
	node->pos = *pos;
	grids[nxgrid_num][nygrid_num].push_back(node);
	return true;
}

bool Manager::Leave(Node * node)
{
	Pos pos = node->pos;
	if (pos.x == INVALID_X || pos.y == INVALID_Y) {
		printf("node has gone away:entity=%p, pos.x=%d, pos.y=%d", node->entity, pos.x, pos.y);
		return false;
	}
	int nxgrid_num = pos.x/GRID_SIZE;
	int nygrid_num = pos.y/GRID_SIZE;

	//删除节点
	bool del_ret = false;
	list_node list = grids[nxgrid_num][nygrid_num];
	for (list_itor it = list.begin(); it!=list.end(); ++it) {
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
	int xl_index = (nxgrid_num-1<0) ? 0 : (nxgrid_num-1);
	int xr_index = (nxgrid_num+1>xgrid_num) ? xgrid_num : (xgrid_num+1);

	//y直径为3
	int yl_index = (nygrid_num-1<0) ? 0 : (nygrid_num-1);
	int yr_index = (nygrid_num+1>ygrid_num) ? ygrid_num : (nygrid_num+1);

	//遍历周围的九个格子
	for (int i=xl_index; i<=xr_index; i++) {
		for (int j=yl_index; j<=yr_index; j++) {
			list_node list = grids[i][j];
			for (list_itor it=list.begin(); it!=list.end(); ++it) {
				Node * itv = *it;
				//it离开node的视野范围
				if (IsInAOI(&(pos), &(itv->pos), node->aoi)) { 
					node->leave_cb(node->entity, itv->entity);
				}
				//node离开it的视野范围
				if (IsInAOI(&(itv->pos), &(pos), itv->aoi)) { 
					itv->leave_cb(itv->entity, node->entity);
				}
			}
		}
	}
	return true;
}