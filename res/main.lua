enemy = require("enemy")
enemy:Create()
a = enemy:Update()

function OnCreate()
    player = Entity.New()
    world = World.Load("world.json")
end

function OnUpdate()
    player:Update(5, 7)
    world:Update()
end

function OnDraw()
    player:Draw()
    world:Draw()
end