local heightAbovePlayer = 0
local distanceBehindPlayer = 2

function Camera.onUpdate(cameraId, timestep)
    --local a = glm.vec3(1, 1, 1)
    --local b = glm.vec3(2, 2, 2)
    --print("add: " .. formatv(a + b))
    --print("sub: " .. formatv(a - b))
    --print("mul: " .. formatv(a * b))
    --print("div: " .. formatv(a / b))

    local cameraTransformComponent = Entity:getTransformComponent(cameraId)

    local playerId = Entity:getIdByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerId)

    print("cb: " .. formatv(cameraTransformComponent.position))
    local c = cameraTransformComponent.position + 1
    print("ca: " .. formatv(c))

    print("db: " .. formatv(playerTransformComponent.position))
    local d = playerTransformComponent.position + 1
    print("da: " .. formatv(d))

    print("add: " .. formatv(c + d))
    print("sub: " .. formatv(c - d))
    print("mul: " .. formatv(c * d))
    print("div: " .. formatv(c / d))

    --
    --cameraTransformComponent.position.z = playerTransformComponent.position.z + distanceBehindPlayer
    --
    --Entity:setTransformComponent(cameraId, cameraTransformComponent)
end
