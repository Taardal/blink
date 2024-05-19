function Scene.onCreate()
    -- --------------------------------------------------------------------------------------------------------------
    -- Scene camera
    -- --------------------------------------------------------------------------------------------------------------

    SceneCamera:setMoveSpeed(25.0)
    SceneCamera:setRotationSpeed(1.0)
    SceneCamera:setPosition(glm.vec3(0, 0, 0))
    SceneCamera:setWorldUpDirection(glm.vec3(0, 1, 0))
    SceneCamera:setForwardDirection(glm.vec3(0, 0, -1))
    SceneCamera:setFrustum({
        fieldOfView = math.rad(45.0),
        nearClip = 0.1,
        farClip = 10000.0,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Terrain
    -- --------------------------------------------------------------------------------------------------------------

    local terrainEntityId = Entity:create()

    Entity:setTagComponent(terrainEntityId, {
        tag = "Terrain"
    })
    Entity:setMeshComponent(terrainEntityId, {
        modelPath = "models/terrain/terrain.obj",
        textureAtlasPath = "models/terrain/diff2.jpg",
    })
    Entity:setTransformComponent(terrainEntityId, {
        position = glm.vec3(0, -50, 0),
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Player
    -- --------------------------------------------------------------------------------------------------------------

    local playerEntityId = Entity:create()

    Entity:setTagComponent(playerEntityId, {
        tag = "Player"
    })
    Entity:setLuaComponent(playerEntityId, {
        type = "Player",
        path = "lua/entities/player.out",
    })
    Entity:setMeshComponent(playerEntityId, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld1.jpg",
    })
    Entity:setTransformComponent(playerEntityId, {
        position = glm.vec3(0, 0, 0),
        yawOffset = 180,
        size = glm.vec3(10, 10, 10)
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Player camera
    -- --------------------------------------------------------------------------------------------------------------

    local playerCameraEntityId = Entity:create()

    Entity:setTagComponent(playerCameraEntityId, {
        tag = "Camera"
    })
    Entity:setLuaComponent(playerCameraEntityId, {
        type = "Camera",
        path = "lua/entities/camera.out",
    })
    Entity:setMeshComponent(playerCameraEntityId, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(playerCameraEntityId, {
        position = glm.vec3(0, 0, 0),
        size = glm.vec3(0.05, 0.05, 0.05),
        yawOffset = 90.0,
        pitchOffset = -90.0,
    })
    Entity:setCameraComponent(playerCameraEntityId, {})

    -- --------------------------------------------------------------------------------------------------------------
    -- Line patrol fighter jet squadron
    -- --------------------------------------------------------------------------------------------------------------

    local linePatrol1EntityId = Entity:create()

    Entity:setTagComponent(linePatrol1EntityId, {
        tag = "Line patrol fighter jet 1"
    })
    Entity:setLuaComponent(linePatrol1EntityId, {
        type = "AiPatrolLine",
        path = "lua/entities/ai_patrol_line.out",
    })
    Entity:setMeshComponent(linePatrol1EntityId, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld4.jpg",
    })
    Entity:setTransformComponent(linePatrol1EntityId, {
        position = glm.vec3(0, 50, 0),
        yaw = 180,
        yawOffset = 180,
        size = glm.vec3(100, 100, 100)
    })

    local linePatrol2EntityId = Entity:create()

    Entity:setTagComponent(linePatrol2EntityId, {
        tag = "Line patrol fighter jet 2"
    })
    Entity:setLuaComponent(linePatrol2EntityId, {
        type = "AiPatrolLine",
        path = "lua/entities/ai_patrol_line.out",
    })
    Entity:setMeshComponent(linePatrol2EntityId, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld5.jpg",
    })
    Entity:setTransformComponent(linePatrol2EntityId, {
        position = glm.vec3(0, 50, -100),
        yaw = 180,
        yawOffset = 180,
        size = glm.vec3(100, 100, 100)
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Oxar freighter
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighterEntityId = Entity:create()

    Entity:setTagComponent(oxarFreighterEntityId, {
        tag = "Oxar freighter"
    })
    Entity:setMeshComponent(oxarFreighterEntityId, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighterEntityId, {
        position = glm.vec3(-405, 260, -577),
        yawOffset = 180,
        size = glm.vec3(10, 10, 10)
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Sibenik cathedral
    -- --------------------------------------------------------------------------------------------------------------

    local sibenikEntityId = Entity:create()

    Entity:setTagComponent(sibenikEntityId, {
        tag = "Sibenik"
    })
    Entity:setMeshComponent(sibenikEntityId, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenikEntityId, {
        position = glm.vec3(-161.29372, -7.5, -54.056705),
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- Viking room
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoomEntityId = Entity:create()

    Entity:setTagComponent(vikingRoomEntityId, {
        tag = "Viking room"
    })
    Entity:setMeshComponent(vikingRoomEntityId, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoomEntityId, {
        position = glm.vec3(-100, -25, -900),
        yaw = -225,
        pitchOffset = -90,
        size = glm.vec3(100, 100, 100)
    })
end
