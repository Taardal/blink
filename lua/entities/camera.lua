local heightAbovePlayer = 0
local distanceBehindPlayer = 2

function Camera.onUpdate(cameraId, timestep)
    local cameraTransformComponent = Entity:getTransformComponent(cameraId)

    local playerId = Entity:getIdByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerId)

    cameraTransformComponent.position.z = playerTransformComponent.position.z + distanceBehindPlayer

    Entity:setTransformComponent(cameraId, cameraTransformComponent)
end
