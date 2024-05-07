local playerId = nil

function Camera.onUpdate(cameraId, timestep)
    --print("begin")
    local cameraTransformComponent = Entity:getTransformComponent(cameraId)
    --print("cameraTransformComponent -- " .. tostring(cameraTransformComponent ~= nil))

    if playerId == nil then
    	playerId = Entity:getIdByTag("Player")
    end
    --print("playerId -- " .. playerId)
    local playerTransformComponent = Entity:getTransformComponent(playerId)
    --print("playerTransformComponent -- " .. tostring(playerTransformComponent ~= nil))

    cameraTransformComponent.position.x = playerTransformComponent.position.x + 2
    cameraTransformComponent.position.y = playerTransformComponent.position.y + 1
    cameraTransformComponent.position.z = playerTransformComponent.position.z + 0

    Entity:setTransformComponent(cameraId, cameraTransformComponent)
    --print("position [" .. cameraTransformComponent.position.x .. ", " .. cameraTransformComponent.position.y .. ", " .. cameraTransformComponent.position.z .. "]")
end
