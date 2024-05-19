-- ChatGPT:
--
-- In numerical computing, epsilon (often referred to as machine epsilon or floating-point epsilon) is a measure of
-- the smallest difference between two representable floating-point numbers. It gives an upper bound on the relative
-- error due to rounding in floating-point arithmetic.
--
-- In this context, epsilon is used to check if a floating-point result is close enough to zero to be considered
-- zero. This is necessary because floating-point calculations can introduce very small errors.
local EPSILON = 1e-7;

function clampNumberToZero(number)
    if math.abs(number) < EPSILON then
        return 0
    end
    return number
end

function clampVectorToZero(vector)
    if math.abs(vector.x) < EPSILON then
        vector.x = 0
    end
    if math.abs(vector.y) < EPSILON then
        vector.y = 0
    end
    if math.abs(vector.z) < EPSILON then
        vector.z = 0
    end
end

function formatv(vector)
    return "x: " .. vector.x .. ", y: " .. vector.y .. ", z: " .. vector.z
end

function printv(vector)
    print(formatv(vector))
end

function printtc(transformComponent, tag)
    local position = transformComponent.position
    local forwardDirection = transformComponent.forwardDirection
    local rightDirection = transformComponent.rightDirection
    local upDirection = transformComponent.upDirection
    local yaw = transformComponent.yaw
    local pitch = transformComponent.pitch
    local roll = transformComponent.roll

    if tag ~= nil and string.len(tag) > 0 then
    	print("--------------------------------------------------------------------------------------------------------------")
    	print(tag)
    end
    print("--------------------------------------------------------------------------------------------------------------")
    print("Position: [" .. position.x .. ", " .. position.y .. ", " .. position.z .. "]")
    print("")
    print("Forward direction: [" .. forwardDirection.x .. ", " .. forwardDirection.y .. ", " .. forwardDirection.z .. "]")
    print("Up direction: [" .. upDirection.x .. ", " .. upDirection.y .. ", " .. upDirection.z .. "]")
    print("Right direction: [" .. rightDirection.x .. ", " .. rightDirection.y .. ", " .. rightDirection.z .. "]")
    print("")
    print("Yaw: " .. yaw .. "°")
    print("Pitch: " .. pitch .. "°")
    print("Roll: " .. roll .. "°")
    print("--------------------------------------------------------------------------------------------------------------")
    print("")
end

function printt(entityId)
    local transformComponent = Entity:getTransformComponent(entityId)
    local tagComponent = Entity:getTagComponent(entityId)
	printtc(transformComponent, tagComponent.tag)
end