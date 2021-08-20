local aoi = require "aoi"

local map = {}

function map:OnEnter(entityId1, entityId2)
    print("~~~~~~~~~~~~~~~EEEEE", self, entityId1, entityId2)
end

function map:OnLeave(entityId1, entityId2)
    print("~~~~~~~~~~~~~~~LLLLL", self, entityId1, entityId2)
end

local M = {}

for i=1000, 1, -1 do
    M.manager = aoi.new(300,300)
    local random_aoi = math.random(1,150)
    local random_x = math.random(1,150)
    local random_y = math.random(1,150)
    M.manager:enter(map, i, random_aoi, random_x, random_y)

    local random_aoi = math.random(1,100)
    local random_x = math.random(1,150)
    local random_y = math.random(1,150)
    M.manager:enter(map, i+1, random_aoi, random_x, random_y)

    local random_x = math.random(1,150)
    local random_y = math.random(1,150)
    M.manager:move(map, i, random_x, random_y)
    local random_x = math.random(1,150)
    local random_y = math.random(1,150)
    M.manager:move(map, i+1, random_x, random_y)

    M.manager:leave(map, i)
    M.manager:leave(map, i+1)
end


