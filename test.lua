local aoi = require "aoi"

local map = {}

function map:OnEnter()
end

function map:OnLeave()
end

local M = {}

M.manager = aoi.new(300,300)
M.manager:enter(map, 1, 2, 3, 4)
M.manager:enter(map, 2, 2, 3, 4)