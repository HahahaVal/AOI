local aoi = require "aoi"

local map = {}

function map:OnEnter(entityId1, entityId2)
    print("~~~~~~~~~~~~~~~EEEEE", self, entityId1, entityId2)
end

function map:OnLeave(entityId1, entityId2)
    print("~~~~~~~~~~~~~~~LLLLL", self, entityId1, entityId2)
end

local M = {}

M.manager = aoi.new(300,300)
M.manager:enter(map, 1, 2, 3, 4)
M.manager:enter(map, 2, 2, 3, 4)

M.manager:move(map, 1, 3, 100)
M.manager:move(map, 2, 3, 100)

M.manager:leave(map, 1)
M.manager:leave(map, 2)