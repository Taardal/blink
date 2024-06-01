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

    local cameraPosition = cameraTransformComponent.position
    local cameraOrientation = cameraTransformComponent.orientation

    local playerPosition = playerTransformComponent.position
    local playerOrientation = playerTransformComponent.orientation

    local targetPlayerToCameraOffset = glm.vec3(0, heightAbovePlayer, distanceBehindPlayer)
    local targetPlayerToCameraWorldOffset = glm.rotate(playerOrientation, targetPlayerToCameraOffset)
    local targetCameraPosition = playerPosition + targetPlayerToCameraWorldOffset

    local positionLerpFactor = 5
    local newCameraPosition = glm.lerp(cameraPosition, targetCameraPosition, positionLerpFactor * timestep);

    local cameraView = glm.lookAt(newCameraPosition, playerPosition, WORLD_UP_DIRECTION)
    local targetCameraOrientation = glm.toQuat(cameraView)

    local orientationSlerpFactor = 5
    local newCameraOrientation = glm.slerp(cameraOrientation, targetCameraOrientation, orientationSlerpFactor * timestep)
    --print("newCameraOrientation: " .. "x " .. newCameraOrientation.x .. ", y " .. newCameraOrientation.y .. ", z " .. newCameraOrientation.z .. ", w " .. newCameraOrientation.w);

    cameraTransformComponent.position = newCameraPosition
    cameraTransformComponent.orientation = newCameraOrientation

    Entity:setTransformComponent(entity, cameraTransformComponent)

    --Entity:setCameraComponent(entity, {
    --    view = cameraView
    --})
end
