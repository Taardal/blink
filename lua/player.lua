local moveSpeed = 5.0

function Player.onUpdate(entityId, timestep)
  local position = Entity:getPosition(entityId)
  local velocity = moveSpeed * timestep
  if (Keyboard:isPressed(Key.H) or Keyboard:isPressed(Key.Numpad_4)) then
      position.x = position.x - velocity
  end
  if (Keyboard:isPressed(Key.L) or Keyboard:isPressed(Key.Numpad_6)) then
      position.x = position.x + velocity
  end
  if (Keyboard:isPressed(Key.J) or Keyboard:isPressed(Key.Numpad_5)) then
      position.y = position.y - velocity
  end
  if (Keyboard:isPressed(Key.K) or Keyboard:isPressed(Key.Numpad_8)) then
      position.y = position.y + velocity
  end
  Entity:setPosition(entityId, position)
end
