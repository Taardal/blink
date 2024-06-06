require("utils")

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
    local newCameraPosition = glm.lerp(cameraPosition, targetCameraPosition, positionLerpFactor * timestep)

    local forward = (playerPosition - newCameraPosition):normalize()
    local right = glm.cross(WORLD_UP_DIRECTION, forward):normalize()
    local up = glm.cross(forward, right):normalize()

    local cameraView = glm.lookAt(newCameraPosition, playerPosition, up)
    local targetCameraOrientation = glm.mat4ToQuat(cameraView):normalize()

    local orientationSlerpFactor = 5
    local newCameraOrientation = glm.slerp(cameraOrientation, targetCameraOrientation, orientationSlerpFactor * timestep)
    newCameraOrientation = newCameraOrientation:normalize()

    --local eulerAngles = glm.degrees(glm.eulerAngles(newCameraOrientation))

    Entity:setTransformComponent(entity, {
        position = newCameraPosition,
        orientation = newCameraOrientation,
        --rightDirection = (newCameraOrientation * WORLD_RIGHT_DIRECTION):normalize(),
        --upDirection = (newCameraOrientation * WORLD_UP_DIRECTION):normalize(),
        --forwardDirection = (newCameraOrientation * WORLD_FORWARD_DIRECTION):normalize(),
        --pitch = eulerAngles.x,
        --yaw = eulerAngles.y,
        --roll = eulerAngles.z,
    })

    --local viewRotation = newCameraOrientation:toMat4()
    --local viewTranslation = glm.translate(glm.mat4(1.0), -newCameraPosition)
    --
    --Entity:setCameraComponent(entity, {
    --    view = viewRotation * viewTranslation
    --})
end
