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

for i=10000, 1, -1 do
    local random_aoi = math.random(1,30)
    local random_x1 = math.random(0,300)
    local random_y1 = math.random(0,300)
    M.manager:enter(map, i, random_aoi, random_x1, random_y1)
    local random_x2 = math.random(0,300)
    local random_y2 = math.random(0,300)
    M.manager:move(map, i, random_x2, random_y2)
    local random_x3 = math.random(0,300)
    local random_y3 = math.random(0,300)
    local random_radius = math.random(0, 20)
    local ret = M.manager:find_entitys(random_x3, random_y3, random_radius)
    if #ret > 0 then
        print("~~~~~~~~~~~~~~~FFFFF",#ret,random_x3,random_y3,random_radius)
    end
end

for i=10000, 1, -1 do
    M.manager:leave(map, i)
end

collectgarbage("collect")