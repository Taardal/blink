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

    -- Calculate the offset based on the player's yaw and pitch
        local offset = glm.vec3(
            distanceBehindPlayer * math.sin(math.rad(playerTransformComponent.yaw + YAW_ANGLE_OFFSET)) * math.cos(math.rad(playerTransformComponent.pitch)),
            heightAbovePlayer - distanceBehindPlayer * math.sin(math.rad(playerTransformComponent.pitch)),
            -distanceBehindPlayer * math.cos(math.rad(playerTransformComponent.yaw + YAW_ANGLE_OFFSET)) * math.cos(math.rad(playerTransformComponent.pitch))
        )

        -- Set camera position relative to the player's position
        cameraTransformComponent.position = Vector.add(playerTransformComponent.position, offset)

    cameraTransformComponent.yaw = playerTransformComponent.yaw
    cameraTransformComponent.pitch = playerTransformComponent.pitch

    cameraTransformComponent.forwardDirection = playerTransformComponent.forwardDirection
    cameraTransformComponent.rightDirection = playerTransformComponent.rightDirection
    cameraTransformComponent.upDirection = playerTransformComponent.upDirection

    Entity:setTransformComponent(cameraId, cameraTransformComponent)
end
