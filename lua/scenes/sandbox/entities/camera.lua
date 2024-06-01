local heightAbovePlayer = 8
local distanceBehindPlayer = 30
local loggingEnabled = false

function Camera.onUpdate(entity, timestep)
    local cameraTransformComponent = Entity:getTransformComponent(entity)

    local playerEntity = Entity:getEntityByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerEntity)

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

    local playerPosition = playerTransformComponent.position
    local playerOrientation = playerTransformComponent.orientation

    local playerToCameraOffset = glm.vec3(0, heightAbovePlayer, distanceBehindPlayer)
    local playerToCameraWorldOffset = glm.rotate(playerOrientation, playerToCameraOffset)

    local cameraPosition = playerPosition + playerToCameraWorldOffset

    local cameraView = glm.lookAt(cameraPosition, playerPosition, WORLD_UP_DIRECTION)
    local cameraOrientation = glm.toQuat(cameraView)

    cameraTransformComponent.position = cameraPosition
    cameraTransformComponent.orientation = cameraOrientation

    Entity:setTransformComponent(entity, cameraTransformComponent)

    Entity:setCameraComponent(entity, {
        view = cameraView
    })
end
