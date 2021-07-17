# AOI
基于网格算法实现的AOI动态库，使用lua脚本调用提供以下接口

  * local manager = aoi.new(width, length)
  * manager:enter(map, entityId, aoi, x, y)
    * 提供函数回调map:OnEnter(entityId1, entityId2) entityId2进入entityId1的视野
  * manager:move(map, entityId, x, y)
  * manager:leave(map, entityId)
    * 提供函数回调map:OnLeave(entityId1, entityId2)