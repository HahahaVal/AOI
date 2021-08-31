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

for i=100, 1, -1 do
    local random_aoi = math.random(1,150)
   
    local random_x = math.random(1,150)
    local symbol = math.random(0, 1)
    if symbol == 0 then
        random_x = -random_x
    end
    local random_y = math.random(1,150)
    local symbol = math.random(0, 1)
    if symbol == 0 then
        random_y = -random_y
    end
    M.manager:enter(map, i, random_aoi, random_x, random_y)

   
    local random_x = math.random(1,150)
    local symbol = math.random(0, 1)
    if symbol == 0 then
        random_x = -random_x
    end
    local symbol = math.random(0, 1)
    local random_y = math.random(1,150)
    if symbol == 0 then
        random_y = -random_y
    end
    M.manager:move(map, i, random_x, random_y)


    local random_x = math.random(1,150)
    local symbol = math.random(0, 1)
    if symbol == 0 then
        random_x = -random_x
    end
    local symbol = math.random(0, 1)
    local random_y = math.random(1,150)
    if symbol == 0 then
        random_y = -random_y
    end
    local random_radius = math.random(0, 20)
    local ret = M.manager:find_entitys(random_x, random_y, random_radius)
    if #ret > 0 then
        print("~~~~~~~~~~~~~~~FFFFF",#ret,random_x,random_y,random_radius)
    end
end

for i=100, 1, -1 do
    M.manager:leave(map, i)
end


M.manager:enter(map, 1, 1, -63.2, -77.3)

local ret = M.manager:find_entitys(-63.71772, -72.64935, 2)
print("~~~~~~~~~~~~~~FFFFF",#ret)

M.manager:leave(map, 1)