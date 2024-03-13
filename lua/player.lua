function Player.onUpdate(timestep, entityId)
  print("Hello Player")
  print("-- timestep [" .. timestep .. "]")
  print("-- entityId [" .. entityId .. "]")

  Entity:sayHello()

  local uuid = Entity:getUUID(entityId)
  print("-- uuid [" .. uuid .. "]")

  local position = Entity:getPosition(entityId)
  print("-- position [" .. position.x .. ", " .. position.y .. ", " .. position.z .. "]")

  local aPressed = Keyboard:isPressed(Key.A)
  print("-- A key is pressed [" .. tostring(aPressed) .. "]")

  local aPressed1 = Keyboard:isPressed("A")
    print("-- A string is pressed [" .. tostring(aPressed1) .. "]")
end