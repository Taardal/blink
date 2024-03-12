function Player.onUpdate(timestep, entityId)
  print("Hello Player")
  print("-- timestep [" .. timestep .. "]")
  print("-- entityId [" .. entityId .. "]")

  Entity:sayHello()

  local foo = Entity.sayFoo()
  print("-- foo [" .. foo .. "]")

  local uuid = Entity:getUUID(entityId)
  print("-- uuid [" .. uuid .. "]")
end