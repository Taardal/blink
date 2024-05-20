local heightAbovePlayer = 1
local distanceBehindPlayer = 10
local loggingEnabled = false

function Camera.onUpdate(cameraId, timestep)
    local cameraTransformComponent = Entity:getTransformComponent(cameraId)

    local playerId = Entity:getIdByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerId)

    cameraTransformComponent.position.x = playerTransformComponent.position.x
    cameraTransformComponent.position.y = playerTransformComponent.position.y --+ heightAbovePlayer
    cameraTransformComponent.position.z = playerTransformComponent.position.z + distanceBehindPlayer

    if Keyboard:isPressed(Key.B) then
        loggingEnabled = true
    end
    if Keyboard:isPressed(Key.V) then
        loggingEnabled = false
    end
    if loggingEnabled then
        printt(cameraId)
        printt(playerId)
    end

    Entity:setTransformComponent(cameraId, cameraTransformComponent)
end
