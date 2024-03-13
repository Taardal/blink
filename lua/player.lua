local moveSpeed = 5.0

function Player.onUpdate(timestep, entityId)
  print("Hello Player")
  print("-- timestep [" .. timestep .. "]")
  print("-- entityId [" .. entityId .. "]")

  local position = Entity:getPosition(entityId)
  local velocity = moveSpeed * timestep
  if (Keyboard:isPressed(Key.Numpad_4)) then
      position.x = position.x - velocity
  end
  if (Keyboard:isPressed(Key.Numpad_6)) then
      position.x = position.x + velocity
  end
  if (Keyboard:isPressed(Key.Numpad_5)) then
      position.y = position.y - velocity
  end
  if (Keyboard:isPressed(Key.Numpad_8)) then
      position.y = position.y + velocity
  end
  print("-- position [" .. position.x .. ", " .. position.y .. ", " .. position.z .. "]")
  Entity:setPosition(entityId, position)
end