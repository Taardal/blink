local Vector = require("vector")

local moveSpeed = 2.5

function Player.onUpdate(entityId, timestep)
    local transformComponent = Entity:getTransformComponent(entityId)

    local inputEnabled = true

    if inputEnabled then
    	local velocity = moveSpeed * timestep
        if Keyboard:isPressed(Key.W) then
            transformComponent.position = Vector.add(transformComponent.position, Vector.multiply(transformComponent.forwardDirection, velocity))
        end
        if Keyboard:isPressed(Key.S) then
            transformComponent.position = Vector.subtract(transformComponent.position, Vector.multiply(transformComponent.forwardDirection, velocity))
        end
        if Keyboard:isPressed(Key.D) then
            transformComponent.position = Vector.add(transformComponent.position, Vector.multiply(transformComponent.rightDirection, velocity))
        end
        if Keyboard:isPressed(Key.A) then
            transformComponent.position = Vector.subtract(transformComponent.position, Vector.multiply(transformComponent.rightDirection, velocity))
        end
    end

    if Keyboard:isPressed(Key.Numpad_6) or Keyboard:isPressed(Key.L) then
        transformComponent.yaw = transformComponent.yaw + 1
    end
    if Keyboard:isPressed(Key.Numpad_4) or Keyboard:isPressed(Key.H) then
        transformComponent.yaw = transformComponent.yaw - 1
    end
    if Keyboard:isPressed(Key.Numpad_8) or Keyboard:isPressed(Key.K) then
        transformComponent.pitch = transformComponent.pitch - 1
    end
    if Keyboard:isPressed(Key.Numpad_5) or Keyboard:isPressed(Key.J) then
        transformComponent.pitch = transformComponent.pitch + 1
    end
    if Keyboard:isPressed(Key.Numpad_9) or Keyboard:isPressed(Key.O) then
        transformComponent.roll = transformComponent.roll + 1
    end
    if Keyboard:isPressed(Key.Numpad_7) or Keyboard:isPressed(Key.U) then
        transformComponent.roll = transformComponent.roll - 1
    end
    if transformComponent.pitch > 89.0 then
        transformComponent.pitch = 89.0
    end
    if transformComponent.pitch < -89.0 then
        transformComponent.pitch = -89.0
    end

    transformComponent.forwardDirection.x = math.cos(math.rad(transformComponent.yaw)) * math.cos(math.rad(transformComponent.pitch));
    transformComponent.forwardDirection.y = math.sin(math.rad(transformComponent.pitch));
    transformComponent.forwardDirection.z = math.sin(math.rad(transformComponent.yaw)) * math.cos(math.rad(transformComponent.pitch));
    transformComponent.forwardDirection = glm.normalize(transformComponent.forwardDirection);

    transformComponent.rightDirection = glm.normalize(glm.cross(transformComponent.forwardDirection, transformComponent.worldUpDirection));
    transformComponent.upDirection = glm.normalize(glm.cross(transformComponent.rightDirection, transformComponent.forwardDirection));

    Entity:setTransformComponent(entityId, transformComponent)
end
