local Vector = require("vector")

local YAW_ANGLE_OFFSET = -90

function Camera.onUpdate(cameraId, timestep)
    local cameraTransformComponent = Entity:getTransformComponent(cameraId)

    local playerId = Entity:getIdByTag("Player")
    local playerTransformComponent = Entity:getTransformComponent(playerId)

    local heightAbovePlayer = 0
    local distanceBehindPlayer = 2

    --cameraTransformComponent.position.x = playerTransformComponent.position.x + distanceBehindPlayer * math.sin(math.rad(playerTransformComponent.yaw + YAW_ANGLE_OFFSET))
    --cameraTransformComponent.position.y = playerTransformComponent.position.y + math.sin(math.rad(playerTransformComponent.pitch))
    --cameraTransformComponent.position.z = playerTransformComponent.position.z - distanceBehindPlayer * math.cos(math.rad(playerTransformComponent.yaw + YAW_ANGLE_OFFSET))

    local yawRadians = math.rad(playerTransformComponent.yaw + YAW_ANGLE_OFFSET)
    local pitchRadians = math.rad(playerTransformComponent.pitch)

    -- Calculate the offset based on the player's yaw and pitch
    local offset = glm.vec3()
    offset.x = distanceBehindPlayer * math.sin(yawRadians) * math.cos(pitchRadians)
    offset.y = heightAbovePlayer - distanceBehindPlayer * math.sin(pitchRadians)
    offset.z = -distanceBehindPlayer * math.cos(yawRadians) * math.cos(pitchRadians)

    -- Set camera position relative to the player's position
    cameraTransformComponent.position = Vector.add(playerTransformComponent.position, offset)

    cameraTransformComponent.yaw = playerTransformComponent.yaw
    cameraTransformComponent.pitch = playerTransformComponent.pitch

    cameraTransformComponent.forwardDirection = playerTransformComponent.forwardDirection
    cameraTransformComponent.rightDirection = playerTransformComponent.rightDirection
    cameraTransformComponent.upDirection = playerTransformComponent.upDirection

    Entity:setTransformComponent(cameraId, cameraTransformComponent)
end
