local offset = glm.vec3(0, 20, 30)

function Camera.onUpdate(entity, timestep)
    local cameraTransformComponent = Entity:getTransformComponent(entity)

    local playerEntity = Entity:getEntityByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerEntity)

    local cameraPosition = cameraTransformComponent.position
    local cameraOrientation = cameraTransformComponent.orientation

    local playerPosition = playerTransformComponent.position
    local playerOrientation = playerTransformComponent.orientation

    local worldOffset = glm.rotate(playerOrientation, offset)
    local targetCameraPosition = playerPosition + worldOffset

    local positionLerpFactor = 5
    local newCameraPosition = glm.lerp(cameraPosition, targetCameraPosition, positionLerpFactor * timestep);

    local forward = glm.normalize(playerPosition - newCameraPosition)
    local right = glm.normalize(glm.cross(WORLD_UP_DIRECTION, forward))
    local up = glm.normalize(glm.cross(forward, right))

    local cameraView = glm.lookAt(newCameraPosition, playerPosition, up)
    local targetCameraOrientation = glm.normalizeQuat(glm.mat4ToQuat(cameraView))

    local orientationSlerpFactor = 5
    local newCameraOrientation = glm.normalizeQuat(glm.slerp(cameraOrientation, targetCameraOrientation, orientationSlerpFactor * timestep))

    local rotation = glm.quatToMat4(newCameraOrientation)
    local translation = glm.translate(glm.mat4(1.0), glm.vec3(-newCameraPosition.x, -newCameraPosition.y, -newCameraPosition.z))
    local newCameraView = rotation * translation

    Entity:setTransformComponent(entity, {
        position = newCameraPosition,
        orientation = newCameraOrientation,
    })
    Entity:setCameraComponent(entity, {
        view = newCameraView
    })
end
