local heightAbovePlayer = 20
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

    local offset = glm.vec3(0, heightAbovePlayer, distanceBehindPlayer)
    local worldOffset = glm.rotate(playerOrientation, offset)
    local targetCameraPosition = playerPosition + worldOffset

    local positionLerpFactor = 5
    local newCameraPosition = glm.lerp(cameraPosition, targetCameraPosition, positionLerpFactor * timestep);

    local worldUp = WORLD_UP_DIRECTION

    local forward = glm.normalize(playerPosition - newCameraPosition)
    local right = glm.normalize(glm.cross(worldUp, forward))
    local up = glm.normalize(glm.cross(forward, right))

    -- Gimbal lock
    local cameraView = glm.lookAt(newCameraPosition, playerPosition, up)
    local targetCameraOrientation = glm.mat4ToQuat(cameraView)

    -- Gimbal lock
    local invForward = glm.vec3(-forward.x, -forward.y, -forward.z)
    cameraView = {
        right,
        up,
        invForward,
    }
    --targetCameraOrientation = glm.mat3ToQuat(cameraView)

    -- No problems, but does not aim at player
    --targetCameraOrientation = playerTransformComponent.orientation

    local orientationSlerpFactor = 5
    local newCameraOrientation = glm.slerp(cameraOrientation, targetCameraOrientation, orientationSlerpFactor * timestep)

    cameraTransformComponent.position = newCameraPosition
    cameraTransformComponent.orientation = newCameraOrientation

    local rotation = glm.quatToMat4(newCameraOrientation)
    --rotation = glm.quatToMat4(glm.inverseQuat(newCameraOrientation))

    local translation = glm.translate(glm.mat4(1.0), glm.vec3(-newCameraPosition.x, -newCameraPosition.y, -newCameraPosition.z))
    local cameraView = rotation * translation

    Entity:setTransformComponent(entity, cameraTransformComponent)
    Entity:setCameraComponent(entity, { view = cameraView })
end
