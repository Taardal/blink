local moveSpeed = 5.0

function Player.onUpdate(entityId, timestep)
  print("Player.onUpdate")
  print("-- entityId [" .. entityId .. "]")
  print("-- timestep [" .. timestep .. "]")

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