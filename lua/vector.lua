local Vector = {}

function Vector.tostring(v)
	return "x: " .. v.x .. ", y: " .. v.y .. ", z: " .. v.z
end

function Vector.add(a, b)
    if type(b) == "number" then
        return {
            x = a.x + b,
            y = a.y + b,
            z = a.z + b,
        }
    end
    return {
        x = a.x + b.x,
        y = a.y + b.y,
        z = a.z + b.z,
    }
end

function Vector.subtract(a, b)
    if type(b) == "number" then
        return {
            x = a.x - b,
            y = a.y - b,
            z = a.z - b,
        }
    end
    return {
        x = a.x - b.x,
        y = a.y - b.y,
        z = a.z - b.z,
    }
end

function Vector.multiply(a, b)
    if type(b) == "number" then
        return {
            x = a.x * b,
            y = a.y * b,
            z = a.z * b,
        }
    end
    return {
        x = a.x * b.x,
        y = a.y * b.y,
        z = a.z * b.z,
    }
end

return Vector
