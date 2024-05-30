local heightAbovePlayer = 8
local distanceBehindPlayer = 30
local loggingEnabled = false

function Camera.onUpdate(entity, timestep)

    local cameraTransformComponent = Entity:getTransformComponent(entity)

    local playerEntity = Entity:getEntityByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerEntity)

    cameraTransformComponent.position.x = playerTransformComponent.position.x
    cameraTransformComponent.position.y = playerTransformComponent.position.y + heightAbovePlayer
    cameraTransformComponent.position.z = playerTransformComponent.position.z + distanceBehindPlayer

    cameraTransformComponent.pitch = -10
    --cameraTransformComponent.forwardDirection = playerTransformComponent.forwardDirection

    if Keyboard:isPressed(Key.B) then
        loggingEnabled = true
    end
    if Keyboard:isPressed(Key.V) then
        loggingEnabled = false
    end
    if loggingEnabled then
        printt(entity)
        printt(playerEntity)
    end

    Entity:setTransformComponent(entity, cameraTransformComponent)
end
